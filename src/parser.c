#include <wak_lang/parser.h>
#include <wak_lang/token_module.h>
#include <wak_lang/util/vector.h>
#include <wak_lang/util/assert.h>
#include <wak_lang/ast/expression.h>
#include <wak_lang/ast/statement_debug.h>


#include <string.h>

typedef struct {

} wak_parse_data;


DEFINE_VECTOR(vector_statement, AST_Statement);
DEFINE_VECTOR(vector_error, AST_Error);

typedef struct {
	Token_Module tok_mod;
	size_t tok_index;
	vector_statement* statements;

	vector_error* errors;
	bool fatal_error;
	bool statement_error;
} Parser;

Token parser_peek_token(Parser* parser);
Token parser_peek_token_n(Parser* parser, size_t i);
Token parser_peek_token_skip_break(Parser* parser);

void parser_pop_token(Parser* parser);
void parser_pop_token_n(Parser* parser, size_t i);

AST_Statement parser_next_statement(Parser* parser);
AST_Statement parser_func_decl(Parser* parser);
AST_Statement parser_var_decl(Parser* parser);
AST_Statement parser_return(Parser* parser);
AST_Statement parser_void_call(Parser* parser);
AST_Statement parser_eof(Parser* parser);
AST_Statement parser_assign(Parser* parser);
AST_Statement parser_end_block(Parser* parser);

AST_Statement parser_struct(Parser* parser);
AST_Statement parser_type_decl(Parser* parser);

AST_Data_Type parser_data_type(Parser* parser);

AST_Statement parser_token(Parser* parser);
AST_Statement parser_identifier(Parser* parser);


void parser_append(Parser* parser, AST_Statement statement) {
	vector_statement_append(parser->statements, statement);
}

AST_Value parser_expr(Parser* parser);
AST_Value parser_expr_literal(Parser* parser);
AST_Value parser_expr_iden(Parser* parser);
AST_Value parser_expr_call(Parser* parser);
AST_Value parser_expr_var(Parser* parser);

void parser_print_error(Parser* parser, AST_Error error);

void parser_emit_error_impl(Parser* parser, AST_Error_Code code, const char* msg, const char* file, unsigned int line);

#define parser_emit_error(parser, code, msg) parser_emit_error_impl(parser, code, msg, __FILE__, __LINE__)

Parsed_Module parse(Token_Module module) {
	printf("\n\n START: %zu \n", vector_count((vector*)module.tokens));
	Parser parser = (Parser){ .tok_mod=module, .tok_index=0, .statements=vector_statement_new(), .errors=vector_error_new()};

	AST_Statement statement;
	do
	{
		size_t tok_index = parser.tok_index;
		statement = parser_next_statement(&parser);

		
		
		if (parser.fatal_error) {
			printf("encountered fatal error");
			return (Parsed_Module) {};
		}
		printf("%s\n", get_statement_name(statement));
		if (parser.statement_error) {
			return (Parsed_Module) {};
		} else if (statement.type != AST_STATEMENT_TYPE_EMPTY) {
			parser_append(&parser, statement);
		}
		// TODO: add positions

		wak_assert_msg(parser.tok_index != tok_index, "parser did not progress");
		

	} while(statement.type != AST_STATEMENT_TYPE_EOF);

	return (Parsed_Module) {};
}

AST_Statement parser_next_statement(Parser* parser) {
	Token tok = parser_peek_token(parser);
	AST_Statement statement;
	switch (tok.type)
	{
	case TOKEN_TYPE_TOKEN:
		statement = parser_token(parser);
		break;
	case TOKEN_TYPE_IDENTIFIER:
		statement = parser_identifier(parser);
		break;
	default:
		parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "cannot have literal by itself");
		return (AST_Statement){};
	}
	
	
	if (statement.type == AST_STATEMENT_TYPE_EMPTY || statement_is_block(statement))
		return statement;

	Token line_end = parser_peek_token(parser);
	if (line_end.type == TOKEN_TYPE_TOKEN && line_end.token == TOKEN_SEMICOLON)
		parser_pop_token(parser);
	else
		parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "expected a semi colon");

	return statement;
}

AST_Statement parser_token(Parser* parser) {
	Token tok = parser_peek_token(parser);
	wak_assert(tok.type == TOKEN_TYPE_TOKEN);
	
	switch (tok.token)
	{
		case TOKEN_KEYWORD_FUNC:
			return parser_func_decl(parser);
		case TOKEN_KEYWORD_TYPE:
			return parser_type_decl(parser);
		case '}':
			return parser_end_block(parser);
		case TOKEN_KEYWORD_RETURN:
			return parser_return(parser);
		case TOKEN_EOF:
			return parser_eof(parser);
		case TOKEN_BREAK:
			parser_pop_token(parser);
			return (AST_Statement){AST_STATEMENT_TYPE_EMPTY};
		default:
			parser_emit_error(parser, AST_ERROR_UNSUPPORTED, "unknown token");
			return (AST_Statement){};
	}
}
AST_Statement parser_identifier(Parser* parser) {
	Token tok = parser_peek_token(parser);
	wak_assert_msg(tok.type == TOKEN_TYPE_IDENTIFIER, "expected token of type identifier");
	Token next_tok = parser_peek_token_n(parser, 1);
	if (next_tok.type != TOKEN_TYPE_TOKEN) { 
		parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "invalid operation on token");
		return (AST_Statement){};
	}
	
	switch (next_tok.token)
	{
		case '(':
			return parser_void_call(parser);
		case ':':
			return parser_var_decl(parser);
		case '=':
			return parser_assign(parser);
		case '.':
			// TODO handle dot operator
			parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "dot operator");
			return (AST_Statement){};
		case '{':
			// TODO object construction
			parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "obj constr");
			return (AST_Statement){};
		case '[':
			// TODO array access
			parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "array operator");
			return (AST_Statement){};
		case '*':
		case '/':
		case '+':
		case '-':
			// TODO math operation
			// assignment (+=...)
			parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "math operation");
			return (AST_Statement){};
		default:
			// TODO syntax error
			parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "unknown operation");
			return (AST_Statement){};
	}

}

AST_Value parser_expr(Parser* parser) {
	Token tok = parser_peek_token(parser);
	switch (tok.type)
	{
	case TOKEN_TYPE_LITERAL:
		return parser_expr_literal(parser);
	case TOKEN_TYPE_IDENTIFIER:
		return parser_expr_iden(parser);
	default:
		return (AST_Value){0}; // TODO: error
	}
}

AST_Value parser_expr_literal_decimal(Parser* parser) {
	Token tok = parser_peek_token(parser);
	wak_assert(tok.type == TOKEN_TYPE_LITERAL && tok.literal.type == LITERAL_TYPE_NUM);

	const char* str = tok.literal.v_str;
	size_t str_len = strlen(str);

	parser_pop_token(parser);
	unsigned int int_dig = 0;
	unsigned int frac_dig = 0;
	unsigned long int_val = 0;
	unsigned long frac_val = 0;
	// decimal or float
	for (size_t i = 0; i < str_len; i++) {
		char c = str[i];
		uint8_t digit_val = c - '0';

		switch (c)
		{
		case '0' ... '9':
			if (frac_dig > 0) {
				frac_val = (frac_val * frac_dig) + digit_val;
				frac_dig *= 10;
			} else {
				int_val = (int_val * int_dig) + digit_val;
				int_dig *= 10;
			}
			break;
		case '.':
			if (frac_dig == 0) frac_dig = 1;
			else {
				parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "multiple decimal points");
				return (AST_Value){0};
			}
			break;
		default:
			parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "invalid character");
			return (AST_Value){0};
		}
	}

	if ( frac_dig == 0) {
		return (AST_Value){.type=AST_VALUE_TYPE_CONST_I32, .u_i32=int_val};
	} else {
		double f = int_val;
		f += (float)frac_val / (float)frac_dig;
		return (AST_Value){.type=AST_VALUE_TYPE_CONST_F32, .u_f32=f};
	}
}

AST_Value parser_expr_literal_hex(Parser* parser) {
	parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "hex literal");
	return (AST_Value){0}; 
}

AST_Value parser_expr_literal_binary(Parser* parser) {
	parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "binary literal");
	return (AST_Value){0}; 
}


AST_Value parser_expr_literal_num(Parser* parser) {
	Token tok = parser_peek_token(parser);
	wak_assert(tok.type == TOKEN_TYPE_LITERAL && tok.literal.type == LITERAL_TYPE_NUM);

	const char* str = tok.literal.v_str;
	size_t str_len = strlen(str);

	if (str[0] == '0' && str_len > 1) {
		if (str[1] == 'x') return parser_expr_literal_hex(parser);
		if (str[1] == 'b') return parser_expr_literal_binary(parser);
	}
	

	return parser_expr_literal_decimal(parser);
}

AST_Value parser_expr_literal(Parser* parser) {
	Token tok = parser_peek_token(parser);
	wak_assert_msg( tok.type == TOKEN_TYPE_LITERAL, "expected a token of type literal");
	
	switch (tok.literal.type)
	{
	case LITERAL_TYPE_NUM:
		return parser_expr_literal_num(parser);
	default:
		break;
	}


	parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "literal type");
	return (AST_Value){0}; // TODO: error
}



AST_Value parser_expr_iden(Parser* parser) {
	Token tok = parser_peek_token(parser);
	wak_assert( tok.type == TOKEN_TYPE_IDENTIFIER);

	Token next_tok = parser_peek_token_n(parser, 1);
	if (next_tok.type == TOKEN_TYPE_TOKEN && next_tok.token == TOKEN_OPEN_PAREN)
		return parser_expr_call(parser);
	
	if (next_tok.type == TOKEN_TYPE_TOKEN && next_tok.token == TOKEN_SEMICOLON)
		return parser_expr_var(parser);

	parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "identifier expression");
	return (AST_Value){0}; // TODO: error
}

// TODO: fix how this searches
//		 this also wont handle constants
AST_Value parser_expr_var(Parser* parser) {
	Token tok = parser_peek_token(parser);
	wak_assert_msg( tok.type == TOKEN_TYPE_IDENTIFIER, "expected a token of type identifier");

	const char* name = tok.identifier;
	size_t size = vector_count((vector*)parser->statements);
	// search backwards till we find a match
	for(size_t i = size; i > 0; i--) {
		AST_Statement* itr = parser->statements->start + i;
		if (itr->type != AST_STATEMENT_TYPE_DECLARE) continue;

		int cmp = strcmp(name, itr->s_declare.name);

		if (cmp != 0) continue;

		parser_pop_token(parser);
		return (AST_Value){.type=AST_VALUE_TYPE_INSTR_REF, .u_index=i};
	}

	printf("could not find variable: %s", name);
	return (AST_Value){0}; // TODO: error
}

AST_Value parser_expr_call(Parser* parser) {
	Token tok = parser_peek_token(parser);
	wak_assert_msg( tok.type == TOKEN_TYPE_IDENTIFIER, "expected a token of type identifier");
	
	return (AST_Value){0}; // TODO: error
}

AST_Statement parser_return(Parser* parser) {
	Token tok = parser_peek_token(parser);
	wak_assert(tok.type == TOKEN_TYPE_TOKEN && tok.token == TOKEN_KEYWORD_RETURN);

	parser_pop_token(parser);
	AST_Value value = parser_expr(parser);

	if (value.type == AST_VALUE_TYPE_NULL)
		return (AST_Statement){};

	AST_Statement_Return ret = (AST_Statement_Return) { .value = value };
	return (AST_Statement){AST_STATEMENT_TYPE_RETURN, .s_return=ret};
}

AST_Statement parser_end_block(Parser* parser) {
	
	// IMPROVE: maybe keep a "stack" of blocks

	size_t size = vector_count((vector*)parser->statements);
	// search backwards for a block
	for(size_t i = size; i > 0; i--) {
		//AST_Statement* itr = parser->statements->start + i;
		//if (!statement_is_block(*itr) || itr->) continue;


		parser_pop_token(parser);
		return (AST_Statement){.type=AST_STATEMENT_TYPE_BLOCK_END};
	}

	parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "unexpected '}'");
	return (AST_Statement){};
}

AST_Statement parser_find_variable(Parser* parser) {
	parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "parser_find_variable");
	return (AST_Statement){};
}


AST_Statement parser_var_decl(Parser* parser) {
	Token name_tok = parser_peek_token(parser);
	wak_assert(name_tok.type == TOKEN_TYPE_IDENTIFIER);
	// TODO: check for repeat declarations
	const char* name = name_tok.identifier;
	parser_pop_token(parser);

	Token colon_tok = parser_peek_token(parser);
	wak_assert(colon_tok.type == TOKEN_TYPE_TOKEN && colon_tok.token == TOKEN_COLON);
	parser_pop_token(parser);

	Token type_tok = parser_peek_token(parser);
	if (type_tok.type != TOKEN_TYPE_TOKEN) {
		parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "expected token");
		return (AST_Statement){};
	}
	if (type_tok.token == TOKEN_EQUALS) {
		parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "type inference");
		return (AST_Statement){};
	}
	AST_Data_Type data_type = parser_data_type(parser);
	if (data_type == AST_DATA_TYPE_UNKNOWN) {
		parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "not a type");	
		return (AST_Statement){};
	}
	parser_pop_token(parser);

	AST_Statement_Declare decl;
	decl.name = name;
	decl.data_type = data_type;

	AST_Statement decl_statement = (AST_Statement){AST_STATEMENT_TYPE_DECLARE, .s_declare=decl};

	Token next = parser_peek_token(parser);
	if (next.type == TOKEN_TYPE_TOKEN && next.token == TOKEN_SEMICOLON) {
		parser_pop_token(parser);
		return decl_statement;
	}
	
	if (next.type != TOKEN_TYPE_TOKEN || next.token != TOKEN_EQUALS) {
		parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "expected ';' or '='");
		return (AST_Statement){};
	}
	parser_pop_token(parser);
	size_t index = vector_count((vector*)parser->statements);
	parser_append(parser, decl_statement);

	AST_Value value = parser_expr(parser);

	if (value.type == AST_VALUE_TYPE_NULL)
		return (AST_Statement){};

	AST_Statement_Assign assign = (AST_Statement_Assign) { .dest_index = index, .value = value };

	return (AST_Statement){AST_STATEMENT_TYPE_ASSIGN, .s_assign=assign};
}


AST_Statement parser_func_decl(Parser* parser) {
	{
		Token func_tok = parser_peek_token(parser);
		wak_assert_msg(func_tok.type == TOKEN_TYPE_TOKEN && func_tok.token == TOKEN_KEYWORD_FUNC, "expected tok: func");
		parser_pop_token(parser);
	}

	Token name_tok = parser_peek_token(parser);
	if (name_tok.type != TOKEN_TYPE_IDENTIFIER) {
		parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "missing function name");
		return (AST_Statement){};
	}
	
	const char* name = name_tok.identifier;
	parser_pop_token(parser);

	{
		Token open_paren = parser_peek_token(parser);
		if (open_paren.type != TOKEN_TYPE_TOKEN || open_paren.token != TOKEN_OPEN_PAREN) {
			parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "missing '(' after func name");
			return (AST_Statement){};
		}
		parser_pop_token(parser);

		Token close_paren = parser_peek_token(parser);	
		if (close_paren.type == TOKEN_TYPE_IDENTIFIER) {
			parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "function arguments");
			return (AST_Statement){};
		}
		if (close_paren.type != TOKEN_TYPE_TOKEN || close_paren.token != TOKEN_CLOSE_PAREN) {
			parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "missing ')' after function");
			return (AST_Statement){};
		}
		parser_pop_token(parser);
	}

	
	Token type_or_brack = parser_peek_token(parser);
	if (type_or_brack.type == TOKEN_TYPE_IDENTIFIER) {
		parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "only i32, f32, bool or void types implemented");
		return (AST_Statement){};
	}
	if (type_or_brack.type != TOKEN_TYPE_TOKEN) {
		parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "expected token i32, f32, bool, or void");
		return (AST_Statement){};
	}

	AST_Data_Type ret_type = AST_DATA_TYPE_VOID;
	if (type_or_brack.token != TOKEN_OPEN_BRACKET) {
		ret_type = parser_data_type(parser);
		parser_pop_token(parser);
	}

	if (ret_type == AST_DATA_TYPE_UNKNOWN) {
		parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "expected a type");
		return (AST_Statement){};
	}

	Token open_bracket = parser_peek_token_skip_break(parser);
	if (open_bracket.type != TOKEN_TYPE_TOKEN || open_bracket.token != TOKEN_OPEN_BRACKET) {
		parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "missing '{' after func decl");
		return (AST_Statement){};
	}
	
	parser_pop_token(parser);

	AST_Statement_Block_Function func;
	func.name = name;
	func.ret_type = ret_type;

	return (AST_Statement){AST_STATEMENT_TYPE_BLOCK_FUNCTION, .s_block_function=func};
}

AST_Data_Type parser_data_type(Parser* parser) {
	Token tok = parser_peek_token(parser);
	wak_assert_msg(tok.type == TOKEN_TYPE_TOKEN, "expected token of type token");

	switch (tok.token)
	{
	case TOKEN_KEYWORD_BOOL:
		return AST_DATA_TYPE_BOOL;
	case TOKEN_KEYWORD_I32:
		return AST_DATA_TYPE_I32;
	case TOKEN_KEYWORD_F32:
		return AST_DATA_TYPE_F32;
	case TOKEN_KEYWORD_VOID:
		return AST_DATA_TYPE_VOID;
	default:
		return AST_DATA_TYPE_UNKNOWN;
	}
}

AST_Statement parser_eof(Parser* parser) {
	parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "parser_eof");
	return (AST_Statement){};
}
AST_Statement parser_assign(Parser* parser) {
	parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "parser_assign");
	return (AST_Statement){};
}
AST_Statement parser_type_decl(Parser* parser) {
	parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "parser_type");
	return (AST_Statement){};
}
AST_Statement parser_void_call(Parser* parser) {
	parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "parser_void_call");
	return (AST_Statement){};
}

Token parser_peek_token(Parser* parser) {
	return parser->tok_mod.tokens->start[parser->tok_index];
}
Token parser_peek_token_n(Parser* parser, size_t i) {
	return parser->tok_mod.tokens->start[parser->tok_index + i];
}

Token parser_peek_token_skip_break(Parser* parser) {
	Token tok = parser_peek_token(parser);
	while(tok.type == TOKEN_TYPE_TOKEN && tok.token == TOKEN_BREAK) {
		parser_pop_token(parser);
		tok = parser_peek_token(parser);
	}
	return tok;
}

void parser_pop_token(Parser* parser) {
	parser->tok_index++;
}
void parser_pop_token_n(Parser* parser, size_t i) {
	parser->tok_index += i;
}

void parser_emit_error_impl(Parser* parser, AST_Error_Code code, const char* msg, const char* file, unsigned int line) {
	AST_Error err = (AST_Error){ code, msg, file, line };
	parser->statement_error = true;
	parser_print_error(parser, err);
}

void parser_print_error(Parser* parser, AST_Error err) {
	const char* name = get_ast_error_name(err.code);
	Token_Pos* positions = parser->tok_mod.positions->start;
	Token_Pos pos = positions[parser->tok_index];

	size_t front_space = 1;
	size_t front_tab = 0;
	// walk backwards to find start of line
	const char* line_start = pos.start;
	while (line_start > parser->tok_mod.data_start && *(line_start) != '\n') {
		if (*line_start == '\t') front_tab++;
		else front_space++;
		line_start--;
	}
	// walk forwards to find end of line
	const char* line_end = pos.end;
	while (line_end < parser->tok_mod.data_end && *line_end != '\n')
		line_end++;


	// print error msg
	printf("%i:%i: \033[31merror:\033[0m ", pos.line_number, pos.character_number);
	printf("%s: %s\n", name, err.msg);

	// print line error
	char buffer[4096];
	size_t start_size = pos.start - line_start;
	size_t middle_size = pos.end - pos.start;
	size_t end_size = line_end - pos.end;
	wak_assert(start_size < sizeof(buffer) && middle_size < sizeof(buffer) && end_size < sizeof(buffer));

	buffer[start_size] = 0;
	strncpy(buffer, line_start, start_size);
	printf("%s", buffer);

	buffer[middle_size] = 0;
	strncpy(buffer, pos.start, middle_size);
	printf("\033[31m%s\033[0m", buffer);

	buffer[end_size] = 0;
	strncpy(buffer, pos.end, end_size);
	printf("%s\n", buffer);

	for(size_t i = 0; i < front_tab; i++)
		printf("\t");
	for(size_t i = 0; i < front_space-1; i++)
		printf(" ");
	for(size_t i = 0; i < middle_size; i++)
		printf("~");

	// print out location error created from
	printf("\ngenerated from:  %s:%i\n", err.file, err.line);
}
