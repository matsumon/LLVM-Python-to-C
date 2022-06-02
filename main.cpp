#include <iostream>

#include "ast.hpp"
#include "parser.hpp"

extern int yylex();
extern ASTBlock* programBlock;

extern llvm::LLVMContext TheContext;
extern llvm::IRBuilder<> TheBuilder;
extern llvm::Module* TheModule;

int main() {
  llvm::Function* foo = initializeLLVM();
  if (!yylex()) {
    if (programBlock) {
      std::cout << generateGVSpec(programBlock);
    }
  }
std::cout<<"HERE"<<std::endl;
  llvm::verifyFunction(*foo);
std::cout<<"HERE"<<std::endl;
  TheModule->print(llvm::outs(), NULL);
}
