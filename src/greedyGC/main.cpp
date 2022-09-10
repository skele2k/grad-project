#include "FTL.h"

int main() {
	//parameters : number of ssd blocks, blktrace block size, page size(KB), number of pages in one block
	FTL ftl(128000, 512, 16, 256);
	//128GB - Memory size from README of training data workload. But out of memory happens.
	//FTL ftl(2, 512, 16, 256);//for test
	//FTL ftl(1000, 512, 16, 256);//3.9GB. for Test Data.
	ftl.getIOCommand();
	system("pause");
}