#include "FTL.h"

int main() {
	FTL ftl(1000, 512, 16, 256);//number of ssd blocks, block size, page size, number of pages
	//FTL ftl(2, 512, 16, 256);
	ftl.getIOCommand();
	system("pause");
}