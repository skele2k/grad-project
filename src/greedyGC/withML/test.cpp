#include <iostream>
//#include "tensorflow/c/c_api.h"
#include <array>
#include <vector>
#include <map>
#include <fstream>
#include "Sector.h"
#include "FTL.h"
#include <string>
#include <time.h>

//#define WINDOW_SIZE 128
//#define NINPUT 2

int main() {
    //parameters : number of ssd blocks, blktrace block size, page size(KB), number of pages in one block, temperature flag, trigger threshold, gc threshold
   // FTL ftl(128000, 512, 16, 256);//500GB
    //128GB - Memory size from README of training data workload. But out of memory happens.
    FTL ftl(1200, 512, 4, 128, HWLC, 0.26, 0.84);//2GB
    //FTL ftl(200, 512, 16, 256, HWLC, 0.2, 0.8);//3.9GB. for Test Data.
    //FTL ftl(2560, 512, 16, 256);//10GB. for Test Data.

    char line[101];



    //std::map<unsigned int, char> label;
    std::cout << "Input file name : ";
    char in_filename[20];
    std::cin >> in_filename;
    std::ifstream in_file(in_filename, std::ios::in);
    if (in_file.fail()) {
        std::cout << "File open failed!" << std::endl;
        return 0;
    }

    int countcount = 0;
    float one = 0;
    float two = 0;
    float three = 0;
    float four = 0;
    
    std::vector <char> predictionVals;

    while (true) {
        if (countcount < 3) {
            in_file.getline(line, 50, ',');
            if (in_file.fail()) break;
            if (countcount == 0) {
                one = std::stod(line);
            }
            else if (countcount == 1) {
                two = std::stod(line);
            }
            else {
                three = std::stod(line);
            }
        }
        else {
            in_file.getline(line, 50, '\n');
            four = std::stod(line);

            if (one >= two && one >= three && one >= four) {
                predictionVals.push_back(HOT);
            }
            else if (two >= one && two >= three && two >= four) {
                predictionVals.push_back(WARM);
            }
            else if (three >= one && three >= two && three >= four) {
                predictionVals.push_back(COOL);
            }
            else {
                predictionVals.push_back(COLD);
            }

            countcount = -1;
        }
        ++countcount;

    }
    countcount = 0;
    in_file.close();


    /*
    if (in_file.fail()) {
        std::cout << "File open failed!" << std::endl;
        return 0;
    }
    char word[50]{ '\0' };
    in_file.getline(line, 100, '\n');
    while (true) {
        in_file.getline(line, 100, '\n');
        if (in_file.fail()) break;
        char cluster = line[strlen(line) - 1];
        for (int i = 0; i < strlen(line); ++i) {
            if (line[i] == ',') {
                word[i] = '\0';
                break;
            }
            word[i] = line[i];
        }
        switch (cluster) {
        case '0': {label.insert({ std::stoi(word), HOT }); break; }//        1test       3fi      2fiv
        //case '1': {label.insert({ std::stoi(word), HOT }); break; }
        case '2': { label.insert({ std::stoi(word), WARM }); break; }// 3test      0fi      0fiv
        case '1': { label.insert({ std::stoi(word), COOL }); break; }//   0test       2fi      3fiv
        default: {label.insert({ std::stoi(word), COLD }); break; }
        }
    }

    in_file.close();*/
    


    std::cout << "Input file name : ";
    std::cin >> in_filename;
    //std::ifstream in_file(in_filename, std::ios::in);
    in_file.open(in_filename, std::ios::in);
    if (in_file.fail()) {
        std::cout << "File open failed!" << std::endl;
        return 0;
    }

    int count = 0;
    int progress = 0;
    Sector sector;
    //in_file.getline(line, 100, '\n');
    time_t startTime = time(nullptr);
    /*
    while (true) {
        if (count < 6) {
            in_file.getline(line, 50, ',');
            if (in_file.fail()) break;
            switch (count) {
            case 2: {sector.Clear(); break; }
            case 3: {sector.SetId(std::stoi(line)); break; }
            case 4: {sector.SetSize(std::stoi(line)); break; }
            }
        }
        else {
            in_file.getline(line, 50, '\n');
            if (std::stof(line) > 0.5) {
                sector.SetTemperature('h');
            }
            else {
                sector.SetTemperature('c');
            }
            if (!ftl.issueIOCommand(sector)) {
                return -1;
            }
            ++progress;
            if (progress % 500000 == 0) {//to show how many commands processed.
                std::cout << progress << " processed" << std::endl;
            }
            count = -1;
        }
        ++count;

    }*/
    while (true) {
        if (count < 2) {
            in_file.getline(line, 50, ',');
            if (in_file.fail()) break;
            if (count == 0) {
                sector.Clear();//first column is time table. its only used for ML model.
            }
            else {
                sector.SetId(std::stoi(line));
            }
        }
        else {
            in_file.getline(line, 50, '\n');
            sector.SetSize(std::stoi(line));
            
            /*auto found = label.find(sector.GetId());
            if (found == label.end()) 
                return -1;
            sector.SetTemperature(found->second);*/
           sector.SetTemperature(predictionVals[countcount++]);
            if (!ftl.issueIOCommand(sector)) {
                return -1;
            }
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

    in_file.close();
    ftl.printResult();
    system("pause");
}
