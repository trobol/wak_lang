#ifndef _WAK_LANG_PARSER_H
#define _WAK_LANG_PARSER_H

#include <wak_lang/ast/block.h>
#include <wak_lang/ast/statement.h>
#include <wak_lang/token_module.h>

typedef struct
{
	
	const char* path;
	
	AST_Block *top_block;

} Parsed_Module;

Parsed_Module parse(Token_Module module);

#endif