#pragma once

#include <vector>

#include "Token.hh"
#include "Error.hh"
#include "ParseStruct.hh"

namespace Dlink
{
	using TokenIter = std::vector<Token>::iterator;
}

//Statements
namespace Dlink
{
	int P_block(TokenIter begin, TokenIter end, std::shared_ptr<Statement>& out, ErrorList& e_list);
	int P_scope(TokenIter begin, TokenIter end, std::shared_ptr<Statement>& out, ErrorList& e_list);
	int P_func_decl(TokenIter begin, TokenIter end, std::shared_ptr<Statement>& out, ErrorList& e_list);
	int P_var_decl(TokenIter begin, TokenIter end, std::shared_ptr<Statement>& out, ErrorList& e_list);
	int P_class_decl(TokenIter begin, TokenIter end, std::shared_ptr<Statement>& out, ErrorList& e_list);
	int P_return(TokenIter begin, TokenIter end, std::shared_ptr<Statement>& out, ErrorList& e_list);
	int P_if_else(TokenIter begin, TokenIter end, std::shared_ptr<Statement>& out, ErrorList& e_list);
	int P_expr_stmt(TokenIter begin, TokenIter end, std::shared_ptr<Statement>& out, ErrorList& e_list);
}

//Expressions
namespace Dlink
{
	int P_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_assign_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_logical_or_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_logical_and_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_bitwise_or_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_bitwise_xor_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_bitwise_and_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_eq_noteq_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_lessgreat_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_bit_lrshift_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_addsub_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_muldiv_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_ref_deref_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_not_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_sign_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_func_call_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_incdec_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_member_access_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_paren_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
	int P_literal_expr(TokenIter begin, TokenIter end, std::shared_ptr<Expression>& out, ErrorList& e_list);
}

//Types
namespace Dlink
{	
	int P_type(TokenIter begin, TokenIter end, std::shared_ptr<Type>& out, ErrorList& e_list);
	int P_single_ptr_type(TokenIter begin, TokenIter end, std::shared_ptr<Type>& out, ErrorList& e_list);
	int P_const_type(TokenIter begin, TokenIter end, std::shared_ptr<Type>& out, ErrorList& e_list);
	int P_type_identifier(TokenIter begin, TokenIter end, std::shared_ptr<Type>& out, ErrorList& e_list);
}
