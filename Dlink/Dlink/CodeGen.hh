#pragma once

#include <memory>
#include <string>
#include <map>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/Scalar.h"

#include "Error.hh"

namespace Dlink
{
	extern llvm::LLVMContext context;
	extern std::shared_ptr<llvm::Module> module;
	extern llvm::IRBuilder<> builder;
	extern std::unique_ptr<llvm::legacy::FunctionPassManager> func_pm;
	extern std::map<std::string, llvm::Value*> sym_map;
	extern ErrorList code_gen_errors;
}
