#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Sector.h"

#define TESTFRQSTD 10
#define TESTAVGSTD 0.51

#define TRAINFRQSTD 10
#define TRAINAVGSTD 35.9

#define TESTMAXTIME 41122
#define TRAINMAXTIME 34459

void clearVector(std::vector<double*>& arr) {
	while (!arr.empty()) {
		double* elem = arr.back();
		delete elem;
		arr.pop_back();
	}
}

double calcStdDeriv(std::vector<double*>& arr, double avg) {
	double sum = 0;
	for (int i = 0; i < arr.size(); ++i) {
		sum += pow(*(arr[i]) - avg, 2);
	}
	return sqrt(sum / arr.size());
}

int PreProcess(const double maxval) {
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
	std::vector<double*> time_arr;

	while (1) {
		if (count < 2) {
			in_file.getline(line, 50, ',');
			if (in_file.fail()) break;
			if (count == 0) {
				double time = std::stod(line);
				sector.SetTimeStamp(time);
				double* time_elem = new double;
				*time_elem = time;
				time_arr.push_back(time_elem);
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
					if (sector_last.GetFrequency() > 2) {
						sector_last.SetStdDeriv(calcStdDeriv(time_arr, sector_last.GetAvg()));
					}
					if (sector_last.GetFrequency() < 2) {
						sector_last.SetAvg(maxval);
					}
					out_file << sector_last << '\n';

				}
				clearVector(time_arr);
				sector_last.Clear();
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

	Sector cluster_zero;
	Sector cluster_one;
	Sector cluster_two;
	Sector cluster_three;
	Sector* cluster = nullptr;

	Sector sector;

	int count = 0;
	char line[51];

	unsigned int num_count = 0;
	unsigned int cluster0_count = 0;
	unsigned int cluster1_count = 0;
	unsigned int cluster2_count = 0;
	unsigned int cluster3_count = 0;
	for (int i = 0; i < 4; ++i) {
		in_file.getline(line, 50, ',');
	}
	in_file.getline(line, 50, '\n');
	while (1) {
		if (count < 5) {
			in_file.getline(line, 50, ',');
			if (in_file.fail()) 
				break;
			switch (count) {
			case 0: {
				sector.Clear();
				break; }
			case 1: {
				sector.SetFrequency(std::stoi(line));
				break;
			}
			case 2: {
				sector.SetAvg(std::stod(line));
				break; 
			}
			case 3: {
				sector.SetStdDeriv(std::stod(line));
				break; 
			}
			case 4: {
				sector.SetSize(std::stol(line));
				break;
			}
			}
		}
		else {
			in_file.getline(line, 50, '\n');
			if (strcmp(line, "0") == 0) {
				++cluster0_count;
				cluster = &cluster_zero;
			}
			else if (strcmp(line, "1") == 0) {
				++cluster1_count;
				cluster = &cluster_one;
			}
			else if (strcmp(line, "2") == 0) {
				++cluster2_count;
				cluster = &cluster_two;
			}
			else {
				++cluster3_count;
				cluster = &cluster_three;
			}
			cluster->SetFrequency(cluster->GetFrequency() + sector.GetFrequency());
			cluster->SetAvg(cluster->GetAvg() + sector.GetAvg());
			cluster->SetStdDeriv(cluster->GetStdDeriv() + sector.GetStdDeriv());
			cluster->SetSize(cluster->GetSize() + sector.GetSize());
			cluster = nullptr;
			++num_count;
			count = -1;
		}
		++count;
	}
	in_file.close();
	unsigned int* count_pt = nullptr;
	for (int i = 0; i < 4; ++i) {
		switch (i) {
		case 0: {	cluster = &cluster_zero; count_pt = &cluster0_count; break; }
		case 1: {	cluster = &cluster_one; count_pt = &cluster1_count; break; }
		case 2: {	cluster = &cluster_two; count_pt = &cluster2_count; break; }
		case 3: {	cluster = &cluster_three; count_pt = &cluster3_count; break;	}
		}
		std::cout << "Cluster " << i << "\nAvg Frequency : " << cluster->GetFrequency() / *count_pt <<
			"\nAvg Time Interval : " << cluster->GetAvg() / *count_pt << "\nAvg Time Std Deriv : " << cluster->GetStdDeriv() / *count_pt <<
			"\nAvg # of blocks : " << cluster->GetSize() / *count_pt << "\nTotal numbers : " << *count_pt << '\n' << std::endl;
	}

	std::cout << "Total number of datas :" << num_count << std::endl;
}
int main() {
	//PreProcess(TESTMAXTIME);
	//Label(TRAINFRQSTD, TRAINAVGSTD);
	//timeAvgConvert(TRAINMAXTIME);
	clusterCount();
}
