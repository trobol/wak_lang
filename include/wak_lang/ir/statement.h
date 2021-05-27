#ifndef _WAK_LANG_IR_STATEMENT_H
#define _WAK_LANG_IR_STATEMENT_H

#include "value.h"

#define IR_STATEMENT_BLOCK_BIT (1 << 7)
#define IR_STATEMENT_BLOCK_MASK (IR_STATEMENT_BLOCK_BIT & 255)

typedef enum {
	IR_STATEMENT_TYPE_EMPTY,
	IR_STATEMENT_TYPE_RETURN,
	IR_STATEMENT_TYPE_ASSIGN,
	IR_STATEMENT_TYPE_DECLARE,
	IR_STATEMENT_TYPE_LITERAL,
	IR_STATEMENT_TYPE_VOID_CALL,
	IR_STATEMENT_TYPE_EOF,

	IR_STATEMENT_TYPE_BLOCK_IF = IR_STATEMENT_BLOCK_BIT,
	IR_STATEMENT_TYPE_BLOCK_ELSE,
	IR_STATEMENT_TYPE_BLOCK_FUNCTION,

	IR_STATEMENT_TYPE_BLOCK_END,
	
} IR_Statement_Type; // this naming is confusing should change it

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
typedef struct IR_Reference { size_t index; } IR_Reference;


typedef struct IR_Statement_Call {

} IR_Statement_Call;


typedef enum {
	IR_DATA_TYPE_UNKNOWN,
	IR_DATA_TYPE_VOID,
	IR_DATA_TYPE_I32,
	IR_DATA_TYPE_F32,
	IR_DATA_TYPE_BOOL
} IR_Data_Type;

typedef struct IR_Statement_Literal {
	IR_Data_Type data_type;
	union {
		int32_t i32;
		float f32;
	};
} IR_Statement_Literal;


typedef struct IR_Statement_Return {
	IR_Value value;
} IR_Statement_Return;


typedef struct IR_Statement_Assign {
	size_t dest_index;
	IR_Value value;
} IR_Statement_Assign;

typedef struct IR_Statement_Declare {
	const char* name;
	IR_Data_Type data_type;
} IR_Statement_Declare;


typedef struct IR_Statement_Block {

	size_t end;
} IR_Statement_Block;


typedef struct IR_Statement_Block_If {

} IR_Statement_Block_If;


typedef struct IR_Statement_Block_Else {
} IR_Statement_Block_Else;


typedef struct IR_Statement_Block_End {

} IR_Statement_Block_End;


typedef struct IR_Statement_Block_Function {
	const char* name;
	IR_Data_Type ret_type;
} IR_Statement_Block_Function;




typedef struct IR_Statement {
	IR_Statement_Type type;
	union {
		IR_Statement_Return s_return;
		IR_Statement_Assign s_assign;
		IR_Statement_Declare s_declare;
		IR_Statement_Literal s_literal;

		IR_Statement_Block_If s_block_if;
		IR_Statement_Block_Else s_block_else;
		IR_Statement_Block_Function s_block_function;
		IR_Statement_Block_End s_block_end;
		
	};
} IR_Statement;



static inline bool statement_is_block(IR_Statement statement) {
	return statement.type >= IR_STATEMENT_BLOCK_BIT;
}

#endif