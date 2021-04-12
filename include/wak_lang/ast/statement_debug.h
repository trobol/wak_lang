#ifndef _WAK_LANG_AST_STATEMENT_DEBUG_H
#define _WAK_LANG_AST_STATEMENT_DEBUG_H

#include "statement.h"

static inline const char* get_statement_name(AST_Statement statement) {
	switch (statement.type)
	{
	case AST_STATEMENT_TYPE_ERROR:
		return "ERROR";
	case AST_STATEMENT_TYPE_RETURN:
		return "RETURN";
	case AST_STATEMENT_TYPE_ASSIGN:
		return "ASSIGN";
	case AST_STATEMENT_TYPE_DECLARE:
		return "DECLARE";
	case AST_STATEMENT_TYPE_VOID_CALL:
		return "VOID_CALL";
	case AST_STATEMENT_TYPE_BLOCK_IF:
		return "BLOCK_IF";
	case AST_STATEMENT_TYPE_BLOCK_ELSE:
		return "BLOCK_ELSE";
	case AST_STATEMENT_TYPE_BLOCK_FUNCTION:
		return "BLOCK_FUNCTION";
	case AST_STATEMENT_TYPE_BLOCK_END:
		return "BLOCK_END";
	case AST_STATEMENT_TYPE_EOF:
		return "EOF";
	default:
		return "missing from get_statement_name";
	}
}


static inline const char* get_ast_error_name(AST_Error_Code code) {
	switch (code)
	{
	case AST_ERROR_UNSUPPORTED:
		return "UNSUPPORTED";
	case AST_ERROR_UNIMPLEMENTED:
		return "UNIMPLEMENTED";
	case AST_ERROR_NOT_FOUND:
		return "NOT_FOUND";
	case AST_ERROR_INVALID_SYNTAX:
		return "INVALID_SYNTAX";
	default:
		return "missing from get_ast_error_name";
	}
}

#endif