#include <iostream>

#include "ast.hpp"
#include "parser.hpp"

extern int yylex();
extern ASTBlock* programBlock;

extern llvm::LLVMContext TheContext;
extern llvm::IRBuilder<> TheBuilder;
extern llvm::Module* TheModule;
int main(int argc, char **argv) {
  llvm::Function* target = initializeLLVM();
  if (!yylex()) {
    if (programBlock) {
      // std::cout << generateGVSpec(programBlock);
      generateGVSpec(programBlock);
    }
  }
  traverseLLVM(programBlock);
  TheBuilder.CreateRet(variableValue("return_value"));
  llvm::verifyFunction(*target);
  // std::cout<<"Verify Function:  "<<llvm::verifyFunction(*target)<<" "<<std::endl;
  TheModule->print(llvm::outs(), NULL);
  if(argc > 0){
    generateObjFile(argc[0]);
  }else{
    generateObjFile("target.o");
  }
}
