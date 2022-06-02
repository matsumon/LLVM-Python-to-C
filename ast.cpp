#include <sstream>

#include "ast.hpp"
#include "parser.hpp"

/*
 * Simple template function to convert a value of any type to a string
 * representation.  The type must have an insertion operator (i.e. operator<<).
 */
template <class T>
std::string toString(const T& val) {
  std::ostringstream out;
  out << val;
  return out.str();
}


/*
 * Function to generate GraphView spec for the subtree rooted at any node in
 * an AST.
 *
 * @param node An AST node.
 *
 * @return Returns a string containing a complete GraphView specification to
 *   visualize the AST subtree rooted at node.
 */
std::string generateGVSpec(ASTNode* node) {
  std::string gvSpec = "digraph G {\n";
  std::string nodeName = "n0";
  node->generateGVSpec(nodeName, gvSpec);
  gvSpec += "}\n";
  return gvSpec;
}

/****************************************************************************
 **
 ** Below is the implementation of generateGVSpec() for each class defined in
 ** ast.hpp.  Please see the documentation in ast.hpp for a desription of
 ** this function.
 **
 ****************************************************************************/

void ASTIdentifier::generateGVSpec(std::string nodeName, std::string& gvSpec) const {
  gvSpec += "  " + nodeName
    + " [shape=box,label=\"Identifier: " + *this->name + "\"];\n";
}


void ASTFloat::generateGVSpec(std::string nodeName, std::string& gvSpec) const {
  gvSpec += "  " + nodeName
    + " [shape=box,label=\"Float: " + toString(this->value) + "\"];\n";
}


void ASTInteger::generateGVSpec(std::string nodeName, std::string& gvSpec) const {
  gvSpec += "  " + nodeName
    + " [shape=box,label=\"Integer: " + toString(this->value) + "\"];\n";
}


void ASTBoolean::generateGVSpec(std::string nodeName, std::string& gvSpec) const {
  gvSpec += "  " + nodeName
    + " [shape=box,label=\"Boolean: " + toString(this->value) + "\"];\n";
}


void ASTBinaryOperatorExpression::generateGVSpec(std::string nodeName, std::string& gvSpec) const {
  std::string lhsNodeName = nodeName + "_lhs";
  std::string rhsNodeName = nodeName + "_rhs";
  std::string opStr;
  switch (this->op) {
    case PLUS:
      opStr = "PLUS";
      break;
    case MINUS:
      opStr = "MINUS";
      break;
    case TIMES:
      opStr = "TIMES";
      break;
    case DIVIDEDBY:
      opStr = "DIVIDEDBY";
      break;
    case EQ:
      opStr = "EQ";
      break;
    case NEQ:
      opStr = "NEQ";
      break;
    case GT:
      opStr = "GT";
      break;
    case GTE:
      opStr = "GTE";
      break;
    case LT:
      opStr = "LT";
      break;
    case LTE:
      opStr = "LTE";
      break;
  }

  gvSpec += "  " + nodeName + " [label=\"" + opStr + "\"];\n";
  gvSpec += "  " + nodeName + " -> " + lhsNodeName + " [label=\" LHS\"];\n";
  this->lhs->generateGVSpec(lhsNodeName, gvSpec);
  gvSpec += "  " + nodeName + " -> " + rhsNodeName + " [label=\" RHS\"];\n";
  this->rhs->generateGVSpec(rhsNodeName, gvSpec);
}


void ASTAssignmentStatement::generateGVSpec(std::string nodeName, std::string& gvSpec) const {
  std::string lhsNodeName = nodeName + "_lhs";
  std::string rhsNodeName = nodeName + "_rhs";
  gvSpec += "  " + nodeName + " [label=\"Assignment\"];\n";
  gvSpec += "  " + nodeName + " -> " + lhsNodeName + " [label=\" LHS\"];\n";
  this->lhs->generateGVSpec(lhsNodeName, gvSpec);
  gvSpec += "  " + nodeName + " -> " + rhsNodeName + " [label=\" RHS\"];\n";
  this->rhs->generateGVSpec(rhsNodeName, gvSpec);
}


void ASTBlock::generateGVSpec(std::string nodeName, std::string& gvSpec) const {
  gvSpec += "  " + nodeName + " [label=\"Block\"];\n";
  for (int i = 0; i < this->statements.size(); i++) {
    std::string childNodeName = nodeName + "_" + toString(i);
    gvSpec += "  " + nodeName + " -> " + childNodeName + " [label=\" " + toString(i) + "\"];\n";
    this->statements[i]->generateGVSpec(childNodeName, gvSpec);
  }
}


void ASTIfStatement::generateGVSpec(std::string nodeName, std::string& gvSpec) const {
  std::string conditionNodeName = nodeName + "_cond";
  std::string ifBlockNodeName = nodeName + "_if";
  gvSpec += "  " + nodeName + " [label=\"If\"];\n";
  gvSpec += "  " + nodeName + " -> " + conditionNodeName + "[label=\" Cond\"];\n";
  this->condition->generateGVSpec(conditionNodeName, gvSpec);
  gvSpec += "  " + nodeName + " -> " + ifBlockNodeName + "[label=\" If\"];\n";
  this->ifBlock->generateGVSpec(ifBlockNodeName, gvSpec);

  if (this->elseBlock) {
    std::string elseBlockNodeName = nodeName + "_else";
    gvSpec += "  " + nodeName + " -> " + elseBlockNodeName + "[label=\" Else\"];\n";
    this->elseBlock->generateGVSpec(elseBlockNodeName, gvSpec);
  }
}


void ASTWhileStatement::generateGVSpec(std::string nodeName, std::string& gvSpec) const {
  std::string conditionNodeName = nodeName + "_cond";
  std::string whileBlockNodeName = nodeName + "_while";
  gvSpec += "  " + nodeName + " [label=\"While\"];\n";
  gvSpec += "  " + nodeName + " -> " + conditionNodeName + "[label=\" Cond\"];\n";
  this->condition->generateGVSpec(conditionNodeName, gvSpec);
  gvSpec += "  " + nodeName + " -> " + whileBlockNodeName + ";\n";
  this->whileBlock->generateGVSpec(whileBlockNodeName, gvSpec);
}


void ASTBreakStatement::generateGVSpec(std::string nodeName, std::string& gvSpec) const {
  gvSpec += "  " + nodeName + " [label=\"Break\"];\n";
}

llvm::Function* initializeLLVM(){

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

  return foo;
}

llvm::Value* numericConstant(float val){
  return llvm::ConstantFP::get(TheContext, llvm::APFloat(val));
}

llvm::Value* variableValue(std::string name){
  llvm::Value* ptr = TheSymbolTable[name];
  if (!ptr) {
    std::cerr << "Unknown variable name: " << name << std::endl;
    return NULL;
  }
  return TheBuilder.CreateLoad(
    llvm::Type::getFloatTy(TheContext),
    ptr,
    name.c_str()
  );
}

llvm::Value* generateEntryBlockAlloca(std::string name){
  llvm::Function* currFn = TheBuilder.GetInsertBlock()->getParent();
  llvm::IRBuilder<> tmpBuilder(
    &currFn->getEntryBlock(),
    currFn->getEntryBlock().begin()
  );
  return tmpBuilder.CreateAlloca(
    llvm::Type::getFloatTy(TheContext), 0, name.c_str()
  );
}

llvm::Value* assignmentStatement(std::string lhs, llvm::Value* rhs) {
  if (!rhs) {
    return NULL;
  }

  if (!TheSymbolTable.count(lhs)) {
    TheSymbolTable[lhs] = generateEntryBlockAlloca(lhs);
  }
  return TheBuilder.CreateStore(rhs, TheSymbolTable[lhs]);
}

llvm::Value * ASTFloat::generateLLVM()const{
  return numericConstant(value);
}

llvm::Value* ASTIdentifier::generateLLVM()const{
  if(!TheSymbolTable.count(*name)){
    TheSymbolTable[*name] = NULL;
  }
  return NULL;
  // return *name;
}

llvm::Value * ASTAssignmentStatement::generateLLVM()const{
  std::cout<<rhs<<"ast.cpp line 237"<<std::endl;
   TheBuilder.CreateRet(variableValue("a"));
  // return  assignmentStatement(*lhs->name, numericConstant(2));
  return  assignmentStatement("a", numericConstant(2));
  // return NULL;
}

void ASTBlock::generateLLVM()const{
  llvm::Value * llvmValue;
  for (int i = 0; i < statements.size(); i++) {
    llvmValue= statements[i]->generateLLVM();
  }
}

void traverseLLVM(ASTNode* node){
    node->generateLLVM();
}