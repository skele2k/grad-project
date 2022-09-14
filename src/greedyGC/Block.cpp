#include "Block.h"

Block::Block(int numberOfPages): full(false), NUMBEROFPAGES(numberOfPages), validPages(numberOfPages) {
	this->block = new int[NUMBEROFPAGES];
	memset(this->block, FREE, sizeof(unsigned int) * NUMBEROFPAGES);
}

Block::~Block() {
	delete [] this->block;
}

void Block::markBlock(int idx, int pages, int mark) {//biggest LBA is still smaller than upper limit of LBA used for training and test data.
	for (int i = idx; i < idx + pages; ++i) {//mark can be LBA, FREE, INVALID
		this->block[i] = mark;
	}
	if (mark == INVALID) {//counting number of invalid pages
		this->validPages -= pages;
	}
}

std::vector<Relocater> Block::erase() {
	int last = FREE;
	Relocater reloc;
	bool isValidExist = false;
	std::vector<Relocater> relocInfos;
	for (int i = 0; i < NUMBEROFPAGES; ++i) {
		if (this->block[i] > FREE) {//FREE and INVALID < 0. LBAs >= 0.
			isValidExist = true;//flag used for to store last valid page information
			if (last != this->block[i]) {
				if (last > FREE) {
					relocInfos.push_back(reloc);
				}
				reloc.sectorNumber = this->block[i];
				reloc.pages = 1;
				last = this->block[i];
			}
			else {
				++(reloc.pages);
			}
		}
		this->block[i] = FREE;
	}
	if (isValidExist) {
		relocInfos.push_back(reloc);//storing last valid page that didnt get stored in loop.
	}
	this->validPages = NUMBEROFPAGES;
	this->full = false;
	return relocInfos;
}

int Block::write(int pagesNeeded, AddressMapElement* addrElem, int sectorNumber) {
	if (this->full) return pagesNeeded;
	int idx = getFreePageIdx();
	int freeSpace = NUMBEROFPAGES - idx;

	if (pagesNeeded >= freeSpace) {
		markBlock(idx, freeSpace, sectorNumber);
		this->full = true;
		addrElem->addBlock(this, idx, freeSpace);
		return pagesNeeded - freeSpace;
	}
	markBlock(idx, pagesNeeded, sectorNumber);
	addrElem->addBlock(this, idx, pagesNeeded);
	return 0;
}

int Block::getFreePageIdx() {
	for (int i = 0; i < NUMBEROFPAGES; ++i) {
		if (this->block[i] == FREE) {
			return i;
		}
	}
}

bool Block::isFull() {
	return this->full;
}

int Block::getNumberOfValidPages() {
	return this->validPages;
}