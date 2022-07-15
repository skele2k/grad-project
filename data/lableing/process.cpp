#include <iostream>
#include <fstream>
#include <string>
#include "Sector.h"

int main() {
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
					out_file << sector_last;
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
	out_file << sector_last;
	in_file.close();
	out_file.close();
}
