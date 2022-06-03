#include <sstream>

#include "ast.hpp"
#include "parser.hpp"



llvm::LLVMContext TheContext;
llvm::IRBuilder<> TheBuilder(TheContext);
llvm::Module* TheModule;
std::map<std::string, llvm::Value*> TheSymbolTable;

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

  llvm::FunctionType* targetPrototype = llvm::FunctionType::get(
    llvm::Type::getFloatTy(TheContext), false
  );
  llvm::Function* target = llvm::Function::Create(
    targetPrototype, llvm::GlobalValue::ExternalLinkage,
    "target", TheModule
  );

  llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(
    TheContext, "entry", target
  );
  TheBuilder.SetInsertPoint(entryBlock);

  return target;
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
  if (rhs == NULL) {
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
  return variableValue(*name);
}

llvm::Value * ASTAssignmentStatement::generateLLVM()const{
  llvm::Value * newRHS = rhs->generateLLVM();
  return  assignmentStatement(*lhs->name, newRHS);
}

llvm::Value * ASTBlock::generateLLVM()const{
  llvm::Value * llvmValue;
  for (int i = 0; i < this->statements.size(); i++) {
    llvmValue = this->statements[i]->generateLLVM();
    // valueStatements.push_back(llvmValue);
  }
  return NULL;
}
llvm::Value * ASTInteger::generateLLVM()const{
  return numericConstant((float)value);
}
llvm::Value * ASTBoolean::generateLLVM()const{
  llvm::Value * newLHS = numericConstant((float)value);
  // newLHS = TheBuilder.CreateFCmpUGE(numericConstant((float)value), numericConstant((float)1), "gtetmp");
  return TheBuilder.CreateFCmpUGE(numericConstant((float)value), numericConstant((float)1), "gtetmp");
  return TheBuilder.CreateFPToUI(
    newLHS,
    llvm::Type::getFloatTy(TheContext),
    "gtebooltmp"
  );
  // return numericConstant((float)value);
}
llvm::Value * ASTBinaryOperatorExpression::generateLLVM()const{
  llvm::Value * newLHS = lhs->generateLLVM();
  llvm::Value * newRHS = rhs->generateLLVM();
  switch (op) {
    case PLUS:
      return TheBuilder.CreateFAdd(newLHS, newRHS, "addtmp");
    case MINUS:
      return TheBuilder.CreateFSub(newLHS, newRHS, "subtmp");
    case TIMES:
      return TheBuilder.CreateFMul(newLHS, newRHS, "multmp");
    case DIVIDEDBY:
      return TheBuilder.CreateFDiv(newLHS, newRHS, "divtmp");
    case LT:
      newLHS = TheBuilder.CreateFCmpULT(newLHS, newRHS, "lttmp");
      return TheBuilder.CreateUIToFP(
        newLHS,
        llvm::Type::getFloatTy(TheContext),
        "ltbooltmp"
      );
    case LTE:
      // return TheBuilder.CreateFCmpULE(newLHS, newRHS, "ltetmp");
      newLHS = TheBuilder.CreateFCmpULE(newLHS, newRHS, "ltetmp");
      return TheBuilder.CreateUIToFP(
        newLHS,
        llvm::Type::getFloatTy(TheContext),
        "ltebooltmp"
      );
    case GT:
      // return TheBuilder.CreateFCmpUGT(newLHS, newRHS, "gttmp");
      newLHS = TheBuilder.CreateFCmpUGT(newLHS, newRHS, "gttmp");
      return TheBuilder.CreateUIToFP(
        newLHS,
        llvm::Type::getFloatTy(TheContext),
        "gtbooltmp"
      );
    case GTE:
      // return TheBuilder.CreateFCmpUGE(newLHS, newRHS, "gtetmp");
      newLHS = TheBuilder.CreateFCmpUGE(newLHS, newRHS, "gtetmp");
      return TheBuilder.CreateUIToFP(
        newLHS,
        llvm::Type::getFloatTy(TheContext),
        "gtebooltmp"
      );
    case NEQ:
      // return TheBuilder.CreateFCmpUNE(newLHS, newRHS, "netmp");
      newLHS = TheBuilder.CreateFCmpUNE(newLHS, newRHS, "netmp");
      return TheBuilder.CreateUIToFP(
        newLHS,
        llvm::Type::getFloatTy(TheContext),
        "nebooltmp"
      );
    case EQ:
      // return TheBuilder.CreateFCmpUEQ(newLHS, newRHS, "eqtmp");
      newLHS = TheBuilder.CreateFCmpUEQ(newLHS, newRHS, "eqtmp");
      return TheBuilder.CreateUIToFP(
        newLHS,
        llvm::Type::getFloatTy(TheContext),
        "nebooltmp"
      );
    default:
      std::cerr << "Invalid operator: " << op << std::endl;
      return NULL;
  }
}

llvm::Value * ASTIfStatement::generateLLVM()const{
  llvm::Value* cond = condition->generateLLVM();
   cond = TheBuilder.CreateFCmpONE(
    cond, numericConstant(0), "ifcond"
  );
  llvm::Function* currFn = TheBuilder.GetInsertBlock()->getParent();

  llvm::BasicBlock* ifBlockOne = llvm::BasicBlock::Create(
    TheContext,
    "ifBlock",
    currFn
  );

  llvm::BasicBlock* elseBlockOne = llvm::BasicBlock::Create(
    TheContext,
    "elseBlock"
  );
  llvm::BasicBlock* continuationBlock = llvm::BasicBlock::Create(
    TheContext,
    "ifContinueBlock"
  );

  TheBuilder.CreateCondBr(cond, ifBlockOne, elseBlockOne);

  TheBuilder.SetInsertPoint(ifBlockOne);
  llvm::Value* ifStatement = ifBlock->generateLLVM();
  TheBuilder.CreateBr(continuationBlock);

  currFn->getBasicBlockList().push_back(elseBlockOne);
  TheBuilder.SetInsertPoint(elseBlockOne);
  if(elseBlock){
    llvm::Value* elseStatment = elseBlock->generateLLVM();
  }
  TheBuilder.CreateBr(continuationBlock);

  currFn->getBasicBlockList().push_back(continuationBlock);
  TheBuilder.SetInsertPoint(continuationBlock);
  return continuationBlock;
}

void traverseLLVM(ASTNode* node){
    node->generateLLVM();
}

void generateObjFile(std::string filename) {
  std::string targetTriple = llvm::sys::getDefaultTargetTriple();
  std::cerr << "Target triple: " << targetTriple << std::endl;

  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();

  std::string err;
  const llvm::Target* target = llvm::TargetRegistry::lookupTarget(targetTriple, err);
  if (!target) {
    std::cerr << "Error looking up target: " << err << std::endl;
    return;
  }

  std::string cpu = "generic";
  std::string features = "";
  llvm::TargetOptions options;
  llvm::TargetMachine* targetMachine = target->createTargetMachine(
    targetTriple,
    cpu,
    features,
    options,
    llvm::Optional<llvm::Reloc::Model>()
  );

  TheModule->setDataLayout(targetMachine->createDataLayout());
  TheModule->setTargetTriple(targetTriple);

  std::error_code ec;
  llvm::raw_fd_ostream fd(filename, ec, llvm::sys::fs::OF_None);
  if (ec) {
    std::cerr << "Error opening optput file: " << ec.message() << std::endl;
    return;
  }
  llvm::legacy::PassManager pm;
  targetMachine->addPassesToEmitFile(pm, fd, NULL, llvm::CodeGenFileType::CGFT_ObjectFile);
  pm.run(*TheModule);
  fd.close();
}