#include "Block.h"

Block::Block(int numberOfPages): full(false), NUMBEROFPAGES(numberOfPages) {
	this->block = new int[NUMBEROFPAGES];
	erase();
}

Block::~Block() {
	delete [] this->block;
}

void Block::markBlock(int idx, int pages, int mark) {
	if (mark == INVALID) {
		this->invalidPages += pages;
	}
	for (int i = idx; i < idx + pages; ++i) {
		this->block[i] = mark;
	}
}

void Block::erase() {
	for (int i = 0; i < NUMBEROFPAGES; ++i) {
		this->block[i] = FREE;
	}
	this->invalidPages = 0;
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

int Block::getNumberOfInvalids() {
	return this->invalidPages;
}