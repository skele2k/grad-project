#pragma once
#include "AddressMapElement.h"
#include <vector>

#define INVALID -2
#define FREE -1
class AddressMapElement;

typedef struct relocater {
	int sectorNumber;
	int pages;
}Relocater;

class Block
{
private :
	bool full;
	int* block;
	int getFreePageIdx();
	const int NUMBEROFPAGES;
	int validPages;

public :
	Block(int numberOfPages);
	~Block();
	int write(int pagesNeeded, AddressMapElement* addrElem, int sectorNumber);
	std::vector<Relocater> erase();
	void markBlock(int idx, int pages, int mark);
	bool isFull();
	int getNumberOfValidPages();
};

