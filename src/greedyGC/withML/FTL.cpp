#include "FTL.h"


FTL::FTL(int numberOfSSDBlocks, int blockSize, int pageSize, int numberOfPages, int temprFlag, double trigger, double gcThresh):
NUMBEROFSSDBLOCKS(numberOfSSDBlocks), BLOCKSIZE(blockSize), PAGESIZE(pageSize * 1024),
NUMBEROFPAGES(numberOfPages), writeAmplification(0), temperatureFlag(temprFlag), TRIGGERTHRESH(trigger), GCTHRESH(gcThresh),
fullBlock(numberOfPages+1, std::vector<Block*>(0,0)), nandWrite(0), requestedWrite(0), freeCold(nullptr),
freeCool(nullptr), freeHot(nullptr), freeWarm(nullptr) {
	for (int i = 0; i < NUMBEROFSSDBLOCKS; ++i) {
		this->freeBlock.push_back(new Block(NUMBEROFPAGES, i));
	}
	freeHot = freeBlock[0];
	freeBlock.erase(freeBlock.begin());
	freeCold = freeBlock[0];
	freeBlock.erase(freeBlock.begin());
	
	if (temperatureFlag != HC) {
		freeWarm = freeBlock[0];
		freeBlock.erase(freeBlock.begin());
		freeWarm->setTemperature(WARM);
	}
	if (temperatureFlag == HWLC) {
		freeCool = freeBlock[0];
		freeBlock.erase(freeBlock.begin());
		freeCool->setTemperature(COOL);
	}

	freeHot->setTemperature(HOT);
	freeCold->setTemperature(COLD);
}

void FTL::relocateFullBlockIdx(std::map<int, int> lastValidPages) {
	for (auto iter = lastValidPages.begin(); iter != lastValidPages.end(); ++iter) {
		int lastValidPage = iter->second;
		for (auto fullIter = fullBlock[lastValidPage].begin(); fullIter != fullBlock[lastValidPage].end(); ++fullIter) {
			if ((*fullIter)->getNumberOfValidPages() != lastValidPage) {//idx and number of free pages are different
				fullBlock[(*fullIter)->getNumberOfValidPages()].push_back((*fullIter));//make idx and number of free pages same
				fullBlock[lastValidPage].erase(fullIter);
				break;
			}
		}
	}
}

void FTL::invalidateOverlapped(std::map<unsigned int, AddressMapElement*>::iterator cur) {
	relocateFullBlockIdx(cur->second->markInvalid());
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
		if ((cur->first + cur->second->getNumberOfBlocks() - 1) >= found->first) {//checking wether new LBA is overlapping prior LBA.
			//check only once because all other possibilities are already invalidated.
			invalidateOverlapped(cur);
		}
	}
}

int FTL::numberOfNotNullPt() {
	int count = 0;
	if (this->freeHot != nullptr) {
		++count;
	}
	if (this->freeCold != nullptr) {
		++count;
	}
	if (temperatureFlag != HC) {
		if (this->freeWarm != nullptr) {
			++count;
		}
	}
	if (temperatureFlag == HWLC) {
		if (this->freeCool != nullptr) {
			++count;
		}
	}
	return count;
}

bool FTL::greedyGC() {
	std::map<int, int> relocInfos;//sector id, pages
	int result = 0;
	for (auto fullBlockIter = fullBlock.begin(); fullBlockIter != fullBlock.end(); ++fullBlockIter) {
		//ERASELIMIT means blocks with more than 10% of invalid pages will be erased.
		while (fullBlockIter->begin() != fullBlockIter->end()) {
			Block* cur = (*fullBlockIter->begin());
			char temperature = cur->getTemperature();
			relocInfos = cur->erase();
			freeBlock.push_back(cur);
			result = 0;
			for (auto iter = relocInfos.begin(); iter != relocInfos.end(); ++iter) {//relocating valid pages to another block.
				auto found = addressTable.find(iter->first);//if LBA is not found whole algorithm is in error.
				found->second->unlinkBlock(cur);
				this->nandWrite += iter->second;
				result = writeInDrive(iter->second, found, temperature);
				if (result != 0) {
					return false;//this means no space available anymore.
				}
			}
			fullBlockIter->erase(fullBlockIter->begin());
			
			if ((static_cast<double>(freeBlock.size() + fullBlock[NUMBEROFPAGES].size() + numberOfNotNullPt())
				/ static_cast<double>(NUMBEROFSSDBLOCKS)) > GCTHRESH) {
				return true;
			}
		}
	}
	return true;
}

int FTL::writeByTemp(int pagesNeeded, std::map<unsigned int, AddressMapElement*>::iterator found, Block* &block, char temperature) {
	if (block == nullptr) {
		if (freeBlock.size() == 0) {
			return -1;
		}
		block = freeBlock[0];
		freeBlock.erase(freeBlock.begin());
		block->setTemperature(temperature);
	}
	while (freeBlock.size() > 0 || !(block->isFull())) {
		if (pagesNeeded == 0) {
			return 0;
		}
		pagesNeeded = block->write(pagesNeeded, found->second, found->first);
		if (block->isFull()) {
			if (block->getBlockID() == 30) {
				int a = 10;
			}
			fullBlock[block->getNumberOfValidPages()].push_back(block);
			if (freeBlock.size() == 0) {
				block = nullptr;
				break;
			}
			block = freeBlock[0];
			freeBlock.erase(freeBlock.begin());
			block->setTemperature(temperature);
		}
	}
	return pagesNeeded;
}

int FTL::writeInDrive(int pagesNeeded, std::map<unsigned int, AddressMapElement*>::iterator found, char temperature) {
	int result = -1;
	switch (temperature) {
	case HOT: {result = writeByTemp(pagesNeeded, found, freeHot, temperature); break; }
	case COLD: {result = writeByTemp(pagesNeeded, found, freeCold, temperature); break; }
	case WARM: {
		if (temperatureFlag != HC) {
			result = writeByTemp(pagesNeeded, found, freeWarm, temperature);
		}
		break; 
	}
	case COOL: {
		if (temperatureFlag == HWLC) {
			result = writeByTemp(pagesNeeded, found, freeCool, temperature);
		}
		break;
	}
	}

	
	return result;
}

std::map<unsigned int, AddressMapElement*>::iterator FTL::checkSectorIdExist(std::map<unsigned int, AddressMapElement*>::iterator found, unsigned int sectorId) {
	if (found != addressTable.end()) {
		relocateFullBlockIdx(found->second->markInvalid());//if address already exist, it need to be invalidated
		found->second->clear();
	}
	else {
		found = addressTable.insert({ sectorId, new AddressMapElement }).first;
	}
	return found;
}

bool FTL::issueIOCommand(Sector& sector) {
	int pagesNeeded = static_cast<int>(std::ceil((sector.GetSize() * BLOCKSIZE) / static_cast<double>(PAGESIZE)));
	this->requestedWrite += pagesNeeded;
	bool success = true;
	int result = 0;
	std::map<unsigned int, AddressMapElement*>::iterator found = checkSectorIdExist(addressTable.find(sector.GetId()), sector.GetId());
	if (sector.GetSize() < 1) return true;

	found->second->setNumberOfBlocks(sector.GetSize());
	markOverlapped(found);
	result = writeInDrive(pagesNeeded, found, sector.GetTemperature());
	if (result != 0) {//means no spaces available forr to write.
		if (greedyGC()) {
			if (result < 0) {
				result =  writeInDrive(pagesNeeded, found, sector.GetTemperature());
			}
			else {
				result = writeInDrive(result, found, sector.GetTemperature());
			}
			success = (result == 0);
		}
		else {//try write after gc. if it fails memory out of space.
			success = false;
		}
	}
	else {
		if ((static_cast<double>(freeBlock.size() + fullBlock[NUMBEROFPAGES].size() + numberOfNotNullPt()) / 
			static_cast<double>(NUMBEROFSSDBLOCKS)) < TRIGGERTHRESH) {
			//free blocks + all valid blocks / number of total blocks < 0.31
			success = greedyGC();
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

template <class Elem>
void clearBlockMap(std::map<unsigned int, Elem*> mapItem) {
	for (std::map<unsigned int, Elem*>::iterator i = mapItem.begin(); i != mapItem.end(); ++i) {
		delete i->second;
	}
	mapItem.clear();
}

FTL::~FTL() {
	clearBlockVector(freeBlock);
	for (auto iter = fullBlock.begin(); iter != fullBlock.end(); ++iter) {
		clearBlockVector(*iter);
	}
	fullBlock.clear();
	if (freeHot != nullptr)
		delete freeHot;
	if (freeCold != nullptr)
		delete freeCold;
	if (freeWarm != nullptr)
		delete freeWarm;
	if (freeCool != nullptr)
		delete freeCool;
	clearBlockMap(addressTable);
}

void FTL::printResult() {
	std::cout << "Requested Write : " << this->requestedWrite << std::endl <<
		"Additional Write : " << this->nandWrite << std::endl <<
		"Nand Write : " << this->requestedWrite + this->nandWrite << std::endl <<
		"Write Amplification : " << static_cast<double>(this->requestedWrite + this->nandWrite) / static_cast<double>(requestedWrite) << std::endl;
}