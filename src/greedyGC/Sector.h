#pragma once
#include <vector>

class Sector
{
private:
	unsigned int  sector_id_;
	unsigned int size_;
	
public:
	Sector();
	Sector(unsigned int id, unsigned int size);
	void operator =(const Sector& other);
	void SetId(unsigned int id);
	unsigned int GetId();
	void SetSize(unsigned int size);
	unsigned int GetSize();

	void Clear();
};

