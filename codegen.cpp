#include "node.h"
#include "codegen.h"
#include "parser.hpp"

using namespace std;


// Compile the AST into a module
void CodeGenContext::generateCode(NBlock &root) {
	std::cout << "Generating code...\n";
	
	// Create the top level interpreter function to call as entry
	vector<Type*> argTypes;
	FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), makeArrayRef(argTypes), false);
	mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "main", module);
	BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", mainFunction, 0);
	
	
	// Push a new variable/block context
	pushBlock(bblock);
	root.codeGen(*this); // emit bytecode for the top level block.
	ReturnInst::Create(getGlobalContext(), bblock);
	
	
	// Print the bytecode in a human-readable format to see if our program compiled properly
	std::cout << "Code is generated.\n";
	PassManager pm;
	pm.add(createPrintModulePass(&outs()));
	pm.run(*module);
}


// Executes the AST by running the main function
GenericValue CodeGenContext::runCode() {
	std::cout << "Running code...\n";
	ExecutionEngine *ee = EngineBuilder(module).create();
	vector<GenericValue> noargs;
	GenericValue v = ee->runFunction(mainFunction, noargs);
	std::cout << "Code was run..." << v.DoubleVal << " " << v.PointerVal;
	return v;
}


// Returns an LLVM type based on the identifier
static Type *typeOf(const NIdentifier& type) {
	if (type._name.compare("int") == 0) {
		return Type::getInt64Ty(getGlobalContext());
	} else if (type._name.compare("double") == 0) {
		return Type::getDoubleTy(getGlobalContext());
	} else if (type._name.compare("bool") == 0) {
		return Type::getInt1Ty(getGlobalContext());
	}
	
	return Type::getVoidTy(getGlobalContext());
}


// Code Generation

Value* NInteger::codeGen(CodeGenContext &context) {
	std::cout << "Creating integer: " << _value << std::endl;
	return ConstantInt::get(Type::getInt64Ty(getGlobalContext()), _value, true);
}


Value* NDouble::codeGen(CodeGenContext &context) {
	std::cout << "Creating double: " << _value << std::endl;
	return ConstantFP::get(Type::getDoubleTy(getGlobalContext()), _value);
}


Value* NBoolean::codeGen(CodeGenContext &context) {
	std::cout << "Creating boolean: " << _value << std::endl;
	return _value? ConstantInt::getTrue(getGlobalContext()) : ConstantInt::getFalse(getGlobalContext());
}


Value* NIdentifier::codeGen(CodeGenContext &context) {
	std::cout << "Creating identifier reference: " << _name << std::endl;
	if (context.locals().find(_name) == context.locals().end()) {
		std::cerr << "undeclared variable " << _name << std::endl;
		return NULL;
	}
	return new LoadInst(context.locals()[_name], "", false, context.currentBlock());
}


Value* NMethodCall::codeGen(CodeGenContext &context) {
	Function *function = context.module->getFunction(_id._name.c_str());
	if (function == NULL) {
		std::cerr << "No such function " << _id._name << std::endl;
	}
	std::vector<Value *> args;
	ExpressionList::const_iterator it;
	for (it = _arguments.begin(); it != _arguments.end(); it++) {
		args.push_back((**it).codeGen(context));
	}
	CallInst *call = CallInst::Create(function, makeArrayRef(args),"", context.currentBlock());
	std::cout << "Creating method call: " << _id._name << std::endl;
	return call;
}


Value* NBinaryOperator::codeGen(CodeGenContext &context) {
	std::cout << "Creating binary operation " << _op << std::endl;
	Instruction::BinaryOps instr = Instruction::BinaryOpsEnd;
	switch (_op) {
		case TPLUS:		instr = Instruction::Add; break;
		case TMINUS:	instr = Instruction::Sub; break;
		case TMUL:		instr = Instruction::Mul; break;
		case TDIV:		instr = Instruction::SDiv; break;
		
		default: break;
		
		// TODO: Comparison
	}
	
	if (instr != Instruction::BinaryOpsEnd) {
		return BinaryOperator::Create(instr, _lhs.codeGen(context), _rhs.codeGen(context), "", context.currentBlock());
	}
	
	// Try to see if _op is a comparison
	CmpInst::Predicate predicate;
	switch (_op) {
		case TCEQ: predicate = CmpInst::ICMP_EQ ; break;
		case TCNE: predicate = CmpInst::ICMP_NE ; break;
		case TCLT: predicate = CmpInst::ICMP_SLT ; break;
		case TCLE: predicate = CmpInst::ICMP_SLE ; break;
		case TCGT: predicate = CmpInst::ICMP_SGT ; break;
		case TCGE: predicate = CmpInst::ICMP_SGE ; break;
		
		default: return NULL;
	}
	BasicBlock *bblock = context.currentBlock();
	return new ICmpInst::ICmpInst((*bblock), predicate, _lhs.codeGen(context), _rhs.codeGen(context), "");
}


Value* NAssignment::codeGen(CodeGenContext &context) {
	std::cout << "Creating assignment for " << _lhs._name << std::endl;
	if (context.locals().find(_lhs._name) == context.locals().end()) {
		std::cerr << "undeclared variable " << _lhs._name << std::endl;
		return NULL;
	}
	return new StoreInst(_rhs.codeGen(context), context.locals()[_lhs._name], false, context.currentBlock());
}


Value* NBlock::codeGen(CodeGenContext &context) {
	StatementList::const_iterator it;
	Value *last = NULL;
	for (it = _statements.begin(); it != _statements.end(); it++) {
		std::cout << "Generating code for " << typeid(**it).name() << std::endl;
		last = (**it).codeGen(context);
	}
	std::cout << "Creating block" << std::endl;
	return last;
}


Value* NExpressionStatement::codeGen(CodeGenContext &context) {
	std::cout << "Generating code for " << typeid(_expression).name() << std::endl;
	return _expression.codeGen(context);
}


Value* NVariableDeclaration::codeGen(CodeGenContext &context) {
	std::cout << "Creating variable declaration " << _type._name << " " << _id._name << std::endl;
	AllocaInst *alloc = new AllocaInst(typeOf(_type), _id._name.c_str(), context.currentBlock());
	context.locals()[_id._name] = alloc;
	if (_assignmentExpression != NULL) {
		NAssignment assn(_id, *_assignmentExpression);
		assn.codeGen(context);
	}
	return alloc;
}


Value* NFunctionDeclaration::codeGen(CodeGenContext &context) {
	vector<Type*> argTypes;
	VariableList::const_iterator it;
	for (it = _arguments.begin(); it != _arguments.end(); it++) {
		argTypes.push_back(typeOf((**it)._type));
	}
	FunctionType *ftype = FunctionType::get(typeOf(_type), makeArrayRef(argTypes), false);
	Function *function = Function::Create(ftype, GlobalValue::InternalLinkage, _id._name.c_str(), context.module);
	BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);
	
	context.pushBlock(bblock);
	
	for (it = _arguments.begin(); it != _arguments.end(); it++) {
		(**it).codeGen(context);
	}
	
	_block.codeGen(context);
	ReturnInst::Create(getGlobalContext(), bblock);
	
	context.popBlock();
	std::cout << "Creating function " << _id._name << std::endl;
	return function;
}


Value* NIfStatement::codeGen(CodeGenContext &context) {
	
}