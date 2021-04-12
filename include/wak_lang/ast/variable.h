#ifndef _WAK_LANG_AST_VARIABLE_H
#define _WAK_LANG_AST_VARIABLE_H
#include "data_type.h"

typedef struct {
	const char* name;
	AST_Data_Type type;
} AST_Variable;

#endif