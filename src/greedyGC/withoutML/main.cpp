#include "FTL.h"
#include <time.h>

int main() {
	//parameters : number of ssd blocks, blktrace block size, page size(KB), number of pages in one block
	//FTL ftl(128000, 512, 16, 256);//500GB
	//128GB - Memory size from README of training data workload. But out of memory happens.
	//FTL ftl(2, 512, 16, 256);//for test
	FTL ftl(1000, 512, 16, 256);//3.9GB. for Test Data.
	time_t start = time(nullptr);
	ftl.getIOCommand();
	time_t end = time(nullptr);

	std::cout << "Time : " << end - start << " seconds" << std::endl;
	system("pause");
}