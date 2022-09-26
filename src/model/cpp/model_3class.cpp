#include <iostream>
#include "tensorflow/c/c_api.h"
#include <array>
#include <vector>

#define WINDOW_SIZE 128
#define NINPUT 2
#define NOUTPUT 3

using namespace std;

int main(){
    auto *run_options = TF_NewBufferFromString("", 0);
    auto *session_options = TF_NewSessionOptions();
    auto *graph = TF_NewGraph();
    auto *status = TF_NewStatus();
    array<char const *, 1> tags{ "serve" };

    auto* session = TF_LoadSessionFromSavedModel(session_options, run_options,
                                                "data/final_stacked_lstm3", tags.data(), tags.size(),
                                                graph, nullptr, status);
    if (TF_GetCode(status) != TF_OK) {
        cout << TF_Message(status) << '\n';
    }

    auto *input_op1 = TF_GraphOperationByName(graph, "serving_default_input_1");
    if (input_op1 == nullptr) {
        std::cout << "Failed to find graph operation\n";
    }

    auto *input_op2 = TF_GraphOperationByName(graph, "serving_default_input_2");
    if (input_op2 == nullptr) {
        std::cout << "Failed to find graph operation\n";
    }

    auto *output_op = TF_GraphOperationByName(graph, "StatefulPartitionedCall_1");
    if (output_op == nullptr) {
        std::cout << "Failed to find graph operation\n";
    }

    std::array<TF_Output, 2> input_ops = { TF_Output{ input_op1, 0 }, TF_Output{ input_op2, 0}};
    std::array<TF_Output, 1> output_ops = { TF_Output{ output_op, 0 }};

    std::array<double, WINDOW_SIZE> sectorIdData {7487488, 7489536, 7491584, 7493632, 7495680, 7497728, 7499776, 7501824, 7503872, 7505920,
                                                7507968, 7510016, 7512064, 7514112, 7516160, 7518208, 7520256, 7522304, 7524352, 7526400, 7528448,
                                                7530496, 7532544, 7534592, 7553024, 7555072, 7557120, 7559168, 7561216, 7563264, 7565312, 7567360,
                                                7569408, 7571456, 7573504, 7575552, 7577600, 7579648, 7581696, 7583744, 7585792, 7587840,
                                                7589888, 7591936, 7593984, 7596032, 7598080, 7600128, 7602176, 7604224, 7606272,
                                                7608320, 7610368, 7612416, 7614464, 7616512, 7618560, 7620608, 7622656, 7624704,
                                                7626752, 7628800, 7630848, 7632896, 7634944, 7636992, 7639040, 7641088, 7643136,
                                                7645184, 7647232, 7649280, 7798784, 7800832, 7802880, 7804928, 7806976, 7809024,
                                                7811072, 7813120, 7815168,7817216,7819264,7821312,7823360,7825408,7827456,7829504,
                                                7831552,7833600,7835648,7837696,7839744,7841792,7843840,7845888,8011776,8013824,8015872,8017920,8019968,
                                                8022016,8024064,8026112,8028160,8030208,8032256,8034304,8036352,8038400,8040448,8042496,8044544,
                                                8046592,8048640,8050688,8052736,8054784,8056832,8058880,8060928,8062976,8065024,8067072,499608704,499608728,499608728,499608728,
};

    std::array<float, WINDOW_SIZE> timeDeltaData {-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,-0.3910644056,
                                                  -0.3910644056,-0.3910644056 };

    std::vector<std::array<double, WINDOW_SIZE>> sectorId{ sectorIdData};
    std::vector<std::array<float, WINDOW_SIZE>> timeDelta{ timeDeltaData };

    std::array<int64_t, 2> const sectorId_dims{ static_cast<int64_t>(sectorId.size()), static_cast<int64_t>(sectorIdData.size()) };
    std::array<int64_t, 2> const timeDelta_dims{ static_cast<int64_t>(sectorId.size()), static_cast<int64_t>(sectorIdData.size()) };


    void *sectorIdRawdata = (void *) sectorId.data();
    void *timeDeltaRawdata = (void *) timeDelta.data();

    std::size_t const sectorId_ndata = WINDOW_SIZE * TF_DataTypeSize(TF_FLOAT);
    std::size_t const timeDelta_ndata = WINDOW_SIZE * TF_DataTypeSize(TF_FLOAT);

    auto const deallocator = [](void *, std::size_t, void *) {}; // unused deallocator because of RAII

    auto *input_tensor1 = TF_NewTensor(TF_FLOAT, sectorId_dims.data(), sectorId_dims.size(), sectorIdRawdata, sectorId_ndata, deallocator, nullptr);
    auto *input_tensor2 = TF_NewTensor(TF_FLOAT, timeDelta_dims.data(), timeDelta_dims.size(), timeDeltaRawdata, timeDelta_ndata, deallocator, nullptr);

    std::array<TF_Tensor *, 2> input_values{ input_tensor1, input_tensor2 };
    std::array<TF_Tensor *, 1> output_values{};

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

    auto *output_tensor = static_cast<std::array<float, 1> *>(TF_TensorData(output_values[0]));
    std::vector<std::array<float, 1>> outputs{ output_tensor, output_tensor + NOUTPUT};

    // outputs[0][0] -> cold
    // outputs[0][1] -> cool
    // outputs[0][2] -> warm
    std::cout << "output: " << outputs[0][0] << " " << outputs[0][1] << " " << outputs[0][2] << '\n'; 

    // cleanup
    TF_DeleteTensor(input_values[0]);
    TF_DeleteTensor(output_values[0]);
    TF_DeleteBuffer(run_options);
    TF_DeleteSessionOptions(session_options);
    TF_DeleteSession(session, status);
    TF_DeleteGraph(graph);
    TF_DeleteStatus(status);

    return 0;
}