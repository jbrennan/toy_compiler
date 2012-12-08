#include <iostream>
#include <vector>
#include <llvm/Value.h>

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;

// Make some list types for our different nodes
typedef std::vector<NStatement *> StatementList;
typedef std::vector<NExpression *> ExpressionList;
typedef std::vector<NVariableDeclaration *> VariableList;


// The root node class
class Node {
public:
	virtual ~Node() {}
	virtual llvm::Value* codeGen(CodeGenContext& context) {}
};


class NExpression : public Node {};
class NStatement : public Node {};

class NInteger : public NExpression {
public:
	long long _value; // the payload for this int
	NInteger(long long value) : _value(value) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NDouble : public NExpression {
public:
	double _value; // the payload for this double
	NDouble(double value) : _value(value) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};


class NIdentifier : public NExpression {
public:
	std::string _name;
	NIdentifier(const std::string& name) : _name(name) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};


class NMethodCall : public NExpression {
public:
	const NIdentifier& _id;
	ExpressionList _arguments;
	NMethodCall(const NIdentifier& id, ExpressionList& arguments) : _id(id), _arguments(arguments) {}
	NMethodCall(const NIdentifier& id) : _id(id) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBinaryOperator : public NExpression {
public:
	int _op;
	NExpression& _lhs;
	NExpression& _rhs;
	NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) : _lhs(lhs), _rhs(rhs), _op(op) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NAssignment : public NExpression {
public:
	NIdentifier& _lhs;
	NExpression& _rhs;
	NAssignment(NIdentifier& lhs, NExpression& rhs) : _lhs(lhs), _rhs(rhs) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBlock : public NExpression {
public:
	StatementList _statements;
	NBlock() {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};


class NExpressionStatement : public NStatement {
public:
	NExpression& _expression;
	NExpressionStatement(NExpression& expression) : _expression(expression) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NVariableDeclaration : public NStatement {
public:
	const NIdentifier& _type;
	NIdentifier& _id;
	NExpression *_assignmentExpression;
	
	NVariableDeclaration(const NIdentifier& type, NIdentifier& id) : _type(type), _id(id) {}
	NVariableDeclaration(const NIdentifier& type, NIdentifier& id, NExpression *assignmentExpression) : _type(type), _id(id), _assignmentExpression(assignmentExpression) {}
	
	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NFunctionDeclaration : public NStatement {
public:
	const NIdentifier& _type;
	const NIdentifier& _id;
	VariableList _arguments;
	NBlock& _block;
	
	NFunctionDeclaration(const NIdentifier& type, const NIdentifier& id, const VariableList& arguments, NBlock& block) : _type(type), _id(id), _arguments(arguments), _block(block) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};