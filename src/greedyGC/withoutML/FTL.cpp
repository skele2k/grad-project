#include "FTL.h"

FTL::FTL(int numberOfSSDBlocks, int blockSize, int pageSize, int numberOfPages, double trigger, double gcThresh):
NUMBEROFSSDBLOCKS(numberOfSSDBlocks), BLOCKSIZE(blockSize), PAGESIZE(pageSize * 1024),
NUMBEROFPAGES(numberOfPages), writeAmplification(0), TRIGGERTHRESH(trigger), GCTHRESH(gcThresh),
fullBlock(numberOfPages+1, std::vector<Block*>(0,0)), nandWrite(0), requestedWrite(0), curBlock(nullptr) {
	for (int i = 0; i < NUMBEROFSSDBLOCKS; ++i) {
		this->freeBlock.push_back(new Block(NUMBEROFPAGES, i));
	}
	curBlock = freeBlock[0];
	freeBlock.erase(freeBlock.begin());
}

void FTL::relocateFullBlockIdx(std::map<int, int> lastValidPages) {
	for (auto iter = lastValidPages.begin(); iter != lastValidPages.end(); ++iter) {
		int lastValidPage = iter->second;
		for (auto fullIter = fullBlock[lastValidPage].begin(); fullIter != fullBlock[lastValidPage].end(); ++fullIter) {
			if ((*fullIter)->getNumberOfValidPages() != lastValidPage) {
				fullBlock[(*fullIter)->getNumberOfValidPages()].push_back((*fullIter));
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
	if (this->curBlock != nullptr) {
		++count;
	}
	return count;
}

bool FTL::greedyGC() {
	std::map<int, int> relocInfos;
	int result = 0;
	for (auto fullBlockIter = fullBlock.begin(); fullBlockIter != fullBlock.end(); ++fullBlockIter) {
		//ERASELIMIT means blocks with more than 10% of invalid pages will be erased.
		while (fullBlockIter->begin() != fullBlockIter->end()) {
			Block* cur = (*fullBlockIter->begin());
			relocInfos = cur->erase();
			freeBlock.push_back(cur);
			result = 0;
			for (auto iter = relocInfos.begin(); iter != relocInfos.end(); ++iter) {//relocating valid pages to another block.
				auto found = addressTable.find(iter->first);//if LBA is not found whole algorithm is in error.
				found->second->unlinkBlock(cur);
				this->nandWrite += iter->second;
				result = writeInDrive(iter->second, found);
				if (result != 0) {
					return false;//this means no space available anymore.
				}
			}
			fullBlockIter->erase(fullBlockIter->begin());
			if (static_cast<double>((freeBlock.size() + fullBlock[NUMBEROFPAGES].size() + numberOfNotNullPt()) /
				static_cast<double>(NUMBEROFSSDBLOCKS)) > GCTHRESH) {
				return true;
			}
		}
	}
	return true;
}

int FTL::writeInDrive(int pagesNeeded, std::map<unsigned int, AddressMapElement*>::iterator found) {
	if (curBlock == nullptr) {
		if (freeBlock.size() == 0) {
			return -1;
		}
		curBlock = freeBlock[0];
		freeBlock.erase(freeBlock.begin());
	}
	while (freeBlock.size() > 0 || !(curBlock->isFull())) {
		if (pagesNeeded == 0) {
			return 0;
		}
		pagesNeeded = curBlock->write(pagesNeeded, found->second, found->first);
		if (curBlock->isFull()) {
			fullBlock[curBlock->getNumberOfValidPages()].push_back(curBlock);
			if (freeBlock.size() == 0) {
				curBlock = nullptr;
				break;
			}
			curBlock = freeBlock[0];
			freeBlock.erase(freeBlock.begin());
		}
	}
	return pagesNeeded;
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
	int pagesNeeded = static_cast<int>(std::ceil(static_cast<double>(sector.GetSize() * BLOCKSIZE) / static_cast<double>(PAGESIZE)));
	this->requestedWrite += pagesNeeded;
	bool success = true;
	int result = 0;
	std::map<unsigned int, AddressMapElement*>::iterator found = checkSectorIdExist(addressTable.find(sector.GetId()), sector.GetId());
	if (sector.GetSize() < 1) {
		return true;
	}
	found->second->setNumberOfBlocks(sector.GetSize());
	markOverlapped(found);
	result = writeInDrive(pagesNeeded, found);
	if (result != 0) {//means no spaces available forr to write.
		if (greedyGC()) {
			if (result < 0) {
				result = writeInDrive(pagesNeeded, found);
			}
			else {
				result = writeInDrive(result, found);
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
	char line[101];
	int progress = 0;
	Sector sector;
	//in_file.getline(line, 100, '\n');
	/*
	while (true) {
		if (count < 6) {
			in_file.getline(line, 50, ',');
			if (in_file.fail()) break;
			switch (count) {
			case 2: {sector.Clear(); break; }
			case 3: {sector.SetId(std::stoi(line)); break; }
			case 4: {sector.SetSize(std::stoi(line)); break; }
			}
		}
		else {
			in_file.getline(line, 50, '\n');
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

	}*/
	
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
	std::cout << "Requested Write : " << this->requestedWrite  << std::endl <<
		"Additional Write : " << this->nandWrite << std::endl <<
		"Nand Write : " << this->requestedWrite + this->nandWrite << std::endl <<
		"Write Amplification : " << static_cast<double>(this->requestedWrite + this->nandWrite) / static_cast<double>(requestedWrite) << std::endl;
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
	if (curBlock != nullptr) {
		delete this->curBlock;
	}
	fullBlock.clear();
	clearBlockMap(addressTable);
}