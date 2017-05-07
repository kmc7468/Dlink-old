#include "Dlink/Lexer.hh"

namespace Dlink
{
	Lexer::Lexer()
	{
		keyword_map_["if"] = TokenType::_if;
		keyword_map_["else"] = TokenType::_else;
		keyword_map_["switch"] = TokenType::_switch;
		keyword_map_["for"] = TokenType::_for;
		keyword_map_["while"] = TokenType::_while;
		keyword_map_["do"] = TokenType::_do;
		keyword_map_["break"] = TokenType::_break;
		keyword_map_["continue"] = TokenType::_continue;
		keyword_map_["return"] = TokenType::_return;
		keyword_map_["auto"] = TokenType::_auto;
		keyword_map_["gpu"] = TokenType::gpu;
		keyword_map_["cpu"] = TokenType::cpu;
		keyword_map_["class"] = TokenType::_class;
		keyword_map_["public"] = TokenType::_public;
		keyword_map_["protected"] = TokenType::_protected;
		keyword_map_["private"] = TokenType::_private;
	}

	void Lexer::lex(const std::string& str)
	{
		std::size_t line = 1;

		for (std::size_t i = 0; i < str.length(); i++)
		{
			char ch = str[i];

			if (std::isalpha(ch))
			{
				std::string temp = "";
				do
				{
					temp += str[i];
					i++;
				} while (std::isalpha(str[i]) || std::isdigit(str[i]) || str[i] == '_');

				i--;

				bool is_keyword = (keyword_map_.find(temp) != keyword_map_.end());

				if (is_keyword)
				{
					Token keyword_token(temp, keyword_map_[temp], line);
					token_seq_.push_back(keyword_token);
				}
				else
				{
					Token identifier_token(temp, TokenType::identifier, line);
					token_seq_.push_back(identifier_token);
				}
			}
			else if (std::isdigit(ch))
			{
				std::string temp = "";

				if (ch == '0' && i < str.length() - 2) //maybe non-decimal integral literal
				{
					char next_ch = str[i + 1];
					switch (next_ch)
					{
					case 'b':
					case 'B': //binary literal
						temp += '0';
						temp += next_ch;
						i += 2;

						do
						{
							temp += str[i];
							i++;
						} while (str[i] == '0' || str[i] == '1');
						i--;

						token_seq_.push_back(Token(temp, TokenType::bin_integer, line));
						break;

					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7': //octal literal
						temp += '0';
						i++;

						do
						{
							temp += str[i];
							i++;
						} while (('0' <= str[i]) && (str[i] <= '7'));
						i--;

						token_seq_.push_back(Token(temp, TokenType::oct_integer, line));
						break;

					case 'x':
					case 'X': //hex literal
						temp += '0';
						temp += next_ch;
						i += 2;

						do
						{
							temp += str[i];
							i++;
						} while (i < str.length() && (std::isdigit(str[i]) ||
							(('a' <= str[i]) && (str[i] <= 'f')) ||
													  (('A' <= str[i]) && (str[i] <= 'F'))));
						i--;

						token_seq_.push_back(Token(temp, TokenType::hex_integer, line));
						break;

					default:
						token_seq_.push_back(Token("0", TokenType::dec_integer, line));
						break;
					}
				}
				else
				{
					do
					{
						temp += str[i];
						i++;
					} while (i < str.length() && std::isdigit(str[i]));

					if (str[i] == '.') //then it's real number
					{
						do
						{
							temp += str[i];
							i++;
						} while (i < str.length() && std::isdigit(str[i]));

						if (str[i] == 'e' && (str[i + 1] == '-' || str[i + 1] == '+')) //expotential notation
						{
							temp += 'e';
							temp += str[i + 1];
							i += 2;
							do
							{
								temp += str[i];
								i++;
							} while (i < str.length() && std::isdigit(str[i]));
						}

						token_seq_.push_back(Token(temp, TokenType::floating, line));
					}
					else //then it's decimal integer
					{
						token_seq_.push_back(Token(temp, TokenType::dec_integer, line));
					}
					i--;
				}
			}
			else if (ch == '\"')
			{
				std::string temp = "";
				i++;

				do
				{
					if (str[i] == '\\')
					{
						if (i < str.length() - 2 && str[i + 1] == '\"')
						{
							temp += '\"';
							i += 2;
							continue;
						}
					}

					temp += str[i];
					i++;
				} while (str[i] != '\"');

				token_seq_.push_back(Token(temp, TokenType::string, line));
			}
			else //operators and punctuations
			{
				switch (ch)
				{
				case '+':
					if (str[i + 1] == '+') //then ++
					{
						token_seq_.push_back(Token("++", TokenType::increment, line));
						i++;
					}
					else if (str[i + 1] == '=') //then +=
					{
						token_seq_.push_back(Token("+=", TokenType::plus_assign, line));
						i++;
					}
					else //then +
					{
						token_seq_.push_back(Token("+", TokenType::plus, line));
					}
					break;
				case '-':
					if (str[i + 1] == '-') //then --
					{
						token_seq_.push_back(Token("--", TokenType::decrement, line));
						i++;
					}
					else if (str[i + 1] == '=') //then -=
					{
						token_seq_.push_back(Token("-=", TokenType::minus_assign, line));
						i++;
					}
					else //then +
					{
						token_seq_.push_back(Token("-", TokenType::minus, line));
					}
					break;
				case '*':
					if (str[i + 1] == '=') //then *=
					{
						token_seq_.push_back(Token("*=", TokenType::multiply_assign, line));
						i++;
					}
					else //then *
					{
						token_seq_.push_back(Token("*", TokenType::multiply, line));
					}
					break;
				case '/':
					if (str[i + 1] == '=') //then /=
					{
						token_seq_.push_back(Token("/=", TokenType::divide_assign, line));
						i++;
					}
					else //then /
					{
						token_seq_.push_back(Token("/", TokenType::divide, line));
					}
					break;
				case '%':
					if (str[i + 1] == '=') //then %=
					{
						token_seq_.push_back(Token("%=", TokenType::modulo_assign, line));
						i++;
					}
					else //then %
					{
						token_seq_.push_back(Token("%", TokenType::modulo, line));
					}
					break;
				case '=':
					if (str[i + 1] != '=') //then =
					{
						token_seq_.push_back(Token("=", TokenType::equal, line));
					}
					else //then ==
					{
						token_seq_.push_back(Token("==", TokenType::comp_equal, line));
						i++;
					}
					break;
				case '>':
					if (str[i + 1] == '=') //then >=
					{
						token_seq_.push_back(Token(">=", TokenType::comp_eqless, line));
						i++;
					}
					else if (str[i + 1] == '>') //then >> or >>=
					{
						if (str[i + 2] == '=') //then >>=
						{
							token_seq_.push_back(Token(">>=", TokenType::bit_lshift_assign, line));
							i++;
						}
						else //then >>
						{
							token_seq_.push_back(Token(">>", TokenType::bit_lshift, line));
						}
						i++;
					}
					else //then >
					{
						token_seq_.push_back(Token(">", TokenType::comp_less, line));
					}
					break;
				case '<':
					if (str[i + 1] == '=') //then <=
					{
						token_seq_.push_back(Token("<=", TokenType::comp_eqless, line));
						i++;
					}
					else if (str[i + 1] == '<') //then << or <<=
					{
						if (str[i + 2] == '=') //then <<=
						{
							token_seq_.push_back(Token("<<=", TokenType::bit_rshift_assign, line));
							i++;
						}
						else //then <<
						{
							token_seq_.push_back(Token("<<", TokenType::bit_rshift, line));
						}
						i++;
					}
					else //then <
					{
						token_seq_.push_back(Token("<", TokenType::comp_less, line));
					}
					break;
				case '&':
					if (str[i + 1] == '&') //then &&
					{
						token_seq_.push_back(Token("&&", TokenType::logic_and, line));
						i++;
					}
					else if (str[i + 1] == '=') //then &=
					{
						token_seq_.push_back(Token("&=", TokenType::bit_and_assign, line));
						i++;
					}
					else //then &
					{
						token_seq_.push_back(Token("&", TokenType::bit_and, line));
					}
					break;
				case '|':
					if (str[i + 1] == '|') //then ||
					{
						token_seq_.push_back(Token("||", TokenType::logic_or, line));
						i++;
					}
					else if (str[i + 1] == '=') //then |=
					{
						token_seq_.push_back(Token("|=", TokenType::bit_or_assign, line));
						i++;
					}
					else //then |
					{
						token_seq_.push_back(Token("|", TokenType::bit_or, line));
					}
					break;
				case '~':
					token_seq_.push_back(Token("~", TokenType::bit_not, line));
					break;
				case '^':
					if (str[i + 1] == '=') //then ^=
					{
						token_seq_.push_back(Token("^=", TokenType::bit_xor_assign, line));
						i++;
					}
					else //then ^
					{
						token_seq_.push_back(Token("^", TokenType::bit_xor, line));
					}
					break;
				case '{':
					token_seq_.push_back(Token("{", TokenType::lbrace, line));
					break;
				case '}':
					token_seq_.push_back(Token("}", TokenType::rbrace, line));
					break;
				case '(':
					token_seq_.push_back(Token("(", TokenType::lparen, line));
					break;
				case ')':
					token_seq_.push_back(Token(")", TokenType::rparen, line));
					break;
				case '[':
					token_seq_.push_back(Token("[", TokenType::lbparen, line));
					break;
				case ']':
					token_seq_.push_back(Token("]", TokenType::rbparen, line));
					break;
				case '.':
					token_seq_.push_back(Token(".", TokenType::dot, line));
					break;
				case ',':
					token_seq_.push_back(Token(",", TokenType::comma, line));
					break;
				case '\'':
					token_seq_.push_back(Token("\'", TokenType::apostrophe, line));
					break;
				case ';':
					token_seq_.push_back(Token(";", TokenType::semicolon, line));
					break;
				case ':':
					if (str[i + 1] != ':') //then :
					{
						token_seq_.push_back(Token(":", TokenType::colon, line));
					}
					break;
				case '!': //! or !=
					if (str[i + 1] != '=') //then !
					{
						token_seq_.push_back(Token("!", TokenType::exclamation, line));
					}
					else //then !=
					{
						token_seq_.push_back(Token("!=", TokenType::comp_noteq, line));
						i++;
					}
					break;
				case '?':
					token_seq_.push_back(Token("?", TokenType::question, line));
					break;
				case '\n':
					line++;
					break;
				}
			}
		}
	}

	const std::vector<Token>& Lexer::get_token_sequence() const
	{
		return token_seq_;
	}

	void Lexer::dump(std::map<TokenType, std::string> tokentype_map) const
	{
		for (const Token& token : token_seq_)
		{
			std::cout << "Line " << token.line << " -> " << tokentype_map[token.type] << "(" << token.data << ")\n";
		}
	}
}
