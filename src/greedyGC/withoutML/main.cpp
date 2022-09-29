#include "FTL.h"
#include <time.h>

int main() {
	//parameters : number of ssd blocks, blktrace block size, page size(KB), number of pages in one block, trigger threshold, gc threshold
	//FTL ftl(120000, 512, 16, 256, 0.26, 0.84);//500GB 128000
	//128GB - Memory size from README of training data workload. But out of memory happens.
	FTL ftl(1200, 512, 4, 128, 0.26, 0.84);//700 Fi1, 
	//FTL ftl(1650, 512, 4, 128, 0.26, 0.84);//3.9GB. for Test Data.
	//FTL ftl(700, 512, 16, 256);//Fi1V
	//FTL ftl(1200, 512, 4, 128, 0.3, 0.6);//3.9GB. for Test Data.
	//time_t start = time(nullptr);
	ftl.getIOCommand();
//	time_t end = time(nullptr);

//	std::cout << "Time : " << end - start << " seconds" << std::endl;
	system("pause");
}