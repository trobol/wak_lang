#include <wak_lang/lexer.h>
#include <wak_lang/util/assert.h>
#include <wak_lang/util/mem.h>
#include <wak_lang/util/kv_map.h>


#include <stdbool.h>
#include <string.h>


typedef struct Lexer_Data {
	const char* start;
	const char* end;
	const char* read;
	kv_map* map;
	size_t line_num, char_num;
	Token_Pos token_pos;
	Token_Module module;
} Lexer_Data;


char lexer_peek_char(Lexer_Data* lexer);
void lexer_pop_char(Lexer_Data* lexer);
char lexer_peek_char_n(Lexer_Data* lexer, size_t n);
void lexer_pop_char_n(Lexer_Data* lexer, size_t n);
void lexer_breakline(Lexer_Data* lexer);
Token lexer_append_literal(Lexer_Data* lexer, const char* start, size_t count, Token_Literal_Type type);
Token lexer_append_identifier(Lexer_Data* lexer, const char* str);

Token lexer_next(Lexer_Data* lexer);

Lexer_Data lexer_init(const char* start, const char* end);

Token lexer_parse_identifier(Lexer_Data* lexer);
Token lexer_parse_num_literal(Lexer_Data* lexer);
Token lexer_parse_string_literal(Lexer_Data* lexer);
Token lexer_parse_char_literal(Lexer_Data* lexer);
Token lexer_parse_slash(Lexer_Data* lexer);
Token lexer_parse_linebreak(Lexer_Data* lexer);
Token lexer_parse_dot(Lexer_Data* lexer);
Token lexer_parse_single_comment(Lexer_Data* lexer);
Token lexer_parse_multi_comment(Lexer_Data* lexer);

Token lexer_next(Lexer_Data* lexer) {

	char c;

	// skip spaces and tabs
	while( (c = lexer_peek_char(lexer), c == ' ' || c == '\t') ) {
		lexer_pop_char(lexer);
	}

	lexer->token_pos.line_number = lexer->line_num;
	lexer->token_pos.character_number = lexer->char_num;

	switch (c)
	{
		case 0:
			return make_token_token(TOKEN_EOF);
		case 'a' ... 'z':
		case 'A' ... 'Z':
		case '_':
			return lexer_parse_identifier(lexer);
		case '.':
			return lexer_parse_dot(lexer);
		case '0' ... '9':
			return lexer_parse_num_literal(lexer);
		case '"':
			return lexer_parse_string_literal(lexer);
		case '\'':
			return lexer_parse_char_literal(lexer);
		case '/':
			return lexer_parse_slash(lexer);
		case '\r':
		case '\n':
			return lexer_parse_linebreak(lexer);
		default:
			lexer_pop_char(lexer);
			return make_token_token(c);
	}
}



Token_Module lexetize(const char* start, const char* end) {
	Token token;
	Lexer_Data lexer = lexer_init(start, end);
	lexer.module = token_module_init();
	size_t i =0;
	do
	{
		token = lexer_next(&lexer);

		if(token.type != TOKEN_TYPE_TOKEN || token.token != TOKEN_EMPTY)
		{
			array_token_append(lexer.module.tokens, token);
			array_pos_append(lexer.module.positions, lexer.token_pos);
			i++;
		}

	} while(token.type != TOKEN_TYPE_TOKEN || token.token != TOKEN_EOF);

	return lexer.module;
}

bool is_identifier_character(char c)
{
	return  (c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			(c >= '0' && c <= '9') ||
			 c == '_';
}

Token lexer_parse_identifier(Lexer_Data* lexer) {
	char c = lexer_peek_char(lexer);

	wak_assert(is_identifier_character(c));

	const char* start = lexer->read;
	size_t count = 0;
	do {
		lexer_pop_char(lexer);
		c = lexer_peek_char(lexer);
		count++;
	} while(is_identifier_character(c));


	char* str = malloc_str(count);
	memcpy(str, start, count);

	int tok = kv_map_find(lexer->map, str);

	if (tok == -1) {
		array_str_append(lexer->module.identifers, str);
		return (Token){TOKEN_TYPE_IDENTIFIER, .identifier=str};
	} else {
		free(str);
		return make_token_token(tok);
	}
}


Token lexer_parse_num_literal(Lexer_Data* lexer) {
	const char* start = lexer->read;
	char c = lexer_peek_char(lexer);
	size_t count = 0;
	while (( c >= '0' && c <= '9' ) ||
			 c == '.' ||
			 c == 'x' ||
			 c == 'b') {
		lexer_pop_char(lexer);
		c = lexer_peek_char(lexer);
		count++;
	}
	
	return lexer_append_literal(lexer, start, count, LITERAL_TYPE_NUM);
}


Token lexer_parse_string_literal(Lexer_Data* lexer) {
	wak_assert(lexer_peek_char(lexer) == '"');

	const char* start = lexer->read;
	size_t count = 0;
	char c;
	do {
		lexer_pop_char(lexer);
		c = lexer_peek_char(lexer);
		count++;
	} while(c != '"' && c != 0 && c != '\n' && c != '\r');

	if (c == '"') {
		count++;
		lexer_pop_char(lexer);
	}

	return lexer_append_literal(lexer, start, count, LITERAL_TYPE_STRING);
}

Token lexer_parse_char_literal(Lexer_Data* lexer) {
	wak_assert(lexer_peek_char(lexer) == '\'');

	const char* start = lexer->read;
	size_t count = 0;
	char c;
	do {
		lexer_pop_char(lexer);
		c = lexer_peek_char(lexer);
		count++;
	} while(c != '\'' && c != 0 && c != '\n' && c != '\r');

	if (c == '\'') {
		count++;
		lexer_pop_char(lexer);
	}

	return lexer_append_literal(lexer, start, count, LITERAL_TYPE_CHAR);
}

Token lexer_parse_slash(Lexer_Data* lexer) {
	wak_assert(lexer_peek_char(lexer) == '/');

	lexer_pop_char(lexer);
	char c = lexer_peek_char(lexer);
	if (c == '*') return lexer_parse_multi_comment(lexer);
	if (c == '/') return lexer_parse_single_comment(lexer);

	return make_token_token(TOKEN_SLASH);
}

Token lexer_parse_single_comment(Lexer_Data* lexer) {
	wak_assert(lexer_peek_char(lexer) == '/');
	
	char c;
	do {
		lexer_pop_char(lexer);
		c = lexer_peek_char(lexer);
	} while (c != '\n' && c != '\r' && c != 0);

	return make_token_token(TOKEN_EMPTY);
}

Token lexer_parse_multi_comment(Lexer_Data* lexer) {
	wak_assert(lexer_peek_char(lexer) == '*');

	char c;
	do {
		lexer_pop_char(lexer);
		c = lexer_peek_char(lexer);

		if (c == 0) return make_token_token(TOKEN_EOF);

	} while (c != '\n' && c != '\r');


	lexer_breakline(lexer);
	return make_token_token(TOKEN_BREAK);
}	

Token lexer_parse_linebreak(Lexer_Data* lexer) {
	char c = lexer_peek_char(lexer);

	wak_assert(c == '\n' || c == '\r');
	
	if (c == '\r') {
		lexer_pop_char(lexer);
		c = lexer_peek_char(lexer);
	}
	if (c == '\n') {
		lexer_pop_char(lexer);
	}
	lexer_breakline(lexer);
	return make_token_token(TOKEN_BREAK);
}



Token lexer_parse_dot(Lexer_Data* lexer) {
	wak_assert(lexer_peek_char(lexer) == '.');

	char c = lexer_peek_char_n(lexer, 1);
	if (c == 0) return make_token_token(TOKEN_EOF);

	if (c >= '0' && c <= '9') {
		return lexer_parse_num_literal(lexer);
	}

	return make_token_token(TOKEN_DOT);
}

Token lexer_append_literal(Lexer_Data* lexer, const char* start, size_t count, Token_Literal_Type type) {
	char* dst = malloc_str(count);
	memcpy(dst, start, count);
	

	Token_Literal literal = (Token_Literal){type, dst};

	array_literal_append(lexer->module.literals, literal);

	return (Token){TOKEN_TYPE_LITERAL, .literal=literal};
}


char lexer_peek_char(Lexer_Data* lexer) {
	if (lexer->read < lexer->end)
		return *(lexer->read);
	return 0;
}

void lexer_pop_char(Lexer_Data* lexer) {
	lexer->read++;
	lexer->char_num++;
}

char lexer_peek_char_n(Lexer_Data* lexer, size_t n) {
	if (lexer->read+n < lexer->end)
		return *(lexer->read+n);
	return 0;
}

void lexer_pop_char_n(Lexer_Data* lexer, size_t n) {
	lexer->read+=n;
	lexer->char_num+=n;
}

void lexer_breakline(Lexer_Data* lexer) {
	lexer->char_num = 0;
	lexer->line_num++;
}

kv_pair token_keywords[] = {
	{"struct", TOKEN_KEYWORD_STRUCT},
	{"type", TOKEN_KEYWORD_TYPE},
	{"if", TOKEN_KEYWORD_IF},
	{"else", TOKEN_KEYWORD_ELSE},
	{"defer", TOKEN_KEYWORD_DEFER},
	{"func", TOKEN_KEYWORD_FUNC},
	{"uninit", TOKEN_KEYWORD_UNINIT},
	{"return", TOKEN_KEYWORD_RETURN},
	{"nullptr", TOKEN_KEYWORD_NULLPTR},
	{"null", TOKEN_KEYWORD_NULL}
};

#define TOKEN_KEYWORD_COUNT sizeof(token_keywords)/sizeof(kv_pair)


Lexer_Data lexer_init(const char* start, const char* end) {
	kv_map* map = kv_map_new(token_keywords, TOKEN_KEYWORD_COUNT);
	return (Lexer_Data){start, end, start, map, 0, 0, 0};
}