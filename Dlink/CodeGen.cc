#include "Dlink/CodeGen.hh"
#include "Dlink/ParseStruct.hh"
#include "Dlink/Error.hh"

#include <iostream>

namespace Dlink
{
	llvm::LLVMContext context;
	std::shared_ptr<llvm::Module> module = std::make_shared<llvm::Module>("top", context);
	llvm::IRBuilder<> builder(context);
	std::unique_ptr<llvm::legacy::FunctionPassManager> func_pm;
	std::map<std::string, llvm::Value*> sym_map;
	std::map<std::string, std::shared_ptr<Type>> sym_typemap;
	std::map<std::string, llvm::GetElementPtrInst*> sym_accessmap;
	std::map<std::string, std::shared_ptr<ClassType>> classes;

	ErrorList code_gen_errors;

	llvm::ConstantInt* constant_int32_0 =
		llvm::ConstantInt::get(module->getContext(),
							   llvm::APInt(32, llvm::StringRef("0"), 10));
}

//Expressions
namespace Dlink
{
	llvm::Value* Block::code_gen()
	{
		llvm::Value* last_value = nullptr;
		for (auto statement : statements)
		{
			last_value = statement->code_gen();
		}

		return last_value;
	}

	llvm::Value* Scope::code_gen()
	{
		llvm::Value* last_value = nullptr;

		auto temp_map = sym_map;
		auto temp_map_type = sym_typemap;
		auto temp_map_access = sym_accessmap;
		for (auto statement : statements)
		{
			last_value = statement->code_gen();
		}
		sym_map = temp_map;
		sym_typemap = temp_map_type;
		sym_accessmap = temp_map_access;
		return last_value;
	}

#ifdef alloca
#define DLINK_MACRO_ALLOCA_PUSH alloca
#undef alloca
#endif

	llvm::Value* VariableDeclaration::code_gen()
	{
		if (type->is_const)
		{
			if (expression == nullptr)
			{
				code_gen_errors.push_back(Error("Expected initialize value when declaring constant", line));
				return nullptr;
			}

			llvm::Value* init_expression = expression->code_gen();
			sym_map[id.id.data] = init_expression;
			sym_typemap[id.id.data] = type;

			return init_expression;
		}
		else
		{
			llvm::AllocaInst* alloca = builder.CreateAlloca(type->get_type(), nullptr, id.id.data);
			alloca->setAlignment(4);

			if (expression != nullptr)
			{
				llvm::Value* init_expression = expression->code_gen();
				builder.CreateStore(init_expression, alloca);
			}

			sym_map[id.id.data] = alloca;
			sym_typemap[id.id.data] = type;
			value_ = alloca;
			return alloca;
		}
	}

#ifdef DLINK_MACRO_ALLOCA_PUSH
#define alloca DLINK_MACRO_ALLOCA_PUSH
#undef DLINK_MACRO_ALLOCA_PUSH
#endif

	llvm::Value* FunctionDeclaration::code_gen()
	{
		std::vector<llvm::Type*> arg_types;
		for (const VariableDeclaration& argument : arg_list)
		{
			arg_types.push_back(argument.type->get_type());
		}

		llvm::FunctionType* function_type = llvm::FunctionType::get(ret_type->get_type(), arg_types, false);
		llvm::Function* function = llvm::Function::Create(function_type, llvm::GlobalValue::ExternalLinkage, id.id.data, module.get());

		std::size_t i = 0;
		for (auto& argument : function->args())
		{
			argument.setName(arg_list[i++].id.id.data);
		}

		llvm::BasicBlock* function_block = llvm::BasicBlock::Create(context, "entry", function, nullptr);

		builder.SetInsertPoint(function_block);
		sym_map.clear();
		sym_accessmap.clear();

		auto args = function->args();

		for (auto& argument : args)
		{
			llvm::AllocaInst* alloca_inst = builder.CreateAlloca(argument.getType(), &argument);
			sym_map[argument.getName()] = alloca_inst;
		}

		llvm::ReturnInst* ret_inst = llvm::dyn_cast<llvm::ReturnInst>(body->code_gen());
		if (!ret_inst)
		{
			if (builder.getCurrentFunctionReturnType() != builder.getVoidTy())
			{
				builder.CreateRet(llvm::Constant::getNullValue(builder.getCurrentFunctionReturnType()));
			}
			else
			{
				builder.CreateRetVoid();
			}
		}

		func_pm->run(*function);

		return function;
	}

	llvm::Value* MethodDeclaration::code_gen()
	{
		// TODO

		return nullptr;
	}

	llvm::Value* ClassDeclaration::code_gen()
	{
		llvm::StructType* struct_type =
			llvm::StructType::create(module->getContext(), std::string("class.") + id.id.data);

		std::vector<llvm::Type*> f;

		for (const auto& f_i : fields)
		{
			f.push_back(f_i.type->get_type());
		}

		struct_type->setBody(f, false);

		classes[id.id.data]->type = struct_type;

		return nullptr;
	}

	llvm::Value* Return::code_gen()
	{
		return builder.CreateRet(ret_expr->code_gen());
	}

	llvm::Value* IfElse::code_gen()
	{
		llvm::Function* parent_func = builder.GetInsertBlock()->getParent();

		llvm::BasicBlock* if_block;
		llvm::BasicBlock* then_block;
		llvm::BasicBlock* else_block;
		llvm::BasicBlock* endif_block;

		if (false_body != nullptr)
		{
			if_block = llvm::BasicBlock::Create(context, "if", parent_func);
			then_block = llvm::BasicBlock::Create(context, "then", parent_func);
			else_block = llvm::BasicBlock::Create(context, "else", parent_func);
			endif_block = llvm::BasicBlock::Create(context, "endif", parent_func);
		}
		else
		{
			if_block = llvm::BasicBlock::Create(context, "if", parent_func);
			then_block = llvm::BasicBlock::Create(context, "then", parent_func);
			endif_block = llvm::BasicBlock::Create(context, "endif", parent_func);
		}

		builder.CreateBr(if_block);
		builder.SetInsertPoint(if_block);

		llvm::Value* cond_value = cond_expr->code_gen();
		llvm::Value* cond_cmp = builder.CreateICmpNE(cond_value, llvm::ConstantInt::get(cond_value->getType(), 0, true), "branch");

		if (false_body != nullptr)
		{

			builder.CreateCondBr(cond_cmp, then_block, else_block);
			builder.SetInsertPoint(then_block);
			true_body->code_gen();
			builder.CreateBr(endif_block);
			builder.SetInsertPoint(else_block);
			false_body->code_gen();
			builder.CreateBr(endif_block);
			builder.SetInsertPoint(endif_block);
		}
		else
		{
			builder.CreateCondBr(cond_cmp, then_block, endif_block);
			builder.SetInsertPoint(then_block);
			true_body->code_gen();
			builder.CreateBr(endif_block);
			builder.SetInsertPoint(endif_block);
		}

		return cond_cmp;
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
		std::size_t start_pos = 0;

		static std::map<std::string, std::string> escape =
		{
			{ R"(\n)", "\n" },
			{ R"(\r)", "\r" },
		};

		std::string fix = data;
		for (const auto e : escape)
		{
			while ((start_pos = data.find(e.first, start_pos)) != std::string::npos)
			{
				fix.replace(start_pos, e.first.length(), e.second);
				start_pos += e.second.length();
			}
		}

		return builder.CreateGlobalStringPtr(fix.c_str());
	}

	llvm::Value* Identifier::code_gen()
	{
		if (is_member)
			return builder.getFalse();

		llvm::Value* value = sym_map[id.data];
		if (!value)
		{
			code_gen_errors.push_back(Error("Undefined symbol \"" + id.data + "\"", line));
			return builder.getFalse();
		}

		if (sym_typemap[id.data]->is_const)
		{
			return value;
		}
		else
		{
			return builder.CreateLoad(value);
		}
	}

	llvm::Value* UnaryOP::code_gen()
	{
		llvm::Value* rhs_value = rhs->code_gen();

		switch (op.type)
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
		case TokenType::equal:
		{
			if (dynamic_cast<BinaryOP*>(lhs.get()))
			{
				builder.CreateStore(rhs_value, lhs_value);
				return rhs_value;
			}
			else
			{
				llvm::LoadInst* load_inst = dynamic_cast<llvm::LoadInst*>(lhs_value);
				if (!load_inst)
				{
					code_gen_errors.push_back(Error("Expected lvalue for left hand side of assignment operation", line));
					return nullptr;
				}
				builder.CreateStore(rhs_value, load_inst->getPointerOperand());
				return rhs_value;
			}
		}
		case TokenType::dot:
		{
			llvm::LoadInst* load_inst = static_cast<llvm::LoadInst*>(lhs_value);
			if (!load_inst)
			{
				code_gen_errors.push_back(Error("Expected lvalue for left hand side of assignment operation", line));
				return nullptr;
			}

			auto load_inst_type = load_inst->getType();
			auto iter_c = std::find_if(classes.begin(), classes.end(),
									   [&load_inst_type](const std::pair<std::string, std::shared_ptr<ClassType>>& a)
			{
				return a.second->type == load_inst_type;
			});
			if (iter_c != classes.end())
			{
				auto rhs_name = std::dynamic_pointer_cast<Identifier>(rhs)->id.data;

				auto iter_f =
					std::find_if(iter_c->second->fields.begin(), iter_c->second->fields.end(),
								 [&](const FieldDeclaration& f) {
					return f.id.id.data == rhs_name;
				});

				if (iter_f == iter_c->second->fields.end())
				{
					code_gen_errors.push_back(Error("Expected inaccessible field or method.", line));
					return nullptr;
				}
				std::string val = std::to_string([&] {
					return (int)std::distance(iter_c->second->fields.begin(),
											  iter_f);
				}());
				std::string temp_id = "member." + (std::dynamic_pointer_cast<Identifier>(lhs))->id.data +
					'.' + (std::dynamic_pointer_cast<Identifier>(rhs))->id.data;
				llvm::Value* ptr_val = load_inst->getPointerOperand();
				auto constant_int = llvm::ConstantInt::get(module->getContext(),
														   llvm::APInt(32, llvm::StringRef(val), 10));

				llvm::GetElementPtrInst* ptr = nullptr;

				if (sym_accessmap.find(temp_id) == sym_accessmap.end())
				{
					ptr = (llvm::GetElementPtrInst*)builder.CreateGEP(load_inst_type, ptr_val, {
					constant_int32_0,
					constant_int
					}, temp_id);

					sym_accessmap[temp_id] = ptr;
				}
				else
				{
					ptr = sym_accessmap[temp_id];
				}

				return ptr;
			}
			else
			{
				code_gen_errors.push_back(Error("Expected the identifier to the left of dot must be an instance of the class.", line));
				return nullptr;
			}
		}
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
		case TokenType::modulo:
			return builder.CreateSRem(lhs_value, rhs_value);
		default:
			code_gen_errors.push_back(Error("Undefined binary operator", line));
			return builder.getFalse();
		}
	}

	llvm::Value* FuncCall::code_gen()
	{
		llvm::Function* function = module->getFunction(id.id.data);

		if (!function)
		{
			code_gen_errors.push_back(Error("Undefined function \"" + id.id.data + "\"", line));
			return builder.getFalse();
		}

		std::vector<llvm::Value*> args_value;
		for (std::shared_ptr<Expression> arg : args)
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
		if (id.id.data == "bool" ||
			id.id.data == "byte") return builder.getInt8Ty();
		else if (id.id.data == "short") return builder.getInt16Ty();
		else if (id.id.data == "int") return builder.getInt32Ty();
		else if (id.id.data == "long") return builder.getInt64Ty();
		else if (id.id.data == "half") return builder.getHalfTy();
		else if (id.id.data == "float") return builder.getFloatTy();
		else if (id.id.data == "double") return builder.getDoubleTy();
		else return builder.getVoidTy();
	}
	
	llvm::Type* PointerType::get_type()
	{
		llvm::Type* pointee_type = type->get_type();
		return pointee_type->getPointerTo();
	}

	llvm::Type* ConstType::get_type()
	{
		return type->get_type();
	}
}
