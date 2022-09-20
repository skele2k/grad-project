#include "Sector.h"

Sector::Sector(unsigned int id, unsigned int size, char temp) {
	SetId(id);
	SetSize(size);
	SetTemperature(temp);
}

Sector::Sector() {
	Clear();
}


void Sector::operator =(const Sector& other) {
	SetId(other.sector_id_);
	SetSize(other.size_);
	SetTemperature(other.temperature_);
}

void Sector::SetId(unsigned int id) {
	this->sector_id_ = id;
}


unsigned int Sector::GetId() {
	return this->sector_id_;
}

void Sector::SetSize(unsigned int size) {
	this->size_ = size;
}

unsigned int Sector::GetSize() {
	return this->size_;
}

void Sector::SetTemperature(char temperature) {
	this->temperature_ = temperature;
}

char Sector::GetTemperature() {
	return this->temperature_;
}

void Sector::Clear()
{
	SetId(0);
	SetSize(0);
	SetTemperature('\0');
}
