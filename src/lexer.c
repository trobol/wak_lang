#include <wak_lang/lexer.h>
#include <wak_lang/util/assert.h>
#include <wak_lang/util/mem.h>
#include <wak_lang/util/const_str_map.h>
#include <wak_lang/util/def.h>

#include <stdbool.h>
#include <string.h>


typedef struct wak_lex_state {
	const char* limit;

	const char* line_start;
	size_t line_num;
	Token_Pos token_pos;

	const_str_map* tok_map;

	Token* tok_array;
	Token_Pos* pos_array;
	uint32_t tok_count;
	uint32_t tok_capacity;

} wak_lex_state;

/*
only the hottest variables,
ideally should always remain in registers
*/
#define WAK_LEX_PARAMS \
	wak_lex_state* state, const char* ptr

#define WAK_LEX_ARGS \
	state, ptr


WAK_FORCEINLINE void lex_breakline(WAK_LEX_PARAMS);

const char* lexer_next(WAK_LEX_PARAMS);


wak_lex_state lex_state_init(const char* ptr, const char* limit);


const char* lex_append_tok_val(WAK_LEX_PARAMS, Token_Value v);
const char* lex_append_tok_substr(WAK_LEX_PARAMS, const char* start, const char* end, Token_Type type);
const char* lex_append_tok_literal_bool(WAK_LEX_PARAMS, bool token);

const char* lex_parse_eof(WAK_LEX_PARAMS);
const char* lex_parse_identifier(WAK_LEX_PARAMS);
const char* lex_parse_num_literal(WAK_LEX_PARAMS);
const char* lex_parse_string_literal(WAK_LEX_PARAMS);
const char* lex_parse_char_literal(WAK_LEX_PARAMS);
const char* lex_parse_slash(WAK_LEX_PARAMS);
const char* lex_parse_linebreak(WAK_LEX_PARAMS);
const char* lex_parse_dot(WAK_LEX_PARAMS);
const char* lex_parse_single_comment(WAK_LEX_PARAMS);
const char* lex_parse_multi_comment(WAK_LEX_PARAMS);

const char* lex_make_error(WAK_LEX_PARAMS);

WAK_NOINLINE
const char* lex_dispatch(WAK_LEX_PARAMS) {
	

	if (WAK_UNLIKELY(ptr >= state->limit)) {
		return ptr;
	}

	return lexer_next(WAK_LEX_ARGS);
}






Token_Module lexetize(const char* ptr, const char* limit) {
	wak_lex_state state_val = lex_state_init(ptr, limit);
	wak_lex_state* state = &state_val;

	ptr = lex_dispatch(WAK_LEX_ARGS);


	vector_token* tok_vec = vector_token_new_count(state->tok_count);
	uint64_t tok_size = state->tok_count * sizeof(Token);
	memcpy(tok_vec->start, state->tok_array, tok_size);

	vector_pos* pos_vec = vector_pos_new_count(state->tok_count);
	uint64_t pos_size = state->tok_count * sizeof(Token_Pos);
	memcpy(pos_vec->start, state->pos_array, pos_size);

	return token_module_init(ptr, limit, tok_vec, pos_vec);
}

WAK_FORCEINLINE
void lex_store_tokpos(WAK_LEX_PARAMS) {
	state->token_pos.line_number = state->line_num;
	state->token_pos.character_number = (uint32_t)(ptr-state->line_start);
	state->token_pos.start = ptr;
	// line start?
}



const char* lexer_next(WAK_LEX_PARAMS) {

	// skip spaces and tabs
	// this should not overflow because there should be an EOF at end
	while( *ptr == ' ' || *ptr == '\t') ptr++;
	
	// the begining of our token is its position
	lex_store_tokpos(WAK_LEX_ARGS);

	switch (*ptr)
	{
		case 0:
			return lex_parse_eof(WAK_LEX_ARGS);
		case 'a' ... 'z':
		case 'A' ... 'Z':
		case '_':
			return lex_parse_identifier(WAK_LEX_ARGS);
		case '.':
			return lex_parse_dot(WAK_LEX_ARGS);
		case '0' ... '9':
			return lex_parse_num_literal(WAK_LEX_ARGS);
		case '"':
			return lex_parse_string_literal(WAK_LEX_ARGS);
		case '\'':
			return lex_parse_char_literal(WAK_LEX_ARGS);
		case '/':
			return lex_parse_slash(WAK_LEX_ARGS);
		case '\r':
		case '\n':
			return lex_parse_linebreak(WAK_LEX_ARGS);
		default:
			ptr++;
			return lex_append_tok_val(WAK_LEX_ARGS, *(ptr-1));
	}
}




bool is_identifier_character(char c)
{
	return  (c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			(c >= '0' && c <= '9') ||
			 c == '_';
}

const char* lex_parse_identifier(WAK_LEX_PARAMS) {
	wak_assert(is_identifier_character(*ptr));

	const char* start = ptr;
	do {
		ptr++;
	} while(is_identifier_character(*ptr));

	uint32_t len = (uint32_t)(ptr - start);
	int tok = const_str_map_find_len(state->tok_map, start, len);

	if (tok == -1) {
		return lex_append_tok_substr(WAK_LEX_ARGS, start, ptr, TOKEN_TYPE_IDENTIFIER);
	} else {
		if (tok == TOKEN_KEYWORD_TRUE || tok == TOKEN_KEYWORD_FALSE)
			return lex_append_tok_literal_bool(WAK_LEX_ARGS, tok); //TODO: fix this, should pass bool val
		else
			return lex_append_tok_val(WAK_LEX_ARGS, tok);
	}
}


const char* lex_parse_num_literal(WAK_LEX_PARAMS) {
	const char* start = ptr;
	while (( *ptr >= '0' && *ptr <= '9' ) ||
			 *ptr == '.' ||
			 *ptr == 'x' ||
			 *ptr == 'b') {
		ptr++;
	}
	
	return lex_append_tok_substr(WAK_LEX_ARGS, start, ptr, TOKEN_TYPE_LITERAL_NUM);
}


const char* lex_parse_string_literal(WAK_LEX_PARAMS) {
	wak_assert(*ptr == '"');

	const char* start = ptr;
	do {
		ptr++;
	} while(*ptr != '"' && *ptr != 0 && *ptr != '\n' && *ptr != '\r');

	if (*ptr == '"') ptr++;

	return lex_append_tok_substr(WAK_LEX_ARGS, start, ptr, TOKEN_TYPE_LITERAL_STR);
}

const char* lex_parse_char_literal(WAK_LEX_PARAMS) {
	wak_assert(*ptr == '\'');

	const char* start = ptr;
	do {
		ptr++;
	} while(*ptr != '\'' && *ptr != 0 && *ptr != '\n' && *ptr != '\r');

	if (*ptr == '\'') ptr++;

	return lex_append_tok_substr(WAK_LEX_ARGS, start, ptr, TOKEN_TYPE_LITERAL_CHAR);
}

const char* lex_parse_slash(WAK_LEX_PARAMS) {
	wak_assert(*ptr == '/');

	ptr++;
	if ( *ptr == '*') return lex_parse_multi_comment(WAK_LEX_ARGS);
	if ( *ptr == '/') return lex_parse_single_comment(WAK_LEX_ARGS);

	return lex_append_tok_val(WAK_LEX_ARGS, TOKEN_SLASH);
}

const char* lex_parse_single_comment(WAK_LEX_PARAMS) {
	wak_assert(*ptr == '/');
	
	do {
		ptr++;
	} while (*ptr != '\n' && *ptr != '\r' && *ptr != 0);

	return lex_append_tok_val(WAK_LEX_ARGS, TOKEN_BREAK);
}

const char* lex_parse_eof(WAK_LEX_PARAMS) {
	wak_assert(*ptr == '\0');
	ptr++;
	return lex_append_tok_val(WAK_LEX_ARGS, TOKEN_EOF);
}


const char* lex_parse_multi_comment(WAK_LEX_PARAMS) {
	wak_assert(*ptr == '*');
	ptr++;

	uint32_t depth = 1;
	do {
		char c = *ptr;
		char next = *(ptr+1);

		if (c == 0) return lex_dispatch(WAK_LEX_ARGS);
		else if (c == '\n') lex_breakline(WAK_LEX_ARGS);
		// skip * (if comment start) or / (if end) because otherwise /*/ will trigger twice
		else if (c == '/' && next == '*') {
			ptr++;
			depth ++;
		}
		else if (c == '*' && next == '/') {
			ptr++;
			depth--;
		}

		ptr++;
	} while (depth > 0);


	return lex_dispatch(WAK_LEX_ARGS);
}	

const char* lex_parse_linebreak(WAK_LEX_PARAMS) {
	wak_assert(*ptr == '\n' || *ptr == '\r');

	if (*ptr == '\r') ptr++;
	if (*ptr == '\n') ptr++;

	lex_breakline(WAK_LEX_ARGS);
	return lex_append_tok_val(WAK_LEX_ARGS, TOKEN_BREAK);
}

void lex_breakline(WAK_LEX_PARAMS) {
	state->line_start = ptr;
	state->line_num++;
}

const char* lex_parse_dot(WAK_LEX_PARAMS) {
	wak_assert(*ptr == '.');

	char c = *(ptr+1);
	if (c >= '0' && c <= '9') 
		return lex_parse_num_literal(WAK_LEX_ARGS);

	return lex_append_tok_val(WAK_LEX_ARGS, TOKEN_DOT);
}

void lex_array_grow(WAK_LEX_PARAMS) {
	Token* new_tok = alloc_array(state->tok_capacity * 2, Token);
	uint64_t tok_size = state->tok_capacity * sizeof(Token);
	memcpy(new_tok, state->tok_array, tok_size);
	free(state->tok_array);
	state->tok_array = new_tok;

	Token_Pos* new_pos = alloc_array(state->tok_capacity * 2, Token_Pos);
	uint64_t pos_size = state->tok_capacity * sizeof(Token_Pos);
	memcpy(new_pos, state->pos_array, pos_size);
	free(state->pos_array);
	state->pos_array = new_pos;

	state->tok_capacity *= 2;
}

WAK_FORCEINLINE const char* lex_append_tok(WAK_LEX_PARAMS, Token tok) {

	if (WAK_UNLIKELY(state->tok_count >= state->tok_capacity))
		lex_array_grow(WAK_LEX_ARGS);
	state->token_pos.end = ptr;
	state->pos_array[state->tok_count] = state->token_pos;
	state->tok_array[state->tok_count] = tok;

	state->tok_count++;

	return lex_dispatch(WAK_LEX_ARGS);
}

const char* lex_append_tok_val(WAK_LEX_PARAMS, Token_Value v) {
	return lex_append_tok(WAK_LEX_ARGS, (Token){ .type=TOKEN_TYPE_TOKEN, .token=v});
}

// copy substring from buffer
const char* lex_append_tok_substr(WAK_LEX_PARAMS, const char* start, const char* end, Token_Type type) {
	uint32_t len = (uint32_t)(end - start);
	char* dst = malloc_str(len);
	memcpy(dst, start, len);
	return lex_append_tok(WAK_LEX_ARGS, (Token){ .type=type, .identifier=dst});
}

const char* lex_append_tok_literal_bool(WAK_LEX_PARAMS, bool token) {
	return lex_append_tok(WAK_LEX_ARGS, (Token){ .type=TOKEN_TYPE_LITERAL_BOOL, .literal_bool=token});
}

const_str_map_pair token_keywords[] = {
	{"struct", TOKEN_KEYWORD_STRUCT},
	{"type", TOKEN_KEYWORD_TYPE},
	{"if", TOKEN_KEYWORD_IF},
	{"else", TOKEN_KEYWORD_ELSE},
	{"defer", TOKEN_KEYWORD_DEFER},
	{"func", TOKEN_KEYWORD_FUNC},
	{"uninit", TOKEN_KEYWORD_UNINIT},
	{"return", TOKEN_KEYWORD_RETURN},
	{"nullptr", TOKEN_KEYWORD_NULLPTR},
	{"null", TOKEN_KEYWORD_NULL},
	{"i32", TOKEN_KEYWORD_I32},
	{"f32", TOKEN_KEYWORD_F32},
	{"bool", TOKEN_KEYWORD_BOOL},
};

#define TOKEN_KEYWORD_COUNT sizeof(token_keywords)/sizeof(token_keywords[0])

WAK_FORCEINLINE
wak_lex_state lex_state_init(const char* ptr, const char* limit) {
	return (wak_lex_state) {
		.limit=limit,
		.tok_map=const_str_map_new(token_keywords, TOKEN_KEYWORD_COUNT),
		.line_num=1,
		.line_start=ptr,
		.token_pos=(Token_Pos){},
		.tok_array=alloc_array(10, Token),
		.pos_array=alloc_array(10, Token_Pos),
		.tok_count=0,
		.tok_capacity=10
	};
}