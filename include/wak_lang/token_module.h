#ifndef _WAK_LANG_TOKEN_MODULE_H
#define _WAK_LANG_TOKEN_MODULE_H
#include <wak_lang/util/array.h>
#include <wak_lang/token.h>

DEFINE_ARRAY(array_token, Token);
DEFINE_ARRAY(array_pos, Token_Pos);
DEFINE_ARRAY(array_literal, Token_Literal);
DEFINE_ARRAY(array_str, const char*);

typedef struct Token_Module {
	array_token* tokens;
	array_pos* positions;
	array_literal* literals;
	array_str* identifers;
} Token_Module;

static inline Token_Module token_module_init() {
	return (Token_Module){
		array_token_new(),
		array_pos_new(),
		array_literal_new(),
		array_str_new()
	};
}


#endif