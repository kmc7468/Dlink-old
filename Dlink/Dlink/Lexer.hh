#pragma once

#include <vector>
#include <map>
#include <cctype>
#include <iostream>

#include "Token.hh"

namespace Dlink
{
	class Lexer
	{
	private:
		std::vector<Token> token_seq_; //Token sequence

		std::map<std::string, TokenType> keyword_map_;

	public:
		Lexer();
		void lex(const std::string& str);
		
		const std::vector<Token>& get_token_sequence() const;

		void dump(std::map<TokenType, std::string> tokentype_map) const;
	};
}
