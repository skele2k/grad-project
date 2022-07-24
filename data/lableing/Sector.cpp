#include "Sector.h"
#include <string>

Sector::Sector(char* id, double time, unsigned int frequency, double total, double avg, long size) {
	SetId(id);
	SetTimeStamp(time);
	SetFrequency(frequency);
	SetTotal(total);
	SetAvg(avg);
	SetSize(size);
}

Sector::Sector() {
	Clear();
}

void Sector::operator =(const Sector& other) {
	SetId(other.sector_id_);
	SetTimeStamp(other.last_Time_Stamp_);
	SetFrequency(other.frequency_);
	SetTotal(other.total_);
	SetAvg(other.avg_);
	SetSize(other.size_);
}
bool Sector::operator <(const Sector& other) {
	return GetFrequency() < other.frequency_;
}
bool Sector::operator <=(const Sector& other) {
	return GetFrequency() <= other.frequency_;
}
bool Sector::operator >(const Sector& other) {
	return this->GetFrequency() > other.frequency_;
}
bool Sector::operator >=(const Sector& other) {
	return this->GetFrequency() >= other.frequency_;
}

void Sector::SetTimeStamp(double time) {
	this->last_Time_Stamp_ = time;
}

double Sector::GetTimeStamp() {
	return this->last_Time_Stamp_;
}

void Sector::SetFrequency(unsigned int freq) {
	this->frequency_ = freq;
}

unsigned int Sector::GetFrequency() {
	return this->frequency_;
}

void Sector::SetTotal(double total) {
	this->total_ = total;
}

double Sector::GetTotal() {
	return this->total_;
}

void Sector::SetAvg(double avg) {
	this->avg_ = avg;
}

double Sector::GetAvg() {
	return this->avg_;
}

void Sector::SetId(char* id) {
	if (id == nullptr) {
		sector_id_[0] = 0;
	}
	else {
		strcpy_s(this->sector_id_, id);
	}
}

void Sector::SetId(const char* id) {
	if (id == nullptr) {
		sector_id_[0] = 0;
	}
	else {
		strcpy_s(this->sector_id_, id);
	}
}

char* Sector::GetId() {
	return this->sector_id_;
}

void Sector::SetSize(long size) {
	this->size_ = size;
}

long Sector::GetSize() {
	return this->size_;
}


void Sector::Clear()
{
	// set each field to an empty string
	SetId("\0");
	SetTimeStamp(0);
	SetFrequency(0);
	SetTotal(0);
	SetAvg(0);
	SetSize(0);
}

void Sector::Calc(const Sector& other) {
	this->frequency_ += other.frequency_;
	double time_diff = other.last_Time_Stamp_ - this->last_Time_Stamp_;
	this->total_ = time_diff + other.total_;
	this->avg_ = this->total_ / (this->frequency_ - 1);
	this->size_ = this->size_ + other.size_;
}

std::ostream& operator << (std::ostream& stream, const Sector& sector) {
	stream << sector.sector_id_ << "," << sector.frequency_ <<
		"," << sector.avg_ << "," << sector.size_;
	return stream;
}