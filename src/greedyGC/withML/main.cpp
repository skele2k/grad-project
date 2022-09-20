#include "FTL.h"
#include <tensorflow/c/c_api.h>
#include <array>
#include <algorithm>
#include <numeric>
#define WINDOW_SIZE 128
#define NINPUT 2

int main() {
	//parameters : number of ssd blocks, blktrace block size, page size(KB), number of pages in one block
	//FTL ftl(128000, 512, 16, 256);//500GB
	//128GB - Memory size from README of training data workload. But out of memory happens.
	//FTL ftl(2, 512, 16, 256);//for test
	FTL ftl(1000, 512, 16, 256);//3.9GB. for Test Data.

    std::map<unsigned int, float> lastEncounter;
    std::cout << "Input file name : ";
    char in_filename[20];
    std::cin >> in_filename;
    std::ifstream in_file(in_filename, std::ios::in);
    if (in_file.fail()) {
        std::cout << "File open failed!" << std::endl;
        return 0;
    }

    int count = 0;
    char line[51];
    int progress = 0;
    Sector sector;
    float timeStamp = 0;
    float timeDiff = 0;
    //double timeDiffSum = 0;
    //unsigned int timeDiffAmount = 0;
    //std::vector<float> timeDiffs;

    auto* run_options = TF_NewBufferFromString("", 0);
    auto* session_options = TF_NewSessionOptions();
    auto* graph = TF_NewGraph();
    auto* status = TF_NewStatus();
    std::array<char const*, 1> tags{ "serve" };

    auto* session = TF_LoadSessionFromSavedModel(session_options, run_options,
        "data/plain_lstm32_20ep", tags.data(), tags.size(),
        graph, nullptr, status);
    if (TF_GetCode(status) != TF_OK) {
        std::cout << TF_Message(status) << '\n';
    }

    auto* input_op1 = TF_GraphOperationByName(graph, "serving_default_input_1");
    if (input_op1 == nullptr) {
        std::cout << "Failed to find graph operation\n";
    }

    auto* input_op2 = TF_GraphOperationByName(graph, "serving_default_input_2");
    if (input_op2 == nullptr) {
        std::cout << "Failed to find graph operation\n";
    }

    auto* output_op = TF_GraphOperationByName(graph, "StatefulPartitionedCall_3");
    if (output_op == nullptr) {
        std::cout << "Failed to find graph operation\n";
    }

    std::array<TF_Output, 2> input_ops = { TF_Output{ input_op1, 0 }, TF_Output{ input_op2, 0} };
    std::array<TF_Output, 1> output_ops = { TF_Output{ output_op, 0 } };

    std::array<double, WINDOW_SIZE> sectorIdData{ 0 };
    std::array<float, WINDOW_SIZE> timeDeltaData{ 0 };
   
    std::array<float, WINDOW_SIZE> timeDiffData{ 0 };

    std::vector<std::array<double, WINDOW_SIZE>> sectorId{ sectorIdData };
    std::vector<std::array<float, WINDOW_SIZE>> timeDelta{ timeDeltaData };

    std::array<int64_t, 2> const sectorId_dims{ static_cast<int64_t>(sectorId.size()), static_cast<int64_t>(sectorIdData.size()) };
    std::array<int64_t, 2> const timeDelta_dims{ static_cast<int64_t>(timeDelta.size()), static_cast<int64_t>(timeDeltaData.size()) };

    std::size_t const sectorId_ndata = WINDOW_SIZE * TF_DataTypeSize(TF_FLOAT);
    std::size_t const timeDelta_ndata = WINDOW_SIZE * TF_DataTypeSize(TF_FLOAT);

    auto const deallocator = [](void*, std::size_t, void*) {}; // unused deallocator because of RAII
    std::array<TF_Tensor*, 2> input_values{ };
    std::array<TF_Tensor*, 1> output_values{};
    time_t startTime = time(nullptr);
    while (true) {
        if (count < 2) {
            in_file.getline(line, 50, ',');
            if (in_file.fail()) break;
            if (count == 0) {
                sector.Clear();//first column is time table. its only used for ML model.
                timeStamp = std::stof(line);
            }
            else {
                sector.SetId(std::stoi(line));
            }
        }
        else {
            in_file.getline(line, 50, '\n');
            sector.SetSize(std::stoi(line));
            auto timeElem = lastEncounter.find(sector.GetId());
            if (timeElem == lastEncounter.end()) {
                timeDiff = 0;
                lastEncounter.insert({ sector.GetId(), timeStamp });
            }
            else {
                timeDiff = timeStamp - timeElem->second;
                timeElem->second = timeStamp;
                //timeDiffSum += timeDiff;
                //++timeDiffAmount;
                //timeDiffs.push_back(timeDiff);
            }
            memcpy(&sectorIdData[0], &sectorIdData[1], sizeof(double) * (WINDOW_SIZE - 1));
            memcpy(&timeDeltaData[0], &timeDeltaData[1], sizeof(float) * (WINDOW_SIZE - 1));
            memcpy(&timeDiffData[0], &timeDiffData[1], sizeof(float) * (WINDOW_SIZE - 1));
            sectorIdData[WINDOW_SIZE - 1] = sector.GetId();
            timeDiffData[WINDOW_SIZE - 1] = timeDiff;

            double sum = std::accumulate(timeDiffData.begin(), timeDiffData.end(), 0.0);
            double mean = sum / timeDiffData.size();
            double accum = 0.0;
            std::for_each(timeDiffData.begin(), timeDiffData.end(), [&](const double d) {
                accum += (d - mean) * (d - mean);
                });

            double stdev = sqrt(accum / (timeDiffData.size()));
            for (int i = 0; i < WINDOW_SIZE; ++i) {
                if (timeDiffData[i] > 0) {
                    timeDeltaData[i] = (timeDiffData[i] - mean) / stdev;
                }
                else {
                    timeDeltaData[i] = 0;
                }
            }

            sectorId[0] = sectorIdData;
            timeDelta[0] = timeDeltaData;

            void* sectorIdRawdata = (void*)sectorId.data();
            void* timeDeltaRawdata = (void*)timeDelta.data();
            auto* input_tensor1 = TF_NewTensor(TF_FLOAT, sectorId_dims.data(), sectorId_dims.size(), sectorIdRawdata, sectorId_ndata, deallocator, nullptr);
            auto* input_tensor2 = TF_NewTensor(TF_FLOAT, timeDelta_dims.data(), timeDelta_dims.size(), timeDeltaRawdata, timeDelta_ndata, deallocator, nullptr);

            input_values[0] = input_tensor1;
            input_values[1] = input_tensor2;


            TF_SessionRun(session,
                run_options,
                input_ops.data(), input_values.data(), input_ops.size(),
                output_ops.data(), output_values.data(), output_ops.size(),
                nullptr, 0,
                nullptr,
                status);
            if (TF_GetCode(status) != TF_OK) {
                std::cout << TF_Message(status) << '\n';
            }
            if (*static_cast<float*>(TF_TensorData(output_values[0])) > 0.5) {
                sector.SetTemperature('h');
            }
            else {
                sector.SetTemperature('c');
            }
            
            if (!ftl.issueIOCommand(sector)) {
                return -1;
            }

            TF_DeleteTensor(input_values[0]);
            TF_DeleteTensor(input_values[1]);
            TF_DeleteTensor(output_values[0]);
            ++progress;
            if (progress % 500000 == 0) {//to show how many commands processed.
                std::cout << progress << " processed" << std::endl;
            }
            count = -1;
        }
        ++count;

    }
    time_t endTime = time(nullptr);
    std::cout << "Time : " << endTime - startTime << " seconds" << std::endl;
    TF_DeleteBuffer(run_options);
    TF_DeleteSessionOptions(session_options);
    TF_DeleteSession(session, status);
    TF_DeleteGraph(graph);
    TF_DeleteStatus(status);


   in_file.close();
   ftl.printResult();
	system("pause");
}