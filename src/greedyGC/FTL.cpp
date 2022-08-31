#include "FTL.h"


FTL::FTL(int numberOfSSDBlocks, int blockSize, int pageSize, int numberOfPages):
NUMBEROFSSDBLOCKS(numberOfSSDBlocks), BLOCKSIZE(blockSize), PAGESIZE(pageSize),
NUMBEROFPAGES(numberOfPages), KB(1024), writeAmplification(0) {
	for (int i = 0; i < NUMBEROFSSDBLOCKS; ++i) {
		this->freeBlock.push_back(new Block(NUMBEROFPAGES));
	}
}

void FTL::invalidateOverlapped(std::map<unsigned int, AddressMapElement*>::iterator cur) {
	cur->second->markInvalid();
	delete cur->second;
}

void FTL::markOverlapped(std::map<unsigned int, AddressMapElement*>::iterator found) {
	std::map<unsigned int, AddressMapElement*>::iterator cur = found;
	++cur;
	while (cur != addressTable.end()) {
		if ((found->first + found->second->getNumberOfBlocks() - 1) >= cur->first) {
			invalidateOverlapped(cur);
			std::map<unsigned int, AddressMapElement*>::iterator oneToDelete = cur++;
			addressTable.erase(oneToDelete);
		}
		else {
			break;
		}
	}

	cur = found;
	if (found != addressTable.begin()) {
		--cur;
		if (cur->first + cur->second->getNumberOfBlocks() - 1 >= found->first) {
			invalidateOverlapped(cur);
			addressTable.erase(cur);
		}
	}
}

void FTL::issueIOCommand(Sector& sector) {
	int pagesNeeded = std::ceil((sector.GetSize() * BLOCKSIZE) / (double)(PAGESIZE * KB));
	int last = pagesNeeded;
	std::map<unsigned int, AddressMapElement*>::iterator found = addressTable.find(sector.GetId());

	if (found != addressTable.end()) {
		found->second->markInvalid();
		found->second->clear();
	}
	else {
		AddressMapElement* addrElem = new AddressMapElement;
		addressTable.insert({ sector.GetId(), addrElem });
		found = addressTable.find(sector.GetId());
	}
	found->second->setNumberOfBlocks(sector.GetSize());
	markOverlapped(found);
	while(true ) {
		pagesNeeded = freeBlock[0]->write(pagesNeeded, found->second, found->first);
		if (last != pagesNeeded) {
			if (freeBlock[0]->isFull()) {
				fullBlock.insert({ NUMBEROFPAGES - freeBlock[0]->getNumberOfInvalids(), freeBlock[0] });
				freeBlock.erase(freeBlock.begin());
			}
			 if (pagesNeeded == 0) {
				return;
			}
			 last = pagesNeeded;
		}
		if ((freeBlock.size() / (float)NUMBEROFSSDBLOCKS) < 0.31) {
			//gcgc
			bool eraseFailed = false;
			if (eraseFailed) {
				std::cout << "out of memory" << std::endl;
				return;
			}
		}
	}
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
	Sector sector;

	while (true) {
		if (count < 2) {
			in_file.getline(line, 50, ',');
			if (in_file.fail()) break;
			if (count == 0) {
				sector.Clear();
			}
			else {
				sector.SetId(std::stoi(line));
			}
		}
		else {
			in_file.getline(line, 50, '\n');
			sector.SetSize(std::stoi(line));
			issueIOCommand(sector);
			count = -1;
		}
		++count;

	}
	in_file.close();
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
	clearBlockMap(fullBlock);
	clearBlockMap(addressTable);
}