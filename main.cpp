#include <iostream>

#include "ast.hpp"
#include "parser.hpp"

extern int yylex();

extern ASTBlock* programBlock;
extern llvm::LLVMContext TheContext;
extern llvm::IRBuilder<> TheBuilder(TheContext);
extern llvm::Module* TheModule;

int main() {
  TheModule = new llvm::Module("target", TheContext);

  llvm::FunctionType* fooPrototype = llvm::FunctionType::get(
    llvm::Type::getFloatTy(TheContext), false
  );
  llvm::Function* foo = llvm::Function::Create(
    fooPrototype, llvm::GlobalValue::ExternalLinkage,
    "foo", TheModule
  );

  llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(
    TheContext, "entry", foo
  );
  TheBuilder.SetInsertPoint(entryBlock);

  if (!yylex()) {
    if (programBlock) {
      std::cout << generateGVSpec(programBlock);
    }
  }

  TheBuilder.CreateRet(variableValue("b"));

  llvm::verifyFunction(*foo);
  TheModule->print(llvm::outs(), NULL);
}
