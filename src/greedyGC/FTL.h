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
	unsigned int writeAmplification;
	void invalidateOverlapped(std::map<unsigned int, AddressMapElement*>::iterator cur);
	std::vector<Block*> freeBlock;
	std::vector<std::vector<Block*>> fullBlock;
	std::map<unsigned int, AddressMapElement*> addressTable;
	void clearBlockVector(std::vector<Block*>& block);
	bool issueIOCommand(Sector& sector);
	void markOverlapped(std::map<unsigned int, AddressMapElement*>::iterator found);
	void relocateFullBlockElem(std::vector<int> lastValidPages);
	bool greedyGC();
	bool writeInDrive(int pagesNeeded, std::map<unsigned int, AddressMapElement*>::iterator found);
	std::map<unsigned int, AddressMapElement*>::iterator checkSectorIdExist(std::map<unsigned int, AddressMapElement*>::iterator found, unsigned int sectorId);
public:
	FTL(int numberOfSSDBlocks, int blockSize, int pageSize, int numberOfPages);
	~FTL();
	int getIOCommand();

};