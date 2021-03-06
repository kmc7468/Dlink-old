#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "llvm/IR/Value.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"

#include "Token.hh"

//Parent nodes
namespace Dlink
{
	struct Node
	{
		std::size_t line = 0;

		virtual std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
		{
			return std::string(depth * 6, ' ') + "<Undefined Tree>";
		};
		
		virtual llvm::Value* code_gen()
		{
			return nullptr;
		}

		virtual ~Node()
		{};
	};

	struct Statement : public Node
	{};
	struct Expression : public Node
	{
		bool is_modifiable = false;
	};

	struct Type
	{
		std::size_t line = 0;

		bool is_const = false;

		virtual std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
		{
			return std::string(depth * 6, ' ') + "<Undefined Tree>";
		};

		virtual llvm::Type* get_type()
		{
			return nullptr;
		}

		virtual ~Type()
		{};
	};

	enum class Modifier
	{
		Public,
		Protected,
		Private,
	};
}

//Expression nodes
namespace Dlink
{
	struct Integer32 : public Expression
	{
		const int32_t data;

		Integer32(int32_t _data) : data(_data)
		{}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override;
	};

	struct Float : public Expression
	{
		const float data;

		Float(float _data) : data(_data)
		{}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override;
	};

	struct String : public Expression
	{
		const std::string data;

		String(std::string _data) : data(_data)
		{}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override;
	};

	struct Identifier : public Expression
	{
		const Token id;
		const bool is_member = false;

		Identifier(const Token& _id) : id(_id)
		{}
		Identifier(const Token& _id, bool _is_member) : id(_id), is_member(_is_member)
		{}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override;
	};

	struct Const_Null : public Expression
	{
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override;
	};

	struct UnaryOP : public Expression
	{
		const Token op;
		std::shared_ptr<Expression> rhs;

		UnaryOP(const Token& _op, std::shared_ptr<Expression> _rhs) : op(_op), rhs(_rhs)
		{}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override;
	};

	struct BinaryOP : public Expression
	{
		const Token op;
		std::shared_ptr<Expression> lhs, rhs;

		BinaryOP(std::shared_ptr<Expression> _lhs, const Token& _op, std::shared_ptr<Expression> _rhs)
			: lhs(_lhs), op(_op), rhs(_rhs)
		{}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override;
	};

	struct FuncCall : public Expression
	{
		const Identifier id;
		std::vector<std::shared_ptr<Expression>> args;

		FuncCall(const Identifier& _id, const std::vector<std::shared_ptr<Expression>>& _args) : id(_id), args(_args)
		{}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override;
	};
}

//Statement nodes
namespace Dlink
{
	struct Block : public Statement
	{
		std::vector<std::shared_ptr<Statement>> statements;

		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override;
	};

	struct Scope : public Block
	{
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override;
	};

	struct VariableDeclaration : public Statement
	{
		std::shared_ptr<Type> type;
		const Identifier id;
		std::shared_ptr<Expression> expression;

		VariableDeclaration(std::shared_ptr<Type> _type, const Identifier& _id) : type(_type), id(_id)
		{}
		VariableDeclaration(std::shared_ptr<Type> _type, const Identifier& _id, std::shared_ptr<Expression> _expression)
			: type(_type), id(_id), expression(_expression)
		{}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override;

		llvm::AllocaInst* value() const
		{
			return value_;
		}

	private:
		llvm::AllocaInst* value_;
	};

	struct FieldDeclaration : public VariableDeclaration
	{
		const bool is_dynamic_field;
		const Modifier modifier;

		FieldDeclaration(std::shared_ptr<Type> _type, const Identifier& _id,
						 bool _is_dynamic_field = false, Modifier _modifier = Modifier::Private)
			: VariableDeclaration(_type, _id), is_dynamic_field(_is_dynamic_field),
			modifier(_modifier)
		{}
		FieldDeclaration(std::shared_ptr<Type> _type, const Identifier& _id, std::shared_ptr<Expression> _expression,
						 bool _is_dynamic_field = false, Modifier _modifier = Modifier::Private)
			: VariableDeclaration(_type, _id, _expression), is_dynamic_field(_is_dynamic_field),
			modifier(_modifier)
		{}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override
		{
			return nullptr;
		}
	};

	struct FunctionDeclaration : public Statement
	{
		std::shared_ptr<Type> ret_type;
		const Identifier id;
		const std::vector<VariableDeclaration> arg_list;
		std::shared_ptr<Statement> body;

		FunctionDeclaration(std::shared_ptr<Type> _ret_type, const Identifier& _id, const std::vector<VariableDeclaration>& _arg_list, std::shared_ptr<Statement> _body)
			: ret_type(_ret_type), id(_id), arg_list(_arg_list), body(_body)
		{}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override;
	};

	struct MethodDeclaration : public FunctionDeclaration
	{
		const bool is_dynamic_method;
		const Modifier modifier;

		MethodDeclaration(std::shared_ptr<Type> _ret_type, const Identifier& _id, const std::vector<VariableDeclaration>& _arg_list, std::shared_ptr<Statement> _body,
							bool _is_dynamic_method = false, Modifier _modifier = Modifier::Private)
			: FunctionDeclaration(_ret_type, _id, _arg_list, _body), is_dynamic_method(_is_dynamic_method),
			modifier(_modifier)
		{}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override;
	};

	struct ClassType;

	struct ClassDeclaration : public Statement
	{
		const Identifier id;
		const std::vector<FieldDeclaration> fields;
		const std::vector<MethodDeclaration> methods;

		ClassDeclaration(const Identifier& _id, const std::vector<FieldDeclaration>& _fields,
						 const std::vector<MethodDeclaration>& _methods);
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override;
	};

	struct Return : public Statement
	{
		std::shared_ptr<Expression> ret_expr;

		Return(std::shared_ptr<Expression> _ret_expr) : ret_expr(_ret_expr)
		{}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override;
	};

	struct IfElse : public Statement
	{
		std::shared_ptr<Expression> cond_expr;
		std::shared_ptr<Statement> true_body, false_body;

		IfElse(std::shared_ptr<Expression> _cond_expr, std::shared_ptr<Statement> _true_body, std::shared_ptr<Statement> _false_body = nullptr)
			: cond_expr(_cond_expr), true_body(_true_body), false_body(_false_body)
		{}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override;
	};

	struct ExpressionStatement : public Statement
	{
		std::shared_ptr<Expression> expression;

		ExpressionStatement(std::shared_ptr<Expression> _expression) : expression(_expression)
		{}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Value* code_gen() override;
	};
}

//Type nodes
namespace Dlink
{
	struct IdentifierType : public Type
	{
		const Identifier id;

		IdentifierType(const Identifier& _id) : id(_id)
		{}

		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Type* get_type() override;
	};
	
	struct PointerType : public Type
	{
		std::shared_ptr<Type> type;

		PointerType(const std::shared_ptr<Type>& _type) : type(_type)
		{}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Type* get_type() override;
	};

	struct ClassType : public IdentifierType
	{
		const std::vector<FieldDeclaration>& fields;
		const std::vector<MethodDeclaration>& methods;
		llvm::StructType* type;

		ClassType(const Identifier& _id, llvm::StructType* _type, const ClassDeclaration& c)
			: IdentifierType(_id), type(_type), fields(c.fields), methods(c.methods)
		{}
		std::string tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const override;
		llvm::Type* get_type() override
		{
			return type;
		}
	};
}
