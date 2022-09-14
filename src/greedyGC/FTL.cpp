#include "FTL.h"


FTL::FTL(int numberOfSSDBlocks, int blockSize, int pageSize, int numberOfPages):
NUMBEROFSSDBLOCKS(numberOfSSDBlocks), BLOCKSIZE(blockSize), PAGESIZE(pageSize),
NUMBEROFPAGES(numberOfPages), KB(1024), ERASELIMIT((NUMBEROFPAGES/10)+1), writeAmplification(0),
fullBlock(numberOfPages+1, std::vector<Block*>(0,0)) {
	for (int i = 0; i < NUMBEROFSSDBLOCKS; ++i) {
		this->freeBlock.push_back(new Block(NUMBEROFPAGES));
	}
}

void FTL::relocateFullBlockElem(std::vector<int> lastValidPages) {
	for (int i = 0; i < lastValidPages.size(); ++i) {
		int lastValidPage = lastValidPages[i];
		for (int j = 0; j < fullBlock[lastValidPage].size(); ++j) {
			if (fullBlock[lastValidPage][j]->getNumberOfValidPages() != lastValidPage) {//idx and number of free pages are different
				fullBlock[fullBlock[lastValidPage][j]->getNumberOfValidPages()].push_back(fullBlock[lastValidPage][j]);//make idx and number of free pages same
				fullBlock[lastValidPage].erase(fullBlock[lastValidPage].begin() + j);
				--j;
			}
		}
	}
}

void FTL::invalidateOverlapped(std::map<unsigned int, AddressMapElement*>::iterator cur) {
	relocateFullBlockElem(cur->second->markInvalid());
	delete cur->second;
	addressTable.erase(cur);
}

void FTL::markOverlapped(std::map<unsigned int, AddressMapElement*>::iterator found) {
	std::map<unsigned int, AddressMapElement*>::iterator cur = found;
	++cur;
	while (cur != addressTable.end()) {
		if ((found->first + found->second->getNumberOfBlocks() - 1) >= cur->first) {//LBA + blocks -1 >= next LBA means overlapped.
			//If writing 2 blocks on LBA 0 means writing 0, 1. So LBA 2 is free page. 0+2-1=1
			std::map<unsigned int, AddressMapElement*>::iterator oneToDelete = cur++;
			invalidateOverlapped(oneToDelete);
		}
		else {
			break;
		}
	}

	cur = found;
	if (found != addressTable.begin()) {
		--cur;
		if (cur->first + cur->second->getNumberOfBlocks() - 1 >= found->first) {//checking wether new LBA is overlapping prior LBA.
			//check only once because all other possibilities are already invalidated.
			invalidateOverlapped(cur);
		}
	}
}

bool FTL::greedyGC() {
	std::vector<Relocater> relocInfos;
	for (auto fullBlockIter = fullBlock.begin(); fullBlockIter != (fullBlock.end()-ERASELIMIT); ++fullBlockIter) {
		//ERASELIMIT means blocks with more than 10% of invalid pages will be erased.
		while (0 < fullBlockIter->size()) {
			relocInfos = (*fullBlockIter->begin())->erase();
			freeBlock.push_back(*fullBlockIter->begin());
			for (int i = 0; i < relocInfos.size(); ++i) {//relocating valid pages to another block.
				auto found = addressTable.find(relocInfos[i].sectorNumber);//if LBA is not found whole algorithm is in error.
				found->second->unlinkBlock(*fullBlockIter->begin());
				this->writeAmplification += relocInfos[i].pages;
				if (!writeInDrive(relocInfos[i].pages, found)) {
					return false;//this means no space available anymore.
				}
			}
			fullBlockIter->erase(fullBlockIter->begin());
		}
	}
	return true;
}

bool FTL::writeInDrive(int& pagesNeeded, std::map<unsigned int, AddressMapElement*>::iterator found) {
	while (true) {
		if (freeBlock.size() == 0) break;
		pagesNeeded = freeBlock[0]->write(pagesNeeded, found->second, found->first);
		if (freeBlock[0]->isFull()) {
			fullBlock[freeBlock[0]->getNumberOfValidPages()].push_back(freeBlock[0]);
			freeBlock.erase(freeBlock.begin());
		}
		if (pagesNeeded == 0) {
			return true;
		}
	}
	if (pagesNeeded > 0) {
		return false;
	}
	return true;
}

std::map<unsigned int, AddressMapElement*>::iterator FTL::checkSectorIdExist(std::map<unsigned int, AddressMapElement*>::iterator found, unsigned int sectorId) {
	if (found != addressTable.end()) {
		relocateFullBlockElem(found->second->markInvalid());//if address already exist, it need to be invalidated
		found->second->clear();
	}
	else {
		AddressMapElement* addrElem = new AddressMapElement;
		addressTable.insert({ sectorId, addrElem });
		found = addressTable.find(sectorId);
	}
	return found;
}

bool FTL::issueIOCommand(Sector& sector) {
	int pagesNeeded = std::ceil((sector.GetSize() * BLOCKSIZE) / (double)(PAGESIZE * KB));
	int last = pagesNeeded;
	bool success = true;
	std::map<unsigned int, AddressMapElement*>::iterator found = checkSectorIdExist(addressTable.find(sector.GetId()), sector.GetId());
	if (sector.GetSize() < 1) return true;

	found->second->setNumberOfBlocks(sector.GetSize());
	markOverlapped(found);
	if (!writeInDrive(pagesNeeded, found)) {//means no spaces available forr to write.
		if (!greedyGC()) {
			success = false;
		}
		else {//try write after gc. if it fails memory out of space.
			success = writeInDrive(pagesNeeded, found);
		}
	}
	if (((freeBlock.size() + fullBlock[NUMBEROFPAGES].size()) / (float)NUMBEROFSSDBLOCKS) < 0.31){
		//free blocks + all valid blocks / number of total blocks < 0.31
		if (!greedyGC()) {
			success = false;
		}
	}
	if (!success) {
		std::cout << "out of memory" << std::endl;
		return false;
	}
	return true;
}

void FTL::clearBlockVector(std::vector<Block*>& block) {
	for (int i = 0; i < block.size(); ++i) {
		delete block[i];
	}
	block.clear();
}



int FTL::getIOCommand() {

	std::cout << "Input file name : ";
	char in_filename[20];
	std::cin >> in_filename;
	std::ifstream in_file(in_filename, std::ios::in);
	if (in_file.fail()) {
		std::cout << "File open failed!" << std::endl;
		return 0;
	}

	int count = 0;
	char line[51];
	int progress = 0;
	Sector sector;

	while (true) {
		if (count < 2) {
			in_file.getline(line, 50, ',');
			if (in_file.fail()) break;
			if (count == 0) {
				sector.Clear();//first column is time table. its only used for ML model.
			}
			else {
				sector.SetId(std::stoi(line));
			}
		}
		else {
			in_file.getline(line, 50, '\n');
			sector.SetSize(std::stoi(line));
			if (!issueIOCommand(sector)) {
				return -1;
			}
			++progress;
			if (progress % 500000 == 0) {//to show how many commands processed.
				std::cout << progress << " processed" << std::endl;
			}
			count = -1;
		}
		++count;

	}
	in_file.close();
	std::cout << "Write Amplification : " << this->writeAmplification << std::endl;
}

template <class Elem>
void clearBlockMap(std::map<unsigned int, Elem*> mapItem) {
	for (std::map<unsigned int, Elem*>::iterator i = mapItem.begin(); i != mapItem.end(); ++i) {
		delete i->second;
	}
	mapItem.clear();
}

FTL::~FTL() {
	clearBlockVector(freeBlock);
	clearBlockMap(addressTable);
	for (auto iter = fullBlock.begin(); iter != fullBlock.end(); ++iter) {
		clearBlockVector(*iter);
	}
	fullBlock.clear();
}