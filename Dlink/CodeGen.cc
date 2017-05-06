#include "Dlink/CodeGen.hh"
#include "Dlink/ParseStruct.hh"
#include "Dlink/Error.hh"

#include <iostream>

namespace Dlink
{
	llvm::LLVMContext context;
	std::shared_ptr<llvm::Module> module = std::make_shared<llvm::Module>("top", context);
	llvm::IRBuilder<> builder(context);
	std::map<std::string, llvm::Value*> sym_map;

	ErrorList code_gen_errors;
}

//Expressions
namespace Dlink
{
	llvm::Value* Block::code_gen()
	{
		llvm::Value* last_value;
		for(auto statement : statements)
		{
			last_value = statement->code_gen();
		}
		
		return last_value;
	}

	llvm::Value* VariableDeclaration::code_gen()
	{
		llvm::AllocaInst* alloca = builder.CreateAlloca(type->get_type(), nullptr, id.id.data);	
		alloca->setAlignment(4);	

		if(expression != nullptr)
		{
			llvm::Value* init_expression = expression->code_gen();
			builder.CreateStore(init_expression, alloca);	
		}

		sym_map[id.id.data] = alloca;
		
		return alloca;
	}

	llvm::Value* FunctionDeclaration::code_gen()
	{
		std::vector<llvm::Type * > arg_types;
		for(const VariableDeclaration& argument : arg_list)
		{
			arg_types.push_back(argument.type->get_type());
		}
		
		llvm::FunctionType* function_type = llvm::FunctionType::get(ret_type->get_type(), arg_types, false);
		llvm::Function* function = llvm::Function::Create(function_type, llvm::GlobalValue::ExternalLinkage, id.id.data, module.get());

		std::size_t i = 0;
		for(auto& argument : function->args())
		{
			argument.setName(arg_list[i++].id.id.data);
		}

		llvm::BasicBlock* function_block = llvm::BasicBlock::Create(context, "entry", function, nullptr);

		builder.SetInsertPoint(function_block);
		sym_map.clear();

		for(auto& argument : function->args())
		{
			sym_map[argument.getName()] = &argument;
		}

		body->code_gen();
		return function;
	}

	llvm::Value* Return::code_gen()
	{
		return builder.CreateRet(ret_expr->code_gen());
	}

	llvm::Value* ExpressionStatement::code_gen()
	{
		return expression->code_gen();
	}
}

//Statements
namespace Dlink
{
	llvm::Value* Integer32::code_gen()
	{
		return builder.getInt32(data);
	}
	
	llvm::Value* Float::code_gen()
	{
		return llvm::ConstantFP::get(builder.getFloatTy(), data);
	}

	llvm::Value* String::code_gen()
	{
		return llvm::ConstantDataArray::getString(context, data, true);
	}

	llvm::Value* Identifier::code_gen()
	{
		llvm::Value* value = sym_map[id.data];
		if(!value)
		{
			code_gen_errors.push_back(Error("Undefined symbol \"" + id.data + "\"", line));
			return builder.getFalse();
		}
		
		if(value->getType()->isPointerTy())
		{
			return builder.CreateLoad(value);
		}
		else
		{
			return value;
		}
	}

	llvm::Value* UnaryOP::code_gen()
	{
		llvm::Value* rhs_value = rhs->code_gen();
		
		switch(op.type)
		{
		case TokenType::bit_not:
			return builder.CreateNot(rhs_value);
		case TokenType::plus:
			return builder.CreateAdd(builder.getInt32(0), rhs_value);	
		case TokenType::minus:
			return builder.CreateSub(builder.getInt32(0), rhs_value);
		case TokenType::pre_inc:
			builder.CreateStore(builder.CreateAdd(rhs_value, builder.getInt32(1)), rhs_value);
			return rhs_value;
		case TokenType::post_inc:
			builder.CreateStore(builder.CreateAdd(rhs_value, builder.getInt32(1)), rhs_value);
			return builder.CreateSub(rhs_value, builder.getInt32(1));
		case TokenType::pre_dec:
			builder.CreateStore(builder.CreateSub(rhs_value, builder.getInt32(1)), rhs_value);
			return rhs_value;
		case TokenType::post_dec:
			builder.CreateStore(builder.CreateSub(rhs_value, builder.getInt32(1)), rhs_value);
			return builder.CreateAdd(rhs_value, builder.getInt32(1));
		default:
			code_gen_errors.push_back(Error("Undefined unary operator", line));
			return builder.getFalse();	
		}
	}

	llvm::Value* BinaryOP::code_gen()
	{
		llvm::Value* lhs_value = lhs->code_gen();
		llvm::Value* rhs_value = rhs->code_gen();

		switch (op.type)
		{
		case TokenType::bit_or:
			return builder.CreateOr(lhs_value, rhs_value);
		case TokenType::bit_xor:
			return builder.CreateXor(lhs_value, rhs_value);
		case TokenType::bit_and:
			return builder.CreateAnd(lhs_value, rhs_value);
		case TokenType::comp_equal:
			return builder.CreateICmpEQ(lhs_value, rhs_value);
		case TokenType::comp_noteq:
			return builder.CreateICmpNE(lhs_value, rhs_value);
		case TokenType::comp_greater:
			return builder.CreateICmpSGT(lhs_value, rhs_value);
		case TokenType::comp_less:
			return builder.CreateICmpSGT(rhs_value, lhs_value);
		case TokenType::comp_eqgreater:
			return builder.CreateOr(builder.CreateICmpEQ(lhs_value, rhs_value), builder.CreateICmpSGT(lhs_value, rhs_value));
		case TokenType::comp_eqless:
			return builder.CreateOr(builder.CreateICmpEQ(lhs_value, rhs_value), builder.CreateICmpSGT(rhs_value, lhs_value));
		case TokenType::bit_lshift:
			return builder.CreateShl(lhs_value, rhs_value);
		case TokenType::bit_rshift:
			return builder.CreateAShr(lhs_value, rhs_value);
		case TokenType::plus:
			return builder.CreateAdd(lhs_value, rhs_value);
		case TokenType::minus:
			return builder.CreateSub(lhs_value, rhs_value);
		case TokenType::multiply:
			return builder.CreateMul(lhs_value, rhs_value);
		case TokenType::divide:
			return builder.CreateSDiv(lhs_value, rhs_value);
		default:
			code_gen_errors.push_back(Error("Undefined binary operator", line));
			return builder.getFalse();
		}
	}

	llvm::Value* FuncCall::code_gen()
	{
		llvm::Function* function = module->getFunction(id.id.data);
		
		if(!function)
		{
			code_gen_errors.push_back(Error("Undefined function \"" + id.id.data + "\"", line));
			return builder.getFalse();
		}

		std::vector<llvm::Value*> args_value;
		for(std::shared_ptr<Expression> arg : args)
		{
			args_value.push_back(arg->code_gen());
		}

		return builder.CreateCall(function, args_value);
	}
}

//Types
namespace Dlink
{
	llvm::Type* IdentifierType::get_type()
	{
		if(id.id.data == "bool" ||
		   id.id.data == "byte") return builder.getInt8Ty();
		else if(id.id.data == "short") return builder.getInt16Ty();
		else if(id.id.data == "int") return builder.getInt32Ty();
		else if(id.id.data == "long") return builder.getInt64Ty();
		else if(id.id.data == "half") return builder.getHalfTy();
		else if(id.id.data == "float") return builder.getFloatTy();
		else if(id.id.data == "double") return builder.getDoubleTy();
		else return builder.getVoidTy();
	}
}
