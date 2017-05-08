#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <streambuf>

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/PassManager.h"

#include "Dlink/Dlink.hh"
#include "Dlink/Lexer.hh"
#include "Dlink/Error.hh"
#include "Dlink/ParseStruct.hh"
#include "Dlink/Parse.hh"
#include "Dlink/Token.hh"
#include "Dlink/CodeGen.hh"
#include "Dlink/Init.hh"

namespace dl = Dlink;

int main(int argc, char* argv[])
{	
	if (!dl::Dlink::parse_command_line(argc, argv))
	{
		return 0;
	}

	std::ifstream code_file(dl::Dlink::input_files[0]);
	
	std::string code((std::istreambuf_iterator<char>(code_file)), std::istreambuf_iterator<char>());

	dl::InitializePassManager();

	dl::Lexer lexer;
	lexer.dump(dl::tokentype_map);
	lexer.lex(code);

	std::vector<dl::Token> result = lexer.get_token_sequence();

	std::shared_ptr<dl::Statement> stmt;
	dl::ErrorList errors;
	int ret = P_block(result.begin(), result.end(), stmt, errors);

	if (!errors.empty()) //If there's error after parsing
	{
		for (auto error : errors)
		{
			error.print(std::cerr);
		}

	}
	else if (ret == -1) //If parsing failed at first try
	{
		dl::Error unexpected_err("Unexpected '" + result[0].data + "'", result[0].line);
		unexpected_err.print(std::cerr);
	}
	else if (ret < result.size()) //If there's remaining token after parsing
	{
		dl::Error unexpected_err("Unexpected '" + result[ret].data + "'", result[ret].line);
		unexpected_err.print(std::cerr);
	}
	else //Everything is OK
	{
		std::clog << "Parsing succeed\n";
		errors.clear();
		std::clog << stmt->tree_gen(0, dl::tokentype_map) << std::endl;

		/* DELME BEGIN */

		llvm::FunctionType *puts_type = llvm::FunctionType::get(dl::builder.getInt32Ty(), dl::builder.getInt8PtrTy(), false);
		llvm::Constant* puts_func = dl::module->getOrInsertFunction("puts", puts_type);
		llvm::FunctionType *printf_type = llvm::FunctionType::get(dl::builder.getInt32Ty(), dl::builder.getInt8PtrTy(), true);
		llvm::Constant* printf_func = dl::module->getOrInsertFunction("printf", printf_type);

		/* DELME END */

		stmt->code_gen();

		if (!dl::code_gen_errors.empty())
		{
			for (dl::Error error : dl::code_gen_errors)
			{
				error.print(std::cerr);
			}
		}
		else
		{
			dl::module->dump();

			std::error_code err_code;
			llvm::raw_fd_ostream output("output.ll", err_code, llvm::sys::fs::F_None);
			dl::module->print(output, nullptr);
		}
	}
	return 0;
}
