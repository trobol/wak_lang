#include <wak_lang/token.h>


const char *token_value_to_name(Token_Value t)
{
	static char buffer[2] = {0, 0};
	switch (t)
	{
		case 1 ... 9:
		case 11 ... 255:
			buffer[0] = t;
			return buffer;
		case TOKEN_INDENT:
			return "TOKEN_INDENT";
		case TOKEN_EMPTY:
			return "TOKEN_EMPTY";
		case TOKEN_NUMBER:
			return "TOKEN_NUMBER";
		case TOKEN_STRING:
			return "TOKEN_STRING";
		case TOKEN_PLUSEQUALS:
			return "TOKEN_PLUSEQUALS";
		case TOKEN_MINUSEQUALS:
			return "TOKEN_MINUSEQUALS";
		case TOKEN_KEYWORD_IF:
			return "TOKEN_KEYWORD_IF";
		case TOKEN_KEYWORD_ELSE:
			return "TOKEN_KEYWORD_ELSE";
		case TOKEN_KEYWORD_THEN:
			return "TOKEN_KEYWORD_THEN";
		case TOKEN_KEYWORD_DEFER:
			return "TOKEN_KEYWORD_DEFER";
		case TOKEN_KEYWORD_STRUCT:
			return "TOKEN_KEWORD_STRUCT";
		case TOKEN_KEYWORD_FUNC:
			return "TOKEN_KEYWORD_FUNC";
		case TOKEN_KEYWORD_TYPE:
			return "TOKEN_KEYWORD_TYPE";
		case TOKEN_KEYWORD_UNINIT:
			return "TOKEN_KEYWORD_UNINIT";
		case TOKEN_KEYWORD_RETURN:
			return "TOKEN_KEYWORD_RETURN";
		case TOKEN_KEYWORD_NULL:
			return "TOKEN_KEYWORD_NULL";
		case TOKEN_KEYWORD_NULLPTR:
			return "TOKEN_KEYWORD_NULLPTR";
			
		
		case TOKEN_ERROR:
			return "TOKEN_ERROR";
		case TOKEN_EOF:
			return "TOKEN_EOF";
		case TOKEN_BREAK:
			return "TOKEN_BREAK";
/*
		case TOKEN_AT:
			return "@";
		case TOKEN_OPEN_BRACKET:
			return "{";
		case TOKEN_CLOSE_BRACKET:
			return "}";
		case TOKEN_OPEN_BRACE:
			return "[";
		case TOKEN_CLOSE_BRACE:
			return "]";
		case TOKEN_OPEN_PAREN:
			return "(";
		case TOKEN_CLOSE_PAREN:
			return ")";
		case TOKEN_COMMA:
			return ",";
		case TOKEN_COLON:
			return ":";
		case TOKEN_EQUALS:
			return "=";

		case '-':
			return "-";
		case '+':
			return "+";
		case '/':
			return "/";
		case '*':
			return "*";

		case TOKEN_AMPERSAND:
			return "&";
		case TOKEN_BACKSLASH:
			return "\\";
		case TOKEN_DOT:
			return ".";
		case TOKEN_SEMICOLON:
			return ";";
		case TOKEN_QUOTE:
			return "\"";
*/

		case TOKEN_KEYWORD_F32:
			return "f32";
		case TOKEN_KEYWORD_I32:
			return "i32";
		case TOKEN_KEYWORD_BOOL:
			return "bool";
		case TOKEN_KEYWORD_VOID:
			return "void";
		

		default:
			return "UNKNOWN_TOKEN";
	}
}

const char *token_type_to_name(Token_Type t) {
	return "";
}
