#pragma once
#include <vector>

class Sector
{
private:
	unsigned int  sector_id_;
	unsigned int size_;
	char temperature_;
	
public:
	Sector();
	Sector(unsigned int id, unsigned int size, char temp);
	void operator =(const Sector& other);
	void SetId(unsigned int id);
	unsigned int GetId();
	void SetSize(unsigned int size);
	unsigned int GetSize();
	void SetTemperature(char temperature);
	char GetTemperature();

	void Clear();
};

