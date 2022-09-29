#pragma once
#include <vector>
#include <map>
#include "Block.h"


class Block;
class AddressMapElement
{
private:
	std::vector<Block*> blocks;
	std::vector<int> offsets;
	std::vector<int> pages;
	unsigned int numberOfBlocks;

public:
	AddressMapElement();
	~AddressMapElement();
	void addBlock(Block* block, int offset, int page);
	void setNumberOfBlocks(unsigned int number);
	unsigned int getNumberOfBlocks();
	std::map<int, int> markInvalid();
	void unlinkBlock(Block* block);
	void clear();

};

