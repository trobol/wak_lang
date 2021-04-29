#ifndef _WAK_LANG_TOKEN_MODULE_H
#define _WAK_LANG_TOKEN_MODULE_H
#include <wak_lang/util/vector.h>
#include <wak_lang/token.h>

DEFINE_VECTOR(vector_token, Token);
DEFINE_VECTOR(vector_pos, Token_Pos);
DEFINE_VECTOR(vector_str, const char*);
DEFINE_VECTOR(vector_uint32, uint32_t);

typedef struct Token_Module {
	const char* data_start;
	const char* data_end;
	vector_token* tokens;
	vector_pos* positions;
} Token_Module;

static inline Token_Module token_module_init(const char* start, const char* end, vector_token* tokens, vector_pos* positions) {

	return (Token_Module){
		.data_start=start,
		.data_end=end,
		.tokens= tokens,
		.positions= positions
	};
}


#endif