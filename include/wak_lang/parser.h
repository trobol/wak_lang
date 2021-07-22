#ifndef _WAK_LANG_PARSER_H
#define _WAK_LANG_PARSER_H

#include <wak_lang/ast/block.h>
#include <wak_lang/ir/statement.h>
#include <wak_lang/token_module.h>


DEFINE_VECTOR(vector_statement, IR_Statement);
DEFINE_VECTOR(vector_error, parse_err);
typedef struct
{
	
	const char* path;
	
	AST_Block *top_block;
	vector_str* str_table;

} Parsed_Module;

Parsed_Module parse(Token_Module module);

#endif