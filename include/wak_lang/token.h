#ifndef _WAK_LANG_TOKEN_H
#define _WAK_LANG_TOKEN_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum Token_Value {
	TOKEN_EOF = 0,
	TOKEN_AT = '@',
	TOKEN_OPEN_BRACKET = '{',
	TOKEN_CLOSE_BRACKET = '}',

	TOKEN_OPEN_BRACE = '[',
	TOKEN_CLOSE_BRACE = ']',

	TOKEN_OPEN_PAREN = '(',
	TOKEN_CLOSE_PAREN = ')',

	TOKEN_COMMA = ',',
	TOKEN_DOT = '.',

	TOKEN_ASTERISK = '*',
	TOKEN_AMPERSAND = '&',
	TOKEN_PLUS = '+',
	TOKEN_MINUS = '-',

	TOKEN_COLON = ':',
	TOKEN_EQUALS = '=',
	TOKEN_SLASH = '/',
	TOKEN_BACKSLASH = '\\',
	TOKEN_SEMICOLON = ';',
	TOKEN_BREAK = '\n',
	TOKEN_QUOTE = '"',

	TOKEN_INDENT = 256,
	TOKEN_EMPTY,
	TOKEN_NUMBER,
	TOKEN_STRING,

	TOKEN_PLUSEQUALS,
	TOKEN_MINUSEQUALS,

	TOKEN_KEYWORD_IF,
	TOKEN_KEYWORD_ELSE,
	TOKEN_KEYWORD_THEN,
	TOKEN_KEYWORD_DEFER,
	TOKEN_KEYWORD_STRUCT,
	TOKEN_KEYWORD_TYPE,
	TOKEN_KEYWORD_UNINIT,
	TOKEN_KEYWORD_FUNC,
	TOKEN_KEYWORD_RETURN,
	TOKEN_KEYWORD_NULL,
	TOKEN_KEYWORD_NULLPTR,

	TOKEN_KEYWORD_TRUE,
	TOKEN_KEYWORD_FALSE,

	TOKEN_KEYWORD_I32,
	TOKEN_KEYWORD_F32,
	TOKEN_KEYWORD_BOOL,
	TOKEN_KEYWORD_VOID,

	TOKEN_ERROR,

} Token_Value;

typedef enum Token_Type
{
	TOKEN_TYPE_IDENTIFIER,
	TOKEN_TYPE_TOKEN,
	TOKEN_TYPE_LITERAL_STR,
	TOKEN_TYPE_LITERAL_CHAR,
	TOKEN_TYPE_LITERAL_BOOL,
	TOKEN_TYPE_LITERAL_NUM
} Token_Type;

typedef struct Token
{
	Token_Type type;
	union {
		Token_Value token;
		const char* identifier;
		const char* literal_str;
		bool literal_bool;
	};
} Token;

static inline Token make_token_token(Token_Value value) {
	return (Token){TOKEN_TYPE_TOKEN, .token=value};
}
/*
static inline Token make_token_literal(size_t index) {
	return (Token){TOKEN_TYPE_LITERAL, index};
}
*/

static inline bool is_token_token_val(Token tok, Token_Value value) {
	return tok.type == TOKEN_TYPE_TOKEN && tok.token == value;
}



typedef struct Token_Pos
{
	uint32_t line_number;
	uint32_t character_number;
	const char* start;
	const char* end;
} Token_Pos;

const char *token_value_to_name(Token_Value t);
const char *token_type_to_name(Token_Type t);


#endif