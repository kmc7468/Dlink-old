#pragma once

#include <memory>
#include <string>
#include <map>

#include "llvm/IR/Constant.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/Scalar.h"

#include "ParseStruct.hh"
#include "Error.hh"

namespace Dlink
{
	extern llvm::LLVMContext context;
	extern std::shared_ptr<llvm::Module> module;
	extern llvm::IRBuilder<> builder;
	extern std::unique_ptr<llvm::legacy::FunctionPassManager> func_pm;
	extern std::map<std::string, llvm::Value*> sym_map;
	extern std::map<std::string, std::shared_ptr<ClassType>> classes;
	extern ErrorList code_gen_errors;

	extern llvm::ConstantInt* constant_int32_0;
}
