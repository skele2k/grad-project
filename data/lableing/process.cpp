#include <iostream>
#include <fstream>
#include <string>
#include "Sector.h"

#define TESTFRQSTD 10
#define TESTAVGSTD 0.51

#define TRAINFRQSTD 10
#define TRAINAVGSTD 35.9

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
	char line[50];
	char last_id[50];

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
	char line[50];
	char last_id[50];

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

int main() {
	//PostProcess();
	Label(TRAINFRQSTD, TRAINAVGSTD);
}
