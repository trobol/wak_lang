#ifndef _WAK_LANG_AST_STATEMENT_H
#define _WAK_LANG_AST_STATEMENT_H

#include "error.h"
#include "data_type.h"
#include "value.h"

#define AST_STATEMENT_BLOCK_BIT (1 << 7)
#define AST_STATEMENT_BLOCK_MASK (AST_STATEMENT_BLOCK_BIT & 255)

typedef enum {
	AST_STATEMENT_TYPE_EMPTY,
	AST_STATEMENT_TYPE_RETURN,
	AST_STATEMENT_TYPE_ASSIGN,
	AST_STATEMENT_TYPE_DECLARE,
	AST_STATEMENT_TYPE_LITERAL,
	AST_STATEMENT_TYPE_VOID_CALL,
	AST_STATEMENT_TYPE_EOF,

	AST_STATEMENT_TYPE_BLOCK_IF = AST_STATEMENT_BLOCK_BIT,
	AST_STATEMENT_TYPE_BLOCK_ELSE,
	AST_STATEMENT_TYPE_BLOCK_FUNCTION,
	AST_STATEMENT_TYPE_BLOCK_END,
	
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
	AST_Value value;
} AST_Statement_Return;


typedef struct AST_Statement_Assign {
	size_t dest_index;
	AST_Value value;
} AST_Statement_Assign;

typedef struct AST_Statement_Declare {
	const char* name;
	AST_Data_Type data_type;
} AST_Statement_Declare;


typedef struct AST_Statement_Block {

	size_t end;
} AST_Statement_Block;


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
		AST_Statement_Return s_return;
		AST_Statement_Assign s_assign;
		AST_Statement_Declare s_declare;
		struct {
			AST_Statement_Block s_block;
			union {
				AST_Statement_Block_If s_block_if;
				AST_Statement_Block_Else s_block_else;
				AST_Statement_Block_Function s_block_function;
				AST_Statement_Block_End s_block_end;
			};
		};
	};
} AST_Statement;



static inline bool statement_is_block(AST_Statement statement) {
	return statement.type >= AST_STATEMENT_BLOCK_BIT;
}

#endif