#include <iostream>
#include <llvm/Support/TargetSelect.h>
#include "codegen.h"
#include "node.h"

using namespace std;

extern NBlock *programBlock;
extern int yyparse();

int main(int argc, char** argv) {
	yyparse();
	std::cout << programBlock << std::endl;
	
	// see: http://comments.gmane.org/gmane.comp.compilers.llvm.devel/33877
	InitializeNativeTarget();
	CodeGenContext context;
	context.generateCode(*programBlock);
	context.runCode();
	
	return 0;
}