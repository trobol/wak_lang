#ifndef _WAK_LANG_IR_STATEMENT_DEBUG_H
#define _WAK_LANG_IR_STATEMENT_DEBUG_H

#include <wak_lang/ir/statement.h>

static inline const char* get_statement_name(IR_Statement statement) {
	switch (statement.type)
	{
	case IR_STATEMENT_TYPE_EMPTY:
		return "EMPTY";
	case IR_STATEMENT_TYPE_RETURN:
		return "RETURN";
	case IR_STATEMENT_TYPE_ASSIGN:
		return "ASSIGN";
	case IR_STATEMENT_TYPE_DECLARE:
		return "DECLARE";
	case IR_STATEMENT_TYPE_VOID_CALL:
		return "VOID_CALL";
	case IR_STATEMENT_TYPE_BLOCK_IF:
		return "BLOCK_IF";
	case IR_STATEMENT_TYPE_BLOCK_ELSE:
		return "BLOCK_ELSE";
	case IR_STATEMENT_TYPE_BLOCK_FUNCTION:
		return "BLOCK_FUNCTION";
	case IR_STATEMENT_TYPE_BLOCK_END:
		return "BLOCK_END";
	case IR_STATEMENT_TYPE_EOF:
		return "EOF";
	default:
		return "missing from get_statement_name";
	}
}


#endif