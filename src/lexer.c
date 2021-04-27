#include <wak_lang/lexer.h>
#include <wak_lang/util/assert.h>
#include <wak_lang/util/mem.h>
#include <wak_lang/util/const_str_map.h>


#include <stdbool.h>
#include <string.h>


typedef struct Lexer_Data {
	const char* start;
	const char* end;
	const char* read;
	const_str_map* map;
	size_t line_num, char_num;
	Token_Pos token_pos;

	vector_str* identifiers;
	vector_literal* literals;
} Lexer_Data;


char lexer_peek_char(Lexer_Data* lexer);
void lexer_pop_char(Lexer_Data* lexer);
char lexer_peek_char_n(Lexer_Data* lexer, size_t n);
void lexer_pop_char_n(Lexer_Data* lexer, size_t n);
void lexer_breakline(Lexer_Data* lexer);
Token lexer_append_literal_str(Lexer_Data* lexer, const char* start, size_t count, Token_Literal_Type type);
Token lexer_append_literal_bool(Lexer_Data* lexer, Token_Value val);
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
	lexer->token_pos.start = lexer->read;

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
	lexer.identifiers = vector_str_new();
	lexer.literals = vector_literal_new();
	
	vector_token* tokens = vector_token_new();
	vector_pos* positions = vector_pos_new();

	size_t i =0;
	do
	{
		token = lexer_next(&lexer);

		if(token.type != TOKEN_TYPE_TOKEN || token.token != TOKEN_EMPTY)
		{
			vector_token_append(tokens, token);
			lexer.token_pos.end = lexer.read;
			vector_pos_append(positions, lexer.token_pos);
			i++;
		}

	} while(token.type != TOKEN_TYPE_TOKEN || token.token != TOKEN_EOF);


	return token_module_init(start, end, tokens, positions, lexer.literals, lexer.identifiers);
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

	int tok = const_str_map_find(lexer->map, str);

	if (tok == -1) {
		vector_str_append(lexer->identifiers, str);
		return (Token){TOKEN_TYPE_IDENTIFIER, .identifier=str};
	} else {
		free(str);
		if (tok == TOKEN_KEYWORD_TRUE || tok == TOKEN_KEYWORD_FALSE)
			return lexer_append_literal_bool(lexer, tok);
		else
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
	
	return lexer_append_literal_str(lexer, start, count, LITERAL_TYPE_NUM);
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

	return lexer_append_literal_str(lexer, start, count, LITERAL_TYPE_STRING);
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

	return lexer_append_literal_str(lexer, start, count, LITERAL_TYPE_CHAR);
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
	lexer_pop_char(lexer);

	
	Token out = make_token_token(TOKEN_EMPTY);
	size_t depth = 1;
	do {
		char c = lexer_peek_char(lexer);
		char next = lexer_peek_char_n(lexer, 1);

		if (c == 0) return make_token_token(TOKEN_EOF);
		else if (c == '\n') lexer_breakline(lexer);
		else if (c == '/' && next == '*') {
			lexer_pop_char(lexer);
			depth ++;
		}
		else if (c == '*' && next == '/') {
			lexer_pop_char(lexer);
			depth--;
		}

		lexer_pop_char(lexer);
	} while (depth > 0);


	return out;
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

	if (c >= '0' && c <= '9') {
		return lexer_parse_num_literal(lexer);
	}

	return make_token_token(TOKEN_DOT);
}
Token lexer_append_literal_bool(Lexer_Data* lexer, Token_Value val) {
	wak_assert(val == TOKEN_KEYWORD_TRUE || val == TOKEN_KEYWORD_FALSE);
	bool b = val == TOKEN_KEYWORD_TRUE;

	Token_Literal literal = (Token_Literal){LITERAL_TYPE_BOOL, .v_bool=b};

	vector_literal_append(lexer->literals, literal);
	return (Token){TOKEN_TYPE_LITERAL, .literal=literal};
}

Token lexer_append_literal_str(Lexer_Data* lexer, const char* start, size_t count, Token_Literal_Type type) {
	char* dst = malloc_str(count);
	memcpy(dst, start, count);
	

	Token_Literal literal = (Token_Literal){type, .v_str=dst};

	vector_literal_append(lexer->literals, literal);

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
	lexer->char_num = 1;
	lexer->line_num++;
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


Lexer_Data lexer_init(const char* start, const char* end) {
	const_str_map* map = const_str_map_new(token_keywords, TOKEN_KEYWORD_COUNT);
	vector_str* identifiers = vector_str_new();
	vector_literal* literals = vector_literal_new();

	Lexer_Data lexer;
	lexer.start = start;
	lexer.read = start;
	lexer.end = end;
	lexer.map = map;
	lexer.identifiers = identifiers;
	lexer.literals = literals;
	lexer.line_num = 1;
	lexer.char_num = 1;

	return lexer;
}