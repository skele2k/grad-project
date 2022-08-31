#pragma once

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <map>
#include "AddressMapElement.h"
#include "Block.h"
#include "Sector.h"

class FTL {
private:
	const int NUMBEROFSSDBLOCKS;
	const int BLOCKSIZE;
	const int PAGESIZE;
	const int NUMBEROFPAGES;
	const int KB;
	void invalidateOverlapped(std::map<unsigned int, AddressMapElement*>::iterator cur);
	std::vector<Block*> freeBlock;
	std::map<unsigned int, Block*> fullBlock;
	std::map<unsigned int, AddressMapElement*> addressTable;
	void clearBlockVector(std::vector<Block*>& block);
	void issueIOCommand(Sector& sector);
	void markOverlapped(std::map<unsigned int, AddressMapElement*>::iterator found);
	unsigned int writeAmplification;

public:
	FTL(int numberOfSSDBlocks, int blockSize, int pageSize, int numberOfPages);
	~FTL();
	int getIOCommand();

};