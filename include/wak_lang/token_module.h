#ifndef _WAK_LANG_TOKEN_MODULE_H
#define _WAK_LANG_TOKEN_MODULE_H
#include <wak_lang/util/vector.h>
#include <wak_lang/token.h>

DEFINE_VECTOR(vector_token, Token);
DEFINE_VECTOR(vector_pos, Token_Pos);
DEFINE_VECTOR(vector_literal, Token_Literal);
DEFINE_VECTOR(vector_str, const char*);

typedef struct Token_Module {
	vector_token* tokens;
	vector_pos* positions;
	vector_literal* literals;
	vector_str* identifers;
} Token_Module;

static inline Token_Module token_module_init() {
	return (Token_Module){
		vector_token_new(),
		vector_pos_new(),
		vector_literal_new(),
		vector_str_new()
	};
}


#endif