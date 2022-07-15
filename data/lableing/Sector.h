#pragma once
#include <iostream>
class Sector
{
private:
	char sector_id_[20];
	double last_Time_Stamp_;
	unsigned int frequency_;
	double total_;
	double avg_;
	long size_;

public:
	Sector();
	Sector(char* id, double time, unsigned int frequency, double total, double avg, long size);
	void operator =(const Sector& other);
	bool operator <(const Sector& other);
	bool operator <=(const Sector& other);
	bool operator >(const Sector& other);
	bool operator >=(const Sector& other);
	void SetId(char* id);
	void SetId(const char* id);
	char* GetId();
	void SetTimeStamp(double time);
	double GetTimeStamp();
	void SetFrequency(unsigned int freq);
	unsigned int GetFrequency();
	void SetTotal(double total);
	double GetTotal();
	void SetAvg(double avg);
	double GetAvg();
	void SetSize(long size);
	long GetSize();

	void Clear();
	void Calc(const Sector& other);
	friend std::ostream& operator << (std::ostream& stream, const Sector& sector);
};

