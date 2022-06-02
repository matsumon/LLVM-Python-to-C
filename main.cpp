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
generateLLVM(programBlock);
std::cout<<*programBlock->statements[0]->lhs->name<<std::endl;
std::cout<<"main.cpp 21"<<std::endl;
llvm::raw_ostream* OS = nullptr;
  llvm::verifyFunction(*foo,  OS);
std::cout<<"Verify Function:  "<<llvm::verifyFunction(*foo)<<" "<<OS<<std::endl;
  TheModule->print(llvm::outs(), NULL);
}
