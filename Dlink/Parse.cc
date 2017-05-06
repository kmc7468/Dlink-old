#include "Dlink/Parse.hh"

namespace Dlink
{
	int P_block(TokenIter begin, TokenIter end, std::shared_ptr<Statement>& out, ErrorList& e_list)
	{
		int i = 0, t;
		auto block = std::make_shared<Block>();

		while (true)
		{
			std::shared_ptr<Statement> sub_stmt;
			if (begin + i != end && (t = P_func_decl(begin + i, end, sub_stmt, e_list)) >= 0)
			{
				i += t;
				block->statements.push_back(sub_stmt);
			}
			else
			{
				break;
			}
		}

		out = block;
		return i;
	}

	int P_func_decl(TokenIter begin, TokenIter end, std::shared_ptr<Statement>& out, ErrorList& e_list)
	{
		int i = 0, t;

		std::shared_ptr<Type> rettype_expr;
		if ((t = P_type(begin + i, end, rettype_expr, e_list)) >= 0)
		{
			i += t;

			if (begin + i != end && begin[i].type == TokenType::identifier)
			{
				Identifier name(begin[i]);
				i++;

				if (begin + i != end && begin[i].type == TokenType::lparen)
				{
					i++;

					std::vector<VariableDeclaration> arg_list;
					while (true)
					{
						std::shared_ptr<Type> arg_type;
						if (begin + i != end && (t = P_type(begin + i, end, arg_type, e_list)) >= 0)
						{
							i += t;

							if (begin + i != end && begin[i].type == TokenType::identifier)
							{
								Identifier name(begin[i]);
								i++;

								VariableDeclaration arg(arg_type, name);
								arg_list.push_back(arg);

								if (begin + i != end && begin[i].type == TokenType::comma)
								{
									i++;
									continue;
								}
								else if (begin + i != end && begin[i].type == TokenType::rparen)
								{
									i++;
									break;
								}
								else
								{
									e_list.push_back(Error("Expected ',' or ')' after argument definition", begin[i - 1].line));
									return -1;
								}
							}
							else
							{
								e_list.push_back(Error("Expected identifier after type", begin[i - t].line));
								return -1;
							}
						}
						else if (begin + i != end && begin[i].type == TokenType::rparen)
						{
							i++;
							break;
						}
						else
						{
							e_list.push_back(Error("Expected type", begin[i - 1].line));
							return -1;
						}
					}

					if (begin + i != end && begin[i].type == TokenType::lbrace)
					{
						i++;

						std::shared_ptr<Statement> body;
						if (begin + i != end)
						{
							t = P_block(begin + i, end, body, e_list);
							i += t;

							if (begin + i != end && begin[i].type == TokenType::rbrace)
							{
								i++;
								out = std::make_shared<FunctionDeclaration>(rettype_expr, name, arg_list, body);
								out->line = begin[i - 1].line;
								return i;
							}
							else
							{
								e_list.push_back(Error("Expected '}'", begin[i - t].line));
								return -1;
							}
						}
						else
						{
							e_list.push_back(Error("Expected block after '{'", begin[i - 1].line));
							return -1;
						}

					}
					else
					{
						e_list.push_back(Error("Expected '{' after argument signature", begin[i - 1].line));
						return -1;
					}
				}
				else
				{
					std::shared_ptr<Statement> sub_expr_stmt;
					t = P_var_decl(begin, end, sub_expr_stmt, e_list);
					out = sub_expr_stmt;
					return t;
				}
			}
			else
			{
				std::shared_ptr<Statement> sub_expr_stmt;
				t = P_var_decl(begin, end, sub_expr_stmt, e_list);
				out = sub_expr_stmt;
				return t;
			}
		}
		else
		{
			std::shared_ptr<Statement> sub_expr_stmt;
			t = P_var_decl(begin, end, sub_expr_stmt, e_list);
			out = sub_expr_stmt;
			return t;
		}
	}
	int P_var_decl(TokenIter begin, TokenIter end, std::shared_ptr<Statement>& out, ErrorList& e_list)
	{
		int i = 0, t;

		std::shared_ptr<Type> type_expr;
		if ((t = P_type(begin + i, end, type_expr, e_list)) >= 0)
		{
			i += t;

			if (begin + i != end && begin[i].type == TokenType::identifier)
			{
				Identifier name(begin[i]);
				i++;

				if (begin + i != end && begin[i].type == TokenType::semicolon)
				{
					i++;
					out = std::make_shared<VariableDeclaration>(type_expr, name);
					out->line = begin[i - 1].line;
					return i;
				}
				else if (begin + i != end && begin[i].type == TokenType::equal)
				{
					i++;

					std::shared_ptr<Expression> init_expr;
					if (begin + i != end && (t = P_expr(begin + i, end, init_expr, e_list)) >= 0)
					{
						i += t;

						if (begin + i != end && begin[i].type == TokenType::semicolon)
						{
							i++;
							out = std::make_shared<VariableDeclaration>(type_expr, name, init_expr);
							out->line = begin[i - 1].line;
							return i;
						}
						else
						{
							e_list.push_back(Error("Expected ';' after expression", begin[i - t].line));
							return -1;
						}
					}
					else
					{
						e_list.push_back(Error("Expected expression after '='", begin[i - 1].line));
						return -1;
					}
				}
				else
				{
					e_list.push_back(Error("Expected semicolon or '=' after identifier", begin[i - 1].line));
					return -1;
				}
			}
			else
			{
				std::shared_ptr<Statement> sub_expr_stmt;
				t = P_return(begin, end, sub_expr_stmt, e_list);
				out = sub_expr_stmt;
				return t;
			}
		}
		else
		{
			std::shared_ptr<Statement> sub_expr_stmt;
			t = P_return(begin, end, sub_expr_stmt, e_list);
			out = sub_expr_stmt;
			return t;
		}
	}

	int P_return(TokenIter begin, TokenIter end, std::shared_ptr<Statement>& out, ErrorList& e_list)
	{
		int i = 0, t;

		if (begin[i].type == TokenType::_return)
		{
			i++;

			std::shared_ptr<Expression> sub_expr;
			if (begin + i != end && (t = P_expr(begin + i, end, sub_expr, e_list)) >= 0)
			{
				i += t;

				if (begin + i != end && begin[i].type == TokenType::semicolon)
				{
					i++;
					out = std::make_shared<Return>(sub_expr);
					out->line = begin[i].line;
					return i;
				}
				else
				{
					e_list.push_back(Error("Expected ';' after expression", begin[i - t].line));
					return -1;
				}
			}
			else
			{
				e_list.push_back(Error("Expected expression after 'return'", begin[i - t].line));
				return -1;
			}
		}
		else
		{
			std::shared_ptr<Statement> sub_expr_stmt;
			t = P_expr_stmt(begin, end, sub_expr_stmt, e_list);
			out = sub_expr_stmt;
			return t;
		}
	}

	int P_expr_stmt(TokenIter begin, TokenIter end, std::shared_ptr<Statement>& out, ErrorList& e_list)
	{
		int i = 0, t;

		std::shared_ptr<Expression> sub_expr;
		if ((t = P_expr(begin + i, end, sub_expr, e_list)) > 0)
		{
			i += t;

			if (begin + i != end && begin[i].type == TokenType::semicolon)
			{
				i++;
				out = std::make_shared<ExpressionStatement>(sub_expr);
				out->line = begin[i].line;
				return i;
			}
			else
			{
				e_list.push_back(Error("Expected ';' after expression", begin[i - t].line));
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}
}

//Expressions
namespace Dlink
{
	int P_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list)
	{
		return P_logical_or_expr(begin, end, out, e_list);
	}

	int P_logical_or_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list)
	{
		if (begin == end)
		{
			return -1;
		}

		int i = 0, t = 0;

		std::shared_ptr<Expression> sub_expr;
		if ((t = P_logical_and_expr(begin + i, end, sub_expr, e_list)) >= 0)
		{
			i = t;
		}
		else
		{
			return -1;
		}


		while (begin + i != end && (begin[i].type == TokenType::logic_or))
		{
			i++;
			std::shared_ptr<Expression> sub_expr2;
			if ((t = P_logical_and_expr(begin + i, end, sub_expr2, e_list)) >= 0)
			{
				i += t;
				sub_expr = std::make_shared<BinaryOP>(sub_expr, begin[i - t - 1], sub_expr2);
				sub_expr->line = begin[i - t].line;
			}
			else
			{
				i--;
				break;
			}
		}

		out = sub_expr;
		return i;
	}

	int P_logical_and_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list)
	{
		if (begin == end)
		{
			return -1;
		}

		int i = 0, t = 0;

		std::shared_ptr<Expression> sub_expr;
		if ((t = P_bitwise_or_expr(begin + i, end, sub_expr, e_list)) >= 0)
		{
			i = t;
		}
		else
		{
			return -1;
		}


		while (begin + i != end && (begin[i].type == TokenType::logic_and))
		{
			i++;
			std::shared_ptr<Expression> sub_expr2;
			if ((t = P_bitwise_or_expr(begin + i, end, sub_expr2, e_list)) >= 0)
			{
				i += t;
				sub_expr = std::make_shared<BinaryOP>(sub_expr, begin[i - t - 1], sub_expr2);
				sub_expr->line = begin[i - t].line;
			}
			else
			{
				i--;
				break;
			}
		}

		out = sub_expr;
		return i;
	}

	int P_bitwise_or_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list)
	{
		if (begin == end)
		{
			return -1;
		}

		int i = 0, t = 0;

		std::shared_ptr<Expression> sub_expr;
		if ((t = P_bitwise_xor_expr(begin + i, end, sub_expr, e_list)) >= 0)
		{
			i = t;
		}
		else
		{
			return -1;
		}


		while (begin + i != end && (begin[i].type == TokenType::bit_or))
		{
			i++;
			std::shared_ptr<Expression> sub_expr2;
			if ((t = P_bitwise_xor_expr(begin + i, end, sub_expr2, e_list)) >= 0)
			{
				i += t;
				sub_expr = std::make_shared<BinaryOP>(sub_expr, begin[i - t - 1], sub_expr2);
				sub_expr->line = begin[i - t].line;
			}
			else
			{
				i--;
				break;
			}
		}

		out = sub_expr;
		return i;
	}

	int P_bitwise_xor_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list)
	{
		if (begin == end)
		{
			return -1;
		}

		int i = 0, t = 0;

		std::shared_ptr<Expression> sub_expr;
		if ((t = P_bitwise_and_expr(begin + i, end, sub_expr, e_list)) >= 0)
		{
			i = t;
		}
		else
		{
			return -1;
		}


		while (begin + i != end && (begin[i].type == TokenType::bit_xor))
		{
			i++;
			std::shared_ptr<Expression> sub_expr2;
			if ((t = P_bitwise_and_expr(begin + i, end, sub_expr2, e_list)) >= 0)
			{
				i += t;
				sub_expr = std::make_shared<BinaryOP>(sub_expr, begin[i - t - 1], sub_expr2);
				sub_expr->line = begin[i - t].line;
			}
			else
			{
				i--;
				break;
			}
		}

		out = sub_expr;
		return i;
	}

	int P_bitwise_and_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list)
	{
		if (begin == end)
		{
			return -1;
		}

		int i = 0, t = 0;

		std::shared_ptr<Expression> sub_expr;
		if ((t = P_eq_noteq_expr(begin + i, end, sub_expr, e_list)) >= 0)
		{
			i = t;
		}
		else
		{
			return -1;
		}


		while (begin + i != end && (begin[i].type == TokenType::bit_and))
		{
			i++;
			std::shared_ptr<Expression> sub_expr2;
			if ((t = P_eq_noteq_expr(begin + i, end, sub_expr2, e_list)) >= 0)
			{
				i += t;
				sub_expr = std::make_shared<BinaryOP>(sub_expr, begin[i - t - 1], sub_expr2);
				sub_expr->line = begin[i - t].line;
			}
			else
			{
				i--;
				break;
			}
		}

		out = sub_expr;
		return i;
	}

	int P_eq_noteq_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list)
	{
		if (begin == end)
		{
			return -1;
		}

		int i = 0, t = 0;

		std::shared_ptr<Expression> sub_expr;
		if ((t = P_lessgreat_expr(begin + i, end, sub_expr, e_list)) >= 0)
		{
			i = t;
		}
		else
		{
			return -1;
		}


		while (begin + i != end && (begin[i].type == TokenType::comp_equal
									|| begin[i].type == TokenType::comp_noteq))
		{
			i++;
			std::shared_ptr<Expression> sub_expr2;
			if ((t = P_lessgreat_expr(begin + i, end, sub_expr2, e_list)) >= 0)
			{
				i += t;
				sub_expr = std::make_shared<BinaryOP>(sub_expr, begin[i - t - 1], sub_expr2);
				sub_expr->line = begin[i - t].line;
			}
			else
			{
				i--;
				break;
			}
		}

		out = sub_expr;
		return i;
	}

	int P_lessgreat_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list)
	{
		if (begin == end)
		{
			return -1;
		}

		int i = 0, t = 0;

		std::shared_ptr<Expression> sub_expr;
		if ((t = P_bit_lrshift_expr(begin + i, end, sub_expr, e_list)) >= 0)
		{
			i = t;
		}
		else
		{
			return -1;
		}


		while (begin + i != end && (begin[i].type == TokenType::comp_greater
									|| begin[i].type == TokenType::comp_eqgreater
									|| begin[i].type == TokenType::comp_less
									|| begin[i].type == TokenType::comp_eqless))
		{
			i++;
			std::shared_ptr<Expression> sub_expr2;
			if ((t = P_bit_lrshift_expr(begin + i, end, sub_expr2, e_list)) >= 0)
			{
				i += t;
				sub_expr = std::make_shared<BinaryOP>(sub_expr, begin[i - t - 1], sub_expr2);
				sub_expr->line = begin[i - t].line;
			}
			else
			{
				i--;
				break;
			}
		}

		out = sub_expr;
		return i;
	}

	int P_bit_lrshift_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list)
	{
		if (begin == end)
		{
			return -1;
		}

		int i = 0, t = 0;

		std::shared_ptr<Expression> sub_expr;
		if ((t = P_addsub_expr(begin + i, end, sub_expr, e_list)) >= 0)
		{
			i = t;
		}
		else
		{
			return -1;
		}


		while (begin + i != end && (begin[i].type == TokenType::bit_lshift || begin[i].type == TokenType::bit_rshift))
		{
			i++;
			std::shared_ptr<Expression> sub_expr2;
			if ((t = P_addsub_expr(begin + i, end, sub_expr2, e_list)) >= 0)
			{
				i += t;
				sub_expr = std::make_shared<BinaryOP>(sub_expr, begin[i - t - 1], sub_expr2);
				sub_expr->line = begin[i - t].line;
			}
			else
			{
				i--;
				break;
			}
		}

		out = sub_expr;
		return i;
	}

	int P_addsub_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list)
	{
		if (begin == end)
		{
			return -1;
		}

		int i = 0, t = 0;

		std::shared_ptr<Expression> sub_expr;
		if ((t = P_muldiv_expr(begin + i, end, sub_expr, e_list)) >= 0)
		{
			i = t;
		}
		else
		{
			return -1;
		}


		while (begin + i != end && (begin[i].type == TokenType::plus || begin[i].type == TokenType::minus))
		{
			i++;
			std::shared_ptr<Expression> sub_expr2;
			if ((t = P_muldiv_expr(begin + i, end, sub_expr2, e_list)) >= 0)
			{
				i += t;
				sub_expr = std::make_shared<BinaryOP>(sub_expr, begin[i - t - 1], sub_expr2);
				sub_expr->line = begin[i - t].line;
			}
			else
			{
				i--;
				break;
			}
		}

		out = sub_expr;
		return i;
	}

	int P_muldiv_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list)
	{
		if (begin == end)
		{
			return -1;
		}

		int i = 0, t = 0;

		std::shared_ptr<Expression> sub_expr;
		if ((t = P_not_expr(begin + i, end, sub_expr, e_list)) >= 0)
		{
			i = t;
		}
		else
		{
			return -1;
		}


		while (begin + i != end && (begin[i].type == TokenType::multiply
									|| begin[i].type == TokenType::divide
									|| begin[i].type == TokenType::modulo))
		{
			i++;
			std::shared_ptr<Expression> sub_expr2;
			if ((t = P_not_expr(begin + i, end, sub_expr2, e_list)) >= 0)
			{
				i += t;
				sub_expr = std::make_shared<BinaryOP>(sub_expr, begin[i - t - 1], sub_expr2);
				sub_expr->line = begin[i - t].line;
			}
			else
			{
				i--;
				break;
			}
		}

		out = sub_expr;
		return i;
	}

	int P_not_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list)
	{
		if (begin == end)
		{
			return -1;
		}

		int i = 0, t = 0;
		std::shared_ptr<Expression> ret;

		if (begin[i].type == TokenType::exclamation || begin[i].type == TokenType::bit_not)
		{
			i++;
			std::shared_ptr<Expression> not_expr;
			if ((t = P_sign_expr(begin + i, end, not_expr, e_list)) >= 0) //bookmark
			{
				i += t;
			}
			else
			{
				if (begin[i - 1].type == TokenType::exclamation)
				{
					e_list.push_back(Error("Expected expression after '!'", begin[i - 1].line));
				}
				else
				{
					e_list.push_back(Error("Expected expression after '~'", begin[i - 1].line));
				}
				return -1;
			}
			ret = std::make_shared<UnaryOP>(begin[i - t - 1], not_expr);
			ret->line = begin[i - t].line;
		}
		else
		{
			if ((t = P_sign_expr(begin + i, end, ret, e_list)) >= 0)
			{
				i += t;
			}
			else
			{
				return -1;
			}
		}
		out = ret;
		return i;
	}

	int P_sign_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list)
	{
		if (begin == end)
		{
			return -1;
		}

		int i = 0, t;
		std::shared_ptr<Expression> ret;

		if (begin[i].type == TokenType::plus || begin[i].type == TokenType::minus)
		{
			i++;
			std::shared_ptr<Expression> sign;
			if ((t = P_func_call_expr(begin + i, end, sign, e_list)) > 0) //bookmark
			{
				i += t;
			}
			else
			{
				if (begin[i - t - 1].type == TokenType::plus)
				{
					e_list.push_back(Error("Expected expression after '+'", begin[i - 1].line));
				}
				else
				{
					e_list.push_back(Error("Expected expression after '-'", begin[i - 1].line));
				}

				return -1;
			}
			ret = std::make_shared<UnaryOP>(begin[i - t - 1], sign);
			ret->line = begin[i - t].line;
		}
		else
		{
			if ((t = P_func_call_expr(begin + i, end, ret, e_list)) >= 0)
			{
				i += t;
			}
			else
			{
				return -1;
			}
		}
		out = ret;
		return i;
	}

	int P_func_call_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list)
	{
		if (begin == end)
		{
			return -1;
		}

		int i = 0, t;
		std::shared_ptr<Expression> ret;
		std::shared_ptr<Expression> sub_expr;

		if (begin[i].type == TokenType::identifier)
		{
			Identifier name(begin[i]);
			i++;

			if (begin[i].type == TokenType::lparen)
			{
				i++;

				std::vector<std::shared_ptr<Expression>> args;
				while (true)
				{
					std::shared_ptr<Expression> arg;

					if ((t = P_expr(begin + i, end, arg, e_list)) > 0)
					{
						i += t;
						args.push_back(arg);
					}

					if (begin + i != end && begin[i].type == TokenType::comma)
					{
						i++;
						continue;
					}
					else if (begin + i != end && begin[i].type == TokenType::rparen)
					{
						i++;
						break;
					}
					else
					{
						e_list.push_back(Error("Expected ',' or ')' after expression", begin[i - t].line));
						return -1;
					}
				}

				ret = std::make_shared<FuncCall>(name, args);
				ret->line = begin[i - 1].line;
			}
			else if ((t = P_incdec_expr(begin + (--i), end, sub_expr, e_list)) >= 0)
			{
				i += t;
				ret = sub_expr;
			}
		}
		else if ((t = P_incdec_expr(begin + i, end, sub_expr, e_list)) >= 0)
		{
			i += t;
			ret = sub_expr;
		}

		out = ret;
		return i;
	}

	int P_incdec_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList & e_list)
	{
		if (begin == end)
		{
			return -1;
		}

		int i = 0, t;
		std::shared_ptr<Expression> ret;
		std::shared_ptr<Expression> sign;

		if (begin[i].type == TokenType::increment || begin[i].type == TokenType::decrement)
		{
			i++;
			if ((t = P_paren_expr(begin + i, end, sign, e_list)) > 0) //bookmark
			{
				i += t;
			}
			else
			{
				if (begin[i - 1].type == TokenType::increment)
				{
					e_list.push_back(Error("Expected expression after '++'", begin[i - 1].line));
				}
				else
				{
					e_list.push_back(Error("Expected expression after '--'", begin[i - 1].line));
				}

				return -1;
			}

			Token op = begin[i - t - 1];
			if (op.type == TokenType::increment) op.type = TokenType::pre_inc;
			else op.type = TokenType::pre_dec;
			ret = std::make_shared<UnaryOP>(op, sign);
			ret->line = begin[i - t].line;
		}
		else if ((t = P_paren_expr(begin + i, end, sign, e_list)) > 0)
		{
			i += t;

			if (begin[i].type == TokenType::increment || begin[i].type == TokenType::decrement)
			{
				i++;
			}
			else
			{
				out = sign;
				return i;
			}

			auto op = begin[i - 1];
			if (op.type == TokenType::increment) op.type = TokenType::post_inc;
			else op.type = TokenType::post_dec;
			ret = std::make_shared<UnaryOP>(op, sign);
			ret->line = begin[i - 1].line;
		}
		else
		{
			if ((t = P_paren_expr(begin + i, end, ret, e_list)) >= 0)
			{
				i += t;
			}
			else
			{
				return -1;
			}
		}
		out = ret;
		return i;
	}

	int P_paren_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list)
	{
		if (begin == end)
		{
			return -1;
		}

		int i = 0, t = 0;

		std::shared_ptr<Expression> ret;

		if (begin[i].type == TokenType::lparen)
		{
			i++;
			if ((t = P_expr(begin + i, end, ret, e_list)) >= 0)
			{
				i += t;
			}

			if (begin + i != end && begin[i].type == TokenType::rparen)
			{
				i++;
			}
			else
			{
				e_list.push_back(Error("Expected ')'", begin[i - 1].line));
				return -1;
			}

			ret->line = begin[i - 1].line;
			out = ret;
			return i;
		}
		else if ((t = P_literal_expr(begin + i, end, ret, e_list)) >= 0)
		{
			i += t;
			out = ret;
			return i;
		}
		else
		{
			return -1;
		}
	}

	int P_literal_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list)
	{
		if (begin == end)
		{
			return -1;
		}

		switch (begin[0].type)
		{
		case TokenType::dec_integer:
			out = std::make_shared<Integer32>(static_cast<int32_t>(std::stoi(begin[0].data)));
			out->line = begin[0].line;
			return 1;
		case TokenType::bin_integer:
			out = std::make_shared<Integer32>(static_cast<int32_t>(std::stoi(begin[0].data.substr(2), nullptr, 2)));
			out->line = begin[0].line;
			return 1;
		case TokenType::oct_integer:
			out = std::make_shared<Integer32>(static_cast<int32_t>(std::stoi(begin[0].data.substr(1), nullptr, 8)));
			out->line = begin[0].line;
			return 1;
		case TokenType::hex_integer:
			out = std::make_shared<Integer32>(static_cast<int32_t>(std::stoi(begin[0].data.substr(2), nullptr, 16)));
			out->line = begin[0].line;
			return 1;
		case TokenType::floating:
			out = std::make_shared<Float>(std::stof(begin[0].data));
			out->line = begin[0].line;
			return 1;
		case TokenType::string:
			out = std::make_shared<String>(begin[0].data);
			out->line = begin[0].line;
			return 1;
		case TokenType::identifier:
			out = std::make_shared<Identifier>(begin[0]);
			out->line = begin[0].line;
			return 1;
		default:
			return -1;
		}
	}
}

//Types
namespace Dlink
{
	int P_type(TokenIter begin, TokenIter end, std::shared_ptr<Type>& out, ErrorList& e_list)
	{
		return P_type_identifier(begin, end, out, e_list);
	}

	int P_type_identifier(TokenIter begin, TokenIter end, std::shared_ptr<Type>& out, ErrorList& e_list)
	{
		if (begin[0].type == TokenType::identifier)
		{
			out = std::make_shared<IdentifierType>(begin[0]);
			out->line = begin[0].line;
			return 1;
		}
		else
		{
			return -1;
		}
	}
}