#ifndef _WAK_LANG_AST_ERROR_H
#define _WAK_LANG_AST_ERROR_H

typedef enum {
	AST_ERROR_NONE,
	AST_ERROR_UNSUPPORTED,
	AST_ERROR_UNIMPLEMENTED,
	AST_ERROR_NOT_FOUND,
	AST_ERROR_INVALID_SYNTAX,
} AST_Error_Code;

typedef struct AST_Error {
	AST_Error_Code code;
	const char* msg;
	const char* file;
	unsigned int line;
} AST_Error;



#endif