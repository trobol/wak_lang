#ifndef _WAK_LANG_AST_STATEMENT_H
#define _WAK_LANG_AST_STATEMENT_H

typedef enum {
	AST_STATEMENT_TYPE_ERROR,
	AST_STATEMENT_TYPE_RETURN,
	AST_STATEMENT_TYPE_ASSIGN,
	AST_STATEMENT_TYPE_DECLARE,
	AST_STATEMENT_TYPE_CALL,
	AST_STATEMENT_TYPE_BLOCK_IF_ELSE,
	AST_STATEMENT_TYPE_BLOCK_FUNCTION,
	AST_STATEMENT_TYPE_BLOCK_SWITCH
} AST_Statement_Type; // this naming is confusing should change it

typedef struct {
	AST_Statement_Type type;
	union {
		AST_Statement_Assign v_assign;
		AST_Statement_Error v_error;
		AST_Statement_Return v_return;
	};
} AST_Statement;

typedef struct {

} AST_Statement_Call;

typedef struct {

} AST_Statement_Error;

typedef struct {

} AST_Statement_Return;

typedef struct {

} AST_Statement_Assign;

#endif