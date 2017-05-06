#include <iostream>
#include <memory>

#include "Dlink/Dlink.hh"
#include "Dlink/Lexer.hh"
#include "Dlink/Error.hh"
#include "Dlink/ParseStruct.hh"
#include "Dlink/Parse.hh"

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
	lexer.lex(code);

	std::vector<dl::Token> result = lexer.get_token_sequence();

	#define MAP_TOKEN(token) std::pair<dl::TokenType, std::string>(dl::TokenType::token, #token)
	std::map<dl::TokenType, std::string> tokentype_map =
	{
		MAP_TOKEN(none),
		MAP_TOKEN(eof),
		MAP_TOKEN(error),
		MAP_TOKEN(identifier),
		MAP_TOKEN(dec_integer),
		MAP_TOKEN(bin_integer),
		MAP_TOKEN(oct_integer),
		MAP_TOKEN(hex_integer),
		MAP_TOKEN(floating),
		MAP_TOKEN(string),
		MAP_TOKEN(plus),
		MAP_TOKEN(increment), 
		MAP_TOKEN(pre_inc),
		MAP_TOKEN(post_inc),
		MAP_TOKEN(plus_assign),
		MAP_TOKEN(minus),
		MAP_TOKEN(decrement), 
		MAP_TOKEN(pre_dec),
		MAP_TOKEN(post_dec), 
		MAP_TOKEN(minus_assign),
		MAP_TOKEN(multiply),
		MAP_TOKEN(multiply_assign),
		MAP_TOKEN(divide),
		MAP_TOKEN(divide_assign),
		MAP_TOKEN(modulo),
		MAP_TOKEN(modulo_assign),
		MAP_TOKEN(equal),
		MAP_TOKEN(comp_equal),
		MAP_TOKEN(comp_noteq),
		MAP_TOKEN(comp_greater),
		MAP_TOKEN(comp_eqgreater),
		MAP_TOKEN(comp_less),
		MAP_TOKEN(comp_eqless),
		MAP_TOKEN(logic_and),
		MAP_TOKEN(logic_or),
		MAP_TOKEN(bit_not),
		MAP_TOKEN(bit_and),
		MAP_TOKEN(bit_and_assign),
		MAP_TOKEN(bit_or),
		MAP_TOKEN(bit_or_assign),
		MAP_TOKEN(bit_xor),
		MAP_TOKEN(bit_xor_assign),
		MAP_TOKEN(bit_lshift),
		MAP_TOKEN(bit_lshift_assign),  
		MAP_TOKEN(bit_rshift),
		MAP_TOKEN(bit_rshift_assign),  
		MAP_TOKEN(lbrace),
		MAP_TOKEN(rbrace),
		MAP_TOKEN(lparen),
		MAP_TOKEN(rparen),
		MAP_TOKEN(lbparen),
		MAP_TOKEN(rbparen),
		MAP_TOKEN(dot),
		MAP_TOKEN(comma),
		MAP_TOKEN(apostrophe),
		MAP_TOKEN(semicolon),
		MAP_TOKEN(colon),
		MAP_TOKEN(exclamation),
		MAP_TOKEN(question),
		MAP_TOKEN(_if),
		MAP_TOKEN(_else),
		MAP_TOKEN(_switch),
		MAP_TOKEN(_case),
		MAP_TOKEN(_for),
		MAP_TOKEN(_while),
		MAP_TOKEN(_do),
		MAP_TOKEN(_break),
		MAP_TOKEN(_continue),
		MAP_TOKEN(_return),
		MAP_TOKEN(_auto),
		MAP_TOKEN(gpu),
		MAP_TOKEN(cpu),
	};
	#undef MAP_TOKEN
	
	lexer.dump(tokentype_map);

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
		std::clog << stmt->tree_gen(0, tokentype_map) << std::endl;
	}
	return 0;
}
