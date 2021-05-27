#ifndef _WAK_LANG_AST_VALUE_H
#define _WAK_LANG_AST_VALUE_H
#include <stddef.h>
#include <stdint.h>

typedef enum {
	AST_VALUE_TYPE_NULL,
	AST_VALUE_TYPE_INSTR_REF,
	AST_VALUE_TYPE_CONST_I32,
	AST_VALUE_TYPE_CONST_F32,
	AST_VALUE_TYPE_CONST_STR,
	AST_VALUE_TYPE_CONST_CHAR
} IR_Value_Type;

typedef struct IR_Value
{
	IR_Value_Type type;
	union
	{
		size_t u_index;
		size_t u_str_index;
		int32_t u_i32;
		float u_f32;
		char u_char;
	};
	
} IR_Value;



#endif