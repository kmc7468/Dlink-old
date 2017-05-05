#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "Token.hh"

//Parent nodes
namespace Dlink
{
	struct Node
	{
		const std::size_t line = 0;

		virtual std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map)
		{
			return std::string(depth*6, '-') + "<Undefined Tree>";
		};

		virtual ~Node() {};
	};

	struct Statement : public Node {};	
	struct Expression : public Node {};

	struct Type
	{
		const std::size_t line = 0;

		virtual std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map)
		{
			return std::string(depth*6, '-') + "<Undefined Tree>";
		};

		virtual ~Type() {};
	};
}

//Expression nodes
namespace Dlink
{
	struct Integer32 : public Expression
	{
		const int32_t data;

		Integer32(int32_t _data) : data(_data) {}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) override;
	};

	struct Float : public Expression
	{
		const float data;

		Float(float _data) : data(_data) {}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) override;
	};

	struct String : public Expression
	{
		const std::string data;

		String(std::string _data) : data(_data) {}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) override;
	};

	struct Identifier : public Expression
	{
		const Token id;

		Identifier(const Token& _id) : id(_id) {}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) override;
	};

	struct UnaryOP : public Expression
	{
		const Token op;
		std::shared_ptr<Expression> rhs;

		UnaryOP(const Token& _op, std::shared_ptr<Expression> _rhs) : op(_op), rhs(_rhs) {}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) override;
	};

	struct BinaryOP : public Expression
	{
		const Token op;
		std::shared_ptr<Expression> lhs, rhs;

		BinaryOP(std::shared_ptr<Expression> _lhs, const Token& _op, std::shared_ptr<Expression> _rhs) 
			: lhs(_lhs), op(_op), rhs(_rhs) {}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) override;
	};

	struct FuncCall : public Expression
	{
		const Identifier id;
		std::vector<std::shared_ptr<Expression>> args;

		FuncCall(const Identifier& _id, const std::vector<std::shared_ptr<Expression>>& _args) : id(_id), args(_args) {}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) override;
	};
}

//Statement nodes
namespace Dlink
{
	struct Block : public Statement
	{
		const std::vector<std::shared_ptr<Statement>> statements;
		
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) override;
	};

	struct VariableDeclaration : public Statement
	{
		std::shared_ptr<Type> type; 
		const Identifier id;
		std::shared_ptr<Expression> expression;

		VariableDeclaration(std::shared_ptr<Type> _type, const Identifier& _id) : type(_type), id(_id) {}
		VariableDeclaration(std::shared_ptr<Type> _type, const Identifier& _id, std::shared_ptr<Expression> _expression) 
			: type(_type), id(_id), expression(_expression) {}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) override;
	};

	struct FunctionDeclaration : public Statement
	{
		std::shared_ptr<Type> ret_type; 
		const Identifier id;
		const std::vector<VariableDeclaration> arg_list;
		std::shared_ptr<Statement> body;

		FunctionDeclaration(std::shared_ptr<Type> _ret_type, const Identifier& _id, const std::vector<VariableDeclaration>& _arg_list, std::shared_ptr<Statement> _body) 
			: ret_type(_ret_type), id(_id), arg_list(_arg_list), body(_body) {}	
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) override;
	};

	struct Return : public Statement
	{
		std::shared_ptr<Expression> ret_expr;

		Return(std::shared_ptr<Expression> _ret_expr) : ret_expr(_ret_expr) {}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) override;
	};

	struct ExpressionStatement : public Statement
	{
		std::shared_ptr<Expression> expression;

		ExpressionStatement(std::shared_ptr<Expression> _expression) : expression(_expression) {}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) override;
	};
}

//Type nodes
namespace Dlink
{
	struct IdentifierType : public Type
	{
		const Identifier id;

		IdentifierType(const Identifier& _id) : id(_id) {}
		
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) override;
	};
}
