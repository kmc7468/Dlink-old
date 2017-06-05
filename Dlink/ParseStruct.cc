#include "Dlink/ParseStruct.hh"
#include "Dlink/CodeGen.hh"

//Expression nodes
namespace Dlink
{
	std::string Integer32::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		return std::string(depth * 6, ' ') + "Integer32 : " + std::to_string(data);
	}

	std::string Float::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		return std::string(depth * 6, ' ') + "Float : " + std::to_string(data);
	}

	std::string String::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		return std::string(depth * 6, ' ') + "String : " + data;
	}

	std::string Identifier::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		return std::string(depth * 6, ' ') + "Identifier : " + id.data;
	}
	std::string Const_Null::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		return std::string(depth * 6, ' ') + "Constant: null";
	}
	std::string UnaryOP::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		std::string tree = std::string(depth * 6, ' ') + "UnaryOP(" + tokentype_map[op.type] + ")\n";
		tree += rhs->tree_gen(depth + 1, tokentype_map);
		return tree;
	}

	std::string BinaryOP::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		std::string tree = std::string(depth * 6, ' ') + "BinaryOP(" + tokentype_map[op.type] + ")\n";
		tree += lhs->tree_gen(depth + 1, tokentype_map) + "\n";
		tree += rhs->tree_gen(depth + 1, tokentype_map);
		return tree;
	}

	std::string FuncCall::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		std::string tree = std::string(depth * 6, ' ') + "FuncCall\n";
		depth++;
		tree += std::string(depth * 6, ' ') + "Name : " + id.id.data + "\n";
		tree += std::string(depth * 6, ' ') + "Args : \n";

		for (auto&& arg : args)
		{
			tree += arg->tree_gen(depth + 1, tokentype_map) + "\n";
		}

		tree.pop_back();

		return tree;
	}
}

//Statement nodes
namespace Dlink
{
	std::string Block::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		std::string tree = std::string(depth * 6, ' ') + "Block Begin\n";
		depth++;

		for (auto statement : statements)
		{
			tree += statement->tree_gen(depth, tokentype_map) + "\n";
		}

		depth--;
		tree += std::string(depth * 6, ' ') + "End Block";
		return tree;
	}

	std::string Scope::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		std::string tree = std::string(depth * 6, ' ') + "Scope Begin\n";
		depth++;

		for (auto statement : statements)
		{
			tree += statement->tree_gen(depth, tokentype_map) + "\n";
		}

		depth--;
		tree += std::string(depth * 6, ' ') + "End Scope";
		return tree;
	}

	std::string VariableDeclaration::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		std::string tree = std::string(depth * 6, ' ') + "Variable Decl Begin\n";
		tree += std::string(depth * 6, ' ') + "Type : \n";
		tree += type->tree_gen(depth + 1, tokentype_map) + "\n";
		tree += std::string(depth * 6, ' ') + "Name : " + id.id.data + "\n";

		if (expression)
		{
			tree += std::string(depth * 6, ' ') + "Init expression : \n";
			tree += expression->tree_gen(depth + 1, tokentype_map) + "\n";
		}

		tree += std::string(depth * 6, ' ') + "End Variable Decl";
		return tree;
	}

	std::string FieldDeclaration::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		std::string tree = std::string(depth * 6, ' ') + "Field Decl Begin\n";
		tree += std::string(depth * 6, ' ') + "Type : \n";
		tree += type->tree_gen(depth + 1, tokentype_map) + "\n";
		tree += std::string(depth * 6, ' ') + "Is Dynamic : " +
			(is_dynamic_field ? "Dynamic" : "Static") + '\n';
		tree += std::string(depth * 6, ' ') + "Name : " + id.id.data + "\n";

		if (expression)
		{
			tree += std::string(depth * 6, ' ') + "Init expression : \n";
			tree += expression->tree_gen(depth + 1, tokentype_map) + "\n";
		}

		tree += std::string(depth * 6, ' ') + "End Field Decl";
		return tree;
	}

	std::string FunctionDeclaration::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		std::string tree = std::string(depth * 6, ' ') + "Function Decl Begin\n";
		tree += std::string(depth * 6, ' ') + "Return Type : \n";
		tree += ret_type->tree_gen(depth + 1, tokentype_map) + "\n";
		tree += std::string(depth * 6, ' ') + "Name : " + id.id.data + "\n";

		tree += std::string(depth * 6, ' ') + "Arguments : \n";
		for (auto arg : arg_list)
		{
			tree += arg.tree_gen(depth + 1, tokentype_map) + "\n";
		}

		tree += std::string(depth * 6, ' ') + "Body : \n";
		tree += body->tree_gen(depth + 1, tokentype_map) + "\n";
		tree += std::string(depth * 6, ' ') + "End Func Decl";
		return tree;
	}

	std::string MethodDeclaration::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		std::string tree = std::string(depth * 6, ' ') + "Method Decl Begin\n";
		tree += std::string(depth * 6, ' ') + "Return Type : \n";
		tree += ret_type->tree_gen(depth + 1, tokentype_map) + '\n';
		tree += std::string(depth * 6, ' ') + "Is Dynamic : " +
			(is_dynamic_method ? "Dynamic" : "Static") + '\n';
		tree += std::string(depth * 6, ' ') + "Name : " + id.id.data + '\n';

		tree += std::string(depth * 6, ' ') + "Arguments : \n";
		for (auto arg : arg_list)
		{
			tree += arg.tree_gen(depth + 1, tokentype_map) + '\n';
		}

		tree += std::string(depth * 6, ' ') + "Body : \n";
		tree += body->tree_gen(depth + 1, tokentype_map) + '\n';
		tree += std::string(depth * 6, ' ') + "End Method Decl End";
		return tree;
	}

	ClassDeclaration::ClassDeclaration(const Identifier& _id, const std::vector<FieldDeclaration>& _fields,
									   const std::vector<MethodDeclaration>& _methods)
		: id(_id), fields(_fields), methods(_methods)
	{
		classes.insert(std::make_pair(id.id.data,
									  std::make_shared<ClassType>(id, nullptr, *this)));
	}
	std::string ClassDeclaration::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		std::string tree = std::string(depth * 6, ' ') + "Class Decl Begin\n";
		tree += std::string(depth * 6, ' ') + "Name : " + id.id.data + '\n';

		tree += std::string(depth * 6, ' ') + "Non-Static Methods : \n";
		for (const auto& m : methods)
		{
			tree += m.tree_gen(depth + 1, tokentype_map) + '\n';
		}

		tree += std::string(depth * 6, ' ') + "Non-Static Fields : \n";
		for (const auto& f : fields)
		{
			tree += f.tree_gen(depth + 1, tokentype_map) + '\n';
		}

		tree += std::string(depth * 6, ' ') + "End Class Decl";
		return tree;
	}

	std::string Return::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		std::string tree = std::string(depth * 6, ' ') + "Return : \n";
		tree += ret_expr->tree_gen(depth + 1, tokentype_map);
		return tree;
	}

	std::string IfElse::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		std::string tree = std::string(depth * 6, ' ') + "If\n";
		tree += std::string(depth * 6, ' ') + "Condition : \n";
		tree += cond_expr->tree_gen(depth + 1, tokentype_map) + "\n";
		tree += std::string(depth * 6, ' ') + "Then : \n";
		tree += true_body->tree_gen(depth + 1, tokentype_map) + "\n";

		if (false_body)
		{
			tree += std::string(depth * 6, ' ') + "Else : \n";
			tree += false_body->tree_gen(depth + 1, tokentype_map) + "\n";
		}

		tree += std::string(depth * 6, ' ') + "End If";
		return tree;
	}

	std::string ExpressionStatement::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		return expression->tree_gen(depth, tokentype_map);
	}
}

//Type nodes
namespace Dlink
{
	std::string IdentifierType::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		return std::string(depth * 6, ' ') + "IdentifierType(" + id.id.data + ")";
	}
	
	std::string PointerType::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		std::string tree = std::string(depth * 6, ' ') + "PointerType : \n";
		tree += type->tree_gen(depth + 1, tokentype_map);
		return tree;
	}

	std::string ClassType::tree_gen(std::size_t depth, std::map<TokenType, std::string> tokentype_map) const
	{
		return std::string(depth * 6, ' ') + "ClassType(" + id.id.data + ")";
	}
}
