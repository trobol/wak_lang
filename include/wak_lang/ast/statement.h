#ifndef _WAK_LANG_AST_STATEMENT_H
#define _WAK_LANG_AST_STATEMENT_H

#include "statement_error.h"
#include "data_type.h"

typedef enum {
	AST_STATEMENT_TYPE_EMPTY,
	AST_STATEMENT_TYPE_ERROR,
	AST_STATEMENT_TYPE_RETURN,
	AST_STATEMENT_TYPE_ASSIGN,
	AST_STATEMENT_TYPE_DECLARE,
	AST_STATEMENT_TYPE_LITERAL,
	AST_STATEMENT_TYPE_VOID_CALL,
	AST_STATEMENT_TYPE_BLOCK_IF,
	AST_STATEMENT_TYPE_BLOCK_ELSE,
	AST_STATEMENT_TYPE_BLOCK_FUNCTION,
	AST_STATEMENT_TYPE_BLOCK_END,
	AST_STATEMENT_TYPE_EOF,
} AST_Statement_Type; // this naming is confusing should change it

/*
 * The idea here is to mimic the way LLVM reprents expressions
 * its sort of like SSA
 * because each expression represents a series of instructions that must be represented linearly,
 * I am representing them linearly instead of in a tree
 * this could pose some difficulties with order of operations
 * right now the plan is to do a second pass to correct the order after parsing is finished
 * for v0.0.1 all expressions are at most a single instruction so we dont have to worry about it
*/
// a "reference" to a statement
typedef struct Ref_AST_Statement { size_t index; } Ref_AST_Statement;

typedef struct AST_Statement_Call {

} AST_Statement_Call;

typedef struct AST_Statement_Literal {
	AST_Data_Type data_type;
	union {
		int32_t i32;
		float f32;
	};
} AST_Statement_Literal;


typedef struct AST_Statement_Return {

} AST_Statement_Return;


typedef struct AST_Statement_Assign {
	Ref_AST_Statement dest;
	Ref_AST_Statement value;
} AST_Statement_Assign;

typedef struct AST_Statement_Declare {
	const char* name;
	AST_Data_Type data_type;
} AST_Statement_Declare;


typedef struct AST_Statement_Block_If {

} AST_Statement_Block_If;


typedef struct AST_Statement_Block_Else {

} AST_Statement_Block_Else;


typedef struct AST_Statement_Block_End {

} AST_Statement_Block_End;


typedef struct AST_Statement_Block_Function {
	const char* name;
	AST_Data_Type ret_type;
} AST_Statement_Block_Function;




typedef struct AST_Statement {
	AST_Statement_Type type;
	union {
		AST_Statement_Error s_error;
		AST_Statement_Return s_return;
		AST_Statement_Assign s_assign;
		AST_Statement_Declare s_declare;
		AST_Statement_Block_If s_block_if;
		AST_Statement_Block_Else s_block_else;
		AST_Statement_Block_Function s_block_function;
		AST_Statement_Block_End s_block_end;
	};
} AST_Statement;

#define make_ast_error(code, msg) impl_make_ast_error(code, msg, __FILE__, __LINE__)

static inline AST_Statement impl_make_ast_error(AST_Error_Code code, const char* msg, const char* file, unsigned int line) {
	return (AST_Statement) {AST_STATEMENT_TYPE_ERROR, .s_error=((AST_Statement_Error){code, msg, file, line})};
}
#endif