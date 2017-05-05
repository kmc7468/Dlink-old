#include <vector>
#include <memory>
#include <iostream>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "CL/cl.hpp"

int main(int argc, char* argv[])
{
	// LLVM test code
	std::cout << "--- LLVM test begin ---\n";

	llvm::LLVMContext context;
	auto module = std::make_unique<llvm::Module>("top", context);
	llvm::IRBuilder<> builder(context);

	std::vector<llvm::Type*> main_args; //Empty argument
	llvm::FunctionType* main_type = llvm::FunctionType::get(builder.getInt32Ty(), main_args, false);
	llvm::Function* main_func = llvm::Function::Create(main_type, llvm::GlobalValue::ExternalLinkage, "main", module.get());

	llvm::BasicBlock* main_block = llvm::BasicBlock::Create(context, "entry", main_func, nullptr);
	builder.SetInsertPoint(main_block);

	builder.CreateRet(builder.getInt32(0));

	module->dump();

	std::cout << "--- LLVM test end ---\n\n";
	std::cout << "--- OpenCL test begin ---\n";
	// OpenCL test code(https://github.com/Dakkers/OpenCL-examples/blob/master/example00/main.cpp)

	std::vector<cl::Platform> all_platforms;
	cl::Platform::get(&all_platforms);

	if (all_platforms.size() == 0) 
	{
		std::cout<<" No platforms found. Check OpenCL installation!\n";
		exit(1);
	}
	cl::Platform default_platform=all_platforms[0];
	std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

	// get default device (CPUs, GPUs) of the default platform
	std::vector<cl::Device> all_devices;
	default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);

	if(all_devices.size()==0)
	{
		std::cout<<" No devices found. Check OpenCL installation!\n";
		exit(1);
	}

	cl::Device default_device=all_devices[1];
	std::cout<< "Using device: "<< default_device.getInfo<CL_DEVICE_NAME>() <<"\n";

	std::cout << "--- OpenCL test end ---\n";
	return 0;
}
