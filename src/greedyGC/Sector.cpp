#include "Sector.h"

Sector::Sector(unsigned int id, unsigned int size) {
	SetId(id);
	SetSize(size);
}

Sector::Sector() {
	Clear();
}


void Sector::operator =(const Sector& other) {
	SetId(other.sector_id_);
	SetSize(other.size_);
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

void Sector::Clear()
{
	SetId(0);
	SetSize(0);
}
