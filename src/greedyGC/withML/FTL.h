#pragma once

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <map>
#include "AddressMapElement.h"
#include "Block.h"
#include "Sector.h"

#define HOT 'h'
#define COLD 'c'
#define WARM 'w'
#define COOL 'l'
#define HC 1
#define HWC 2
#define HWLC 3

class FTL {
private:
	const int NUMBEROFSSDBLOCKS;
	const int BLOCKSIZE;
	const int PAGESIZE;
	const int NUMBEROFPAGES;
	const double TRIGGERTHRESH;
	const double GCTHRESH;
	unsigned int nandWrite;
	unsigned int requestedWrite;
	double writeAmplification;
	Block* freeCold;
	Block* freeCool;
	Block* freeHot;
	Block* freeWarm;
	const int temperatureFlag;

	void invalidateOverlapped(std::map<unsigned int, AddressMapElement*>::iterator cur);
	std::vector<Block*> freeBlock;
	std::vector<std::vector<Block*>> fullBlock;
	std::map<unsigned int, AddressMapElement*> addressTable;
	void clearBlockVector(std::vector<Block*>& block);
	void markOverlapped(std::map<unsigned int, AddressMapElement*>::iterator found);
	void relocateFullBlockIdx(std::map<int, int> lastValidPages);
	bool greedyGC();
	int writeInDrive(int pagesNeeded, std::map<unsigned int, AddressMapElement*>::iterator found, char temperature);
	std::map<unsigned int, AddressMapElement*>::iterator checkSectorIdExist(std::map<unsigned int, AddressMapElement*>::iterator found, unsigned int sectorId);
	int writeByTemp(int pagesNeeded, std::map<unsigned int, AddressMapElement*>::iterator found, Block* &block, char temperature);
	int FTL::numberOfNotNullPt();
public:
	FTL(int numberOfSSDBlocks, int blockSize, int pageSize, int numberOfPages, int temprFlag, double trigger, double gcThresh);
	bool issueIOCommand(Sector& sector);
	void printResult();
	~FTL();
};