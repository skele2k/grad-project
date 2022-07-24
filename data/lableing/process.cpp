#include <iostream>
#include <fstream>
#include <string>
#include "Sector.h"

#define TESTFRQSTD 10
#define TESTAVGSTD 0.51

#define TRAINFRQSTD 10
#define TRAINAVGSTD 35.9

#define TESTMAXTIME 41122
#define TRAINMAXTIME 34459

int PostProcess() {
	std::cout << "Input file name : ";
	char in_filename[20];
	std::cin >> in_filename;
	std::ifstream in_file(in_filename, std::ios::in);
	if (in_file.fail()) {
		std::cout << "File open failed!" << std::endl;
		return 0;
	}
	std::cout << "Output file name : ";
	char out_filename[20];
	std::cin >> out_filename;
	std::ofstream out_file(out_filename, std::ios::out);
	if (out_file.fail()) {
		std::cout << "File open failed!" << std::endl;
		return 0;
	}
	Sector sector;
	Sector sector_last;
	sector_last.SetId("null");

	int count = 0;
	char line[51];

	while (1) {
		if (count < 2) {
			in_file.getline(line, 50, ' ');
			if (in_file.fail()) break;
			if (count == 0) {
				sector.SetTimeStamp(std::stod(line));
				sector.SetFrequency(1);
			}
			else {
				sector.SetId(line);
			}
		}
		else {
			in_file.getline(line, 50, '\n');
			sector.SetSize(std::stol(line));
			count = -1;

			if (strcmp(sector.GetId(), sector_last.GetId())) {
				if (strcmp(sector_last.GetId(), "null")) {
					std::cout << sector_last.GetId() << std::endl;
					out_file << sector_last << '\n';
				}
				sector_last = sector;
			}
			else {
				sector_last.Calc(sector);
			}
		}
		++count;

	}
	std::cout << sector_last.GetId() << std::endl;
	out_file << sector_last << '\n';
	in_file.close();
	out_file.close();
}

int Label(const int frequency_std, const float avg_std) {
	std::cout << "Input file name : ";
	char in_filename[20];
	std::cin >> in_filename;
	std::ifstream in_file(in_filename, std::ios::in);
	if (in_file.fail()) {
		std::cout << "File open failed!" << std::endl;
		return 0;
	}
	std::cout << "Output file name : ";
	char out_filename[20];
	std::cin >> out_filename;
	std::ofstream out_file(out_filename, std::ios::out);
	if (out_file.fail()) {
		std::cout << "File open failed!" << std::endl;
		return 0;
	}
	Sector sector;

	int count = 0;
	unsigned int label_count = 0;
	char line[51];

	while (1) {
		if (count < 3) {
			in_file.getline(line, 50, ' ');
			if (in_file.fail()) break;
			switch (count) {
			case 0: {sector.SetId(line);  break; }
			case 1: {sector.SetFrequency(std::stoi(line)); break; }
			case 2: {sector.SetAvg(std::stod(line)); break; }
			}
		}
		else {
			in_file.getline(line, 50, '\n');
			sector.SetSize(std::stol(line));
			count = -1;
			if (sector.GetFrequency() > frequency_std) {
				out_file << sector << " 1\n";
				++label_count;
			}
			else if ((sector.GetAvg() < avg_std) && (sector.GetFrequency() > 1)) {
				out_file << sector << " 1\n";
				++label_count;
			}
			else {
				out_file << sector << " 0\n";
			}
		}
		++count;
	}
	std::cout << "Labeled datas : " << label_count << std::endl;
	out_file << sector << " 0\n";
	in_file.close();
	out_file.close();
}

int timeAvgConvert(const double max_val) {
	std::cout << "Input file name : ";
	char in_filename[20];
	std::cin >> in_filename;
	std::ifstream in_file(in_filename, std::ios::in);
	if (in_file.fail()) {
		std::cout << "File open failed!" << std::endl;
		return 0;
	}
	std::cout << "Output file name : ";
	char out_filename[20];
	std::cin >> out_filename;
	std::ofstream out_file(out_filename, std::ios::out);
	if (out_file.fail()) {
		std::cout << "File open failed!" << std::endl;
		return 0;
	}
	Sector sector;

	int count = 0;
	char line[51];

	while (1) {
		if (count < 3) {
			in_file.getline(line, 50, ',');
			if (in_file.fail()) break;
			switch (count) {
			case 0: {sector.SetId(line);  break; }
			case 1: {
				sector.SetFrequency(std::stoi(line)); break; }
			case 2: {
				if (strcmp(line, "0") == 0) {
					sector.SetAvg(max_val);
				}
				else {
					sector.SetAvg(std::stod(line));
				}
				break;
			}
			}
		}
		else {
			in_file.getline(line, 50, '\n');
			sector.SetSize(std::stol(line));
			count = -1;
			out_file << sector << '\n';
		}
		++count;
	}
	in_file.close();
	out_file.close();
}

int clusterCount() {
	std::cout << "Input file name : ";
	char in_filename[30];
	std::cin >> in_filename;
	std::ifstream in_file(in_filename, std::ios::in);
	if (in_file.fail()) {
		std::cout << "File open failed!" << std::endl;
		return 0;
	}

	Sector sector;

	int count = 0;
	char line[51];

	unsigned int num_count = 0;
	unsigned int marked_count = 0;
	for (int i = 0; i < 4; ++i) {
		in_file.getline(line, 50, ',');
	}
	in_file.getline(line, 50, '\n');
	while (1) {
		if (count < 4) {
			in_file.getline(line, 50, ',');
			if (in_file.fail()) 
				break;
			switch (count) {
			case 0: {break; }
			case 1: {break;  }
			case 2: {break; }
			case 3: {break; }
			}
		}
		else {
			in_file.getline(line, 50, '\n');
			if (strcmp(line, "0") == 0) {
				++marked_count;
			}
			++num_count;
			count = -1;
		}
		++count;
	}
	in_file.close();
	std::cout << "mark count : " << marked_count << "\nnum count : " << num_count << std::endl;
}
int main() {
	//PostProcess();
	//Label(TRAINFRQSTD, TRAINAVGSTD);
	//timeAvgConvert(TRAINMAXTIME);
	clusterCount();
}
