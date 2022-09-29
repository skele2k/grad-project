#pragma once
#include "AddressMapElement.h"
#include <vector>
#include <map>

#define INVALID -2
#define FREE -1
class AddressMapElement;

class Block
{
private :
	bool full;
	int* block;
	int getFreePageIdx();
	const int NUMBEROFPAGES;
	int validPages;
	char temperature;
	const int blockID;

public :
	Block(int numberOfPages, int id);
	~Block();
	int write(int pagesNeeded, AddressMapElement* addrElem, int sectorNumber);
	std::map<int, int> erase();
	void markBlock(int idx, int pages, int mark);
	bool isFull();
	int getNumberOfValidPages();
	void setTemperature(char temp);
	char getTemperature();
	int getBlockID();
};

