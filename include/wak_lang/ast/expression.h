#ifndef _WAK_LANG_AST_EXPRESSION_H
#define _WAK_LANG_AST_EXPRESSION_H

/*
 * The idea here is to mimic the way LLVM reprents expressions
 * its sort of like SSA
 * because each expression represents a series of instructions that must be represented linearly,
 * I am representing them linearly instead of in a tree
 * this could pose some difficulties with order of operations
 * right now the plan is to do a second pass to correct the order after parsing is finished
 * for v0.0.1 all expressions are at most a single instruction so we dont have to worry about it
*/
typedef struct AST_Expression {
	// maybe include type?
	size_t index;
} AST_Expression;

#endif