#ifndef _WAK_LANG_PARSER_H
#define _WAK_LANG_PARSER_H

typedef struct
{
	
	const char* path;
	
	AST_Statement_Block *top_block;

} Parsed_Module;

#endif