#ifndef _WAK_LANG_TOKEN_MODULE_H
#define _WAK_LANG_TOKEN_MODULE_H
#include <wak_lang/util/vector.h>
#include <wak_lang/token.h>

DEFINE_VECTOR(vector_token, Token);
DEFINE_VECTOR(vector_pos, Token_Pos);
DEFINE_VECTOR(vector_literal, Token_Literal);
DEFINE_VECTOR(vector_str, const char*);
DEFINE_VECTOR(vector_uint32, uint32_t);

typedef struct Token_Module {
	const char* data_start;
	const char* data_end;
	vector_token* tokens;
	vector_pos* positions;
	vector_literal* literals;
	vector_str* identifers;
} Token_Module;

static inline Token_Module token_module_init(const char* start, const char* end, vector_token* tokens, vector_pos* positions, vector_literal* literals, vector_str* identifers) {

	return (Token_Module){
		start,
		end,
		tokens,
		positions,
		literals,
		identifers
	};
}


#endif