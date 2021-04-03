#ifndef _WAK_LANG_TOKEN_H
#define _WAK_LANG_TOKEN_H
#include <stdint.h>

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

	TOKEN_ERROR,

} Token_Value;


typedef enum Token_Type
{
	TOKEN_TYPE_IDENTIFIER = 0,
	TOKEN_TYPE_LITERAL = 1,
	TOKEN_TYPE_TOKEN = 2
} Token_Type;

typedef struct Token
{
	Token_Type type;
	union {
		Token_Value value;
		uint64_t index;
	};
} Token;


typedef enum Token_Literal_Type
{
	LITERAL_TYPE_FLOAT,
	LITERAL_TYPE_UINT,
	LITERAL_TYPE_STRING,
	LITERAL_TYPE_CHAR
} Token_Literal_Type;

typedef struct Token_Literal
{
	Token_Literal_Type type;
	union {
		char char_val;
		char *string_val;
		unsigned long uint_val;
		double float_val;
	};
} Token_Literal;

typedef struct Token_Pos
{
	uint32_t line_number;
	uint32_t character_number;
} Token_Pos;

const char *token_value_to_name(Token_Value t);
const char *token_type_to_name(Token_Type t);


#endif