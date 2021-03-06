#include "Dlink/Init.hh"

namespace Dlink
{
	void InitializePassManager()
	{
		func_pm = std::make_unique<llvm::legacy::FunctionPassManager>(module.get());

		if (Dlink::optimization_level > 1)
		{
			func_pm->add(llvm::createInstructionCombiningPass());
			func_pm->add(llvm::createReassociatePass());
			func_pm->add(llvm::createGVNPass());
			func_pm->add(llvm::createCFGSimplificationPass());
		}

		func_pm->doInitialization();
	}
}
