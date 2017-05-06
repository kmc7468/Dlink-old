#pragma once

#include <string>

namespace Dlink
{
	enum class TokenType
	{
		none,				//special
		eof,
		error,

		identifier,

		dec_integer,		//literal 
		bin_integer,
		oct_integer,
		hex_integer,
		floating,
		string,

		plus,				// +
		increment, 			// ++
		pre_inc,			// ++
		post_inc,			// ++
		plus_assign,		// +=
		minus,				// -
		decrement, 			// --
		pre_dec,			// --
		post_dec, 			// --
		minus_assign,		// -=
		multiply,			// *
		multiply_assign,	// *=
		divide,				// /
		divide_assign,		// /=
		modulo,				// %
		modulo_assign,		// %=
		equal,				// =
		comp_equal,			// ==
		comp_noteq,			// !=
		comp_greater,		// >
		comp_eqgreater,		// >=
		comp_less,			// <
		comp_eqless,		// <=
		logic_and,			// &&
		logic_or,			// ||
		bit_not,			// ~
		bit_and,			// &
		bit_and_assign,		// &=
		bit_or,				// |
		bit_or_assign,		// |=
		bit_xor,			// ^
		bit_xor_assign,		// ^=
		bit_lshift,			// <<
		bit_lshift_assign,  // <<=
		bit_rshift,			// >>
		bit_rshift_assign,  // >>=
		lbrace,				// {
		rbrace,				// }
		lparen,				// (
		rparen,				// )
		lbparen,			// [
		rbparen,			// ]
		dot,				// .
		comma,				// ,
		apostrophe,			// '
		semicolon,			// ;
		colon,				// :
		exclamation,		// !
		question,			// ?

		_if,
		_else,
		_switch,
		_case,
		_for,
		_while,
		_do,
		_break,
		_continue,
	};

	struct Token
	{
		const std::string data;
		const TokenType type;

		const std::size_t line;

		Token(const std::string& _data, TokenType _type, std::size_t _line) : data(_data), type(_type), line(_line) {}
	};
}
