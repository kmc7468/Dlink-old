#include <iostream>

#include "Dlink/Dlink.hh"
#include "Dlink/Lexer.hh"

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
		print("Hello, world!");
		return --a;
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
		MAP_TOKEN(scope),
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
	};
	#undef MAP_TOKEN
	
	lexer.dump(tokentype_map);
	return 0;
}
