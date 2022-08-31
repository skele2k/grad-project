#pragma once
#include "AddressMapElement.h"

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
	int invalidPages;

public :
	Block(int numberOfPages);
	~Block();
	int write(int pagesNeeded, AddressMapElement* addrElem, int sectorNumber);
	void erase();
	void markBlock(int idx, int pages, int mark);
	bool isFull();
	int getNumberOfInvalids();
};

