#include "Block.h"

Block::Block(int numberOfPages): full(false), NUMBEROFPAGES(numberOfPages), freePages(numberOfPages) {
	this->block = new int[NUMBEROFPAGES]{FREE};
}

Block::~Block() {
	delete [] this->block;
}

void Block::markBlock(int idx, int pages, int mark) {
	if (mark == INVALID) {
		this->freePages -= pages;
	}
	for (int i = idx; i < idx + pages; ++i) {
		this->block[i] = mark;
	}
}

std::vector<Relocater> Block::erase() {
	int last = -1;
	Relocater reloc;
	std::vector<Relocater> relocInfos;
	for (int i = 0; i < NUMBEROFPAGES; ++i) {
		if (this->block[i] > FREE) {
			if (last != this->block[i]) {
				if (last >= 0) {
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
	this->freePages = NUMBEROFPAGES;
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

int Block::getNumberOfFreePages() {
	return this->freePages;
}