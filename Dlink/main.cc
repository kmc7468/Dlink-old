#include <iostream>
#include <memory>

#include "Dlink/Dlink.hh"
#include "Dlink/Lexer.hh"
#include "Dlink/Error.hh"
#include "Dlink/ParseStruct.hh"
#include "Dlink/Parse.hh"
#include "Dlink/Token.hh"

namespace dl = Dlink;

int main(int argc, char* argv[])
{
	/* 
	if (!dl::Dlink::parse_command_line(argc, argv))
	{
		return 0;
	}
	*/
	
	std::string code = R"(
		int main()
		{
			bool a = f(-(1 && 0), g() && a);
			return (1 + 1) * 0;
		}
	)";

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
	}
	return 0;
}
