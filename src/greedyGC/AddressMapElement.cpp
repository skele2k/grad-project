#include "AddressMapElement.h"

AddressMapElement::AddressMapElement():numberOfBlocks(0){}

AddressMapElement::~AddressMapElement() {
	clear();
}

void AddressMapElement::addBlock(Block* block, int offset, int page) {
	this->blocks.push_back(block);
	this->offsets.push_back(offset);
	this->pages.push_back(page);
}

void AddressMapElement::clear() {
	this->blocks.clear();
	this->offsets.clear();
	this->pages.clear();
	this->numberOfBlocks = 0;
}

std::vector<int> AddressMapElement::markInvalid() {
	std::vector<int> lastFreePages;
	for (int i = 0; i < this->blocks.size(); ++i) {
		if (blocks[i]->isFull()) {
			lastFreePages.push_back(this->blocks[i]->getNumberOfFreePages());
		}
		this->blocks[i]->markBlock(this->offsets[i], this->pages[i], INVALID);
	}
	return lastFreePages;
}

void AddressMapElement::setNumberOfBlocks(unsigned int number) {
	this->numberOfBlocks = number;
}

unsigned int AddressMapElement::getNumberOfBlocks() {
	return this->numberOfBlocks;
}
