#ifndef _WAK_LANG_AST_BLOCK_H
#define _WAK_LANG_AST_BLOCK_H
#include <wak_lang/util/vector.h>
#include <wak_lang/token.h>

#include "statement.h"
#include "variable.h"

DEFINE_VECTOR(vector_var, AST_Variable);

typedef struct {
	vector_var* variables;
	Token_Pos start, end;
} AST_Block;


#endif