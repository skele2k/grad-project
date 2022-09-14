#include "FTL.h"
#include <tensorflow/c/c_api.h>
#include <array>

int main() {
	//parameters : number of ssd blocks, blktrace block size, page size(KB), number of pages in one block
	//FTL ftl(128000, 512, 16, 256);
	//128GB - Memory size from README of training data workload. But out of memory happens.
	//FTL ftl(2, 512, 16, 256);//for test
	//FTL ftl(1000, 512, 16, 256);//3.9GB. for Test Data.
	//ftl.getIOCommand();
	//printf("Hello from TensorFlow C library version %s\n", TF_Version());
	auto* run_options = TF_NewBufferFromString("", 0);
	auto* session_options = TF_NewSessionOptions();
	auto* graph = TF_NewGraph();
	auto* status = TF_NewStatus();
	std::array<char const*, 1> tags{ "serve" };
	auto* session = TF_LoadSessionFromSavedModel(session_options, run_options,
		"data/saved_model", tags.data(), tags.size(),
		graph, nullptr, status);
	if (TF_GetCode(status) != TF_OK) {
		std::cout << TF_Message(status) << '\n';
	}

	auto* input_op1 = TF_GraphOperationByName(graph, "serving_default_input_1");
	if (input_op1 == nullptr) {
		std::cout << "Failed to find graph operation\n";
	}
	auto* input_op2 = TF_GraphOperationByName(graph, "serving_default_input_2");
	if (input_op2 == nullptr) {
		std::cout << "Failed to find graph operation\n";
	}
	auto* output_op = TF_GraphOperationByName(graph, "StatefulPartitionedCall_1");
	if (output_op == nullptr) {
		std::cout << "Failed to find graph operation\n";
	}

	std::array<TF_Output, 1> input_ops1 = { TF_Output{ input_op1, 0 } };
	std::array<TF_Output, 1> input_ops2 = { TF_Output{ input_op2, 0 } };
	std::array<TF_Output, 1> output_ops = { TF_Output{ output_op, 0 } };
	system("pause");
}