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
      // std::cout << generateGVSpec(programBlock);
      generateGVSpec(programBlock);
    }
  }
  traverseLLVM(programBlock);
  TheBuilder.CreateRet(variableValue("return_value"));
  std::cout<<"main.cpp 21"<<std::endl;
  // llvm::raw_os_ostream OS(std::cout);
  // llvm::verifyFunction(*foo,  &OS);
  llvm::verifyFunction(*foo);
  std::cout<<"Verify Function:  "<<llvm::verifyFunction(*foo)<<" "<<std::endl;
  TheModule->print(llvm::outs(), NULL);
}
