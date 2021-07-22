#if false
// old parser

#include <wak_lang/parser.h>
#include <wak_lang/token_module.h>

#include <wak_lib/vector.h>
#include <wak_lib/assert.h>
#include <wak_lib/def.h>

#include <wak_lang/ast/expression.h>
#include <wak_lang/ast/statement_debug.h>

#include <wak_lang/ir/statement.h>


#include <string.h>

typedef struct {
	Token* limit;
	Token* start;

	vector_statement* statements;
	size_t statement_count;
	size_t statement_capacity;
} wak_parse_state;


//DEFINE_VECTOR(vector_statement, _Statement);
//DEFINE_VECTOR(vector_error, AST_Error);

typedef struct {
	Token_Module tok_mod;
	size_t tok_index;
	//vector_statement* statements;

	//vector_error* errors;
	bool fatal_error;
	bool statement_error;
} Parser;

#define PARSER_PARAMS wak_parse_state* state, Token* tok 
#define PARSER_ARGS state, tok


#define PARSER_ERR_PARAMS PARSER_PARAMS, unsigned int line
#define PARSER_ERR_ARGS PARSER_ARGS, __LINE__

#define PARSER_RET Token*

typedef PARSER_RET (*parser_error_handler)(PARSER_PARAMS, unsigned int line);

inline PARSER_RET parser_append(PARSER_PARAMS, IR_Statement statement);
PARSER_RET parser_append_dispatch(PARSER_PARAMS, IR_Statement statement);


PARSER_RET parser_dispatch(PARSER_PARAMS);


PARSER_RET parser_next(PARSER_PARAMS);
PARSER_RET parser_func_decl(PARSER_PARAMS);
PARSER_RET parser_var_decl(PARSER_PARAMS);
PARSER_RET parser_return(PARSER_PARAMS);
PARSER_RET parser_void_call(PARSER_PARAMS);
PARSER_RET parser_eof(PARSER_PARAMS);
PARSER_RET parser_assign(PARSER_PARAMS);
PARSER_RET parser_end_block(PARSER_PARAMS);

PARSER_RET parser_struct(PARSER_PARAMS);
PARSER_RET parser_type_decl(PARSER_PARAMS);
PARSER_RET parser_token(PARSER_PARAMS);
PARSER_RET parser_identifier(PARSER_PARAMS);

//void parser_print_error(Parser* parser, AST_Error error);


PARSER_RET parser_error_impl(PARSER_PARAMS, parser_error_handler handler, unsigned int line);
PARSER_RET parser_missing(PARSER_ERR_PARAMS, const char* msg);
PARSER_RET parser_err_fatal(PARSER_PARAMS);


#define DEFINE_ERROR_FUNC(name) PARSER_RET error_handle_##name(PARSER_PARAMS, unsigned int line) 

Parsed_Module parse(Token_Module module) {

	wak_parse_state state = (wak_parse_state){ 
		.limit=module.tokens->end, 
		.start=module.tokens->start,
		.statement_count=0,
		.statement_capacity=10,
		.statements=array_alloc(10, IR_Statement)
		};


	return (Parsed_Module) {};
}

PARSER_RET parser_dispatch(PARSER_PARAMS) {

	if (tok >= state->limit) {
		return tok;
	}

	return parser_next(PARSER_ARGS);
}


PARSER_RET parser_next(PARSER_PARAMS) {
	switch (tok->type)
	{
	case TOKEN_TYPE_TOKEN:
		return parser_token(PARSER_ARGS);
	case TOKEN_TYPE_IDENTIFIER:
		return parser_identifier(PARSER_ARGS);
	default:
		return err_handle_unexpected_literal(PARSER_ERR_ARGS);
	}
}

PARSER_RET err_handle_unknown_token(PARSER_ERR_PARAMS) {

}

PARSER_RET parser_token(PARSER_PARAMS) {
	wak_assert(tok->type == TOKEN_TYPE_TOKEN);
	
	switch (tok->token)
	{
		case TOKEN_KEYWORD_FUNC:
			return parser_func_decl(PARSER_ARGS);
		case TOKEN_KEYWORD_TYPE:
			return parser_type_decl(PARSER_ARGS);
		case '}':
			return parser_end_block(PARSER_ARGS);
		case TOKEN_KEYWORD_RETURN:
			return parser_return(PARSER_ARGS);
		case TOKEN_EOF:
			return parser_eof(PARSER_ARGS);
		case TOKEN_BREAK:
			return parser_dispatch(PARSER_ARGS);
		default:
			return err_handle_unknown_token(PARSER_ERR_ARGS);
	}
}

PARSER_RET err_handle_invalid_op(PARSER_ERR_PARAMS) {

	return tok;
}


PARSER_RET parser_identifier(PARSER_PARAMS) {
	wak_assert(tok->type == TOKEN_TYPE_IDENTIFIER);

	Token* next_tok = tok+1;
	if (next_tok->type != TOKEN_TYPE_TOKEN) { 
		return err_handle_invalid_op(PARSER_ERR_ARGS);
	}
	
	switch (next_tok->token)
	{
		case '(':
			return parser_void_call(PARSER_ARGS);
		case ':':
			return parser_var_decl(PARSER_ARGS);
		case '=':
			return parser_assign(PARSER_ARGS);
		case '.':
			// TODO handle dot operator
			return parser_missing(PARSER_ERR_ARGS, "dot operator");
		case '{':
			// TODO object construction
			return parser_missing(PARSER_ERR_ARGS, "obj constructor");
		case '[':
			// TODO array access
			return parser_missing(PARSER_ERR_ARGS, "array operator");
		case '*':
		case '/':
		case '+':
		case '-':
			// TODO math operation
			// assignment (+=...)
			return parser_missing(PARSER_ERR_ARGS, "math operator");
		default:
			// TODO syntax error
			return parser_missing(PARSER_ERR_ARGS, "unknown operation");
	}

}

PARSER_RET parser_expr(PARSER_PARAMS) {
	switch (tok->type)
	{
	case TOKEN_TYPE_LITERAL_STR:
	case TOKEN_TYPE_LITERAL_CHAR:
	case TOKEN_TYPE_LITERAL_NUM:
	case TOKEN_TYPE_LITERAL_BOOL:
		return parser_expr_literal(PARSER_ARGS);
	case TOKEN_TYPE_IDENTIFIER:
		return parser_expr_iden(PARSER_ARGS);
	default:
		return parser_missing(PARSER_ERR_ARGS, "expression token"); // TODO: error
	}
}

PARSER_RET err_handle_literal_decimal(PARSER_ERR_PARAMS, const char* msg) {

}

PARSER_RET parser_expr_literal_decimal(PARSER_PARAMS) {
	wak_assert( tok->type == TOKEN_TYPE_LITERAL_NUM );

	const char* str = tok->literal_str;
	size_t str_len = strlen(str);


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
			else return err_handle_num(PARSER_ERR_ARGS, "multiple decimal points");
			break;
		default:
			return err_handle_num(PARSER_ERR_ARGS, "invalid character");
		}
	}

	tok++;

	if ( frac_dig == 0) {
		IR_Statement statement = (IR_Statement){
			.type=IR_STATEMENT_TYPE_LITERAL,
			.s_literal=(IR_Statement_Literal) {
				.data_type=IR_DATA_TYPE_I32,
				.i32=int_val
			}
		};
		return parser_append(PARSER_ARGS, statement);
	} else {
		double f = int_val;
		f += (float)frac_val / (float)frac_dig;
		IR_Statement statement = (IR_Statement){
			.type=IR_STATEMENT_TYPE_LITERAL,
			.s_literal=(IR_Statement_Literal) {
				.data_type=IR_DATA_TYPE_F32,
				.f32=f
			}
		};
		return parser_append(PARSER_ARGS, statement);
	}

	
}

PARSER_RET parser_expr_literal_hex(PARSER_PARAMS) {
	return parser_missing(PARSER_ERR_ARGS, "hex literal");
}

PARSER_RET parser_expr_literal_binary(PARSER_PARAMS) {
	return parser_missing(PARSER_ERR_ARGS, "binary literal");
}


PARSER_RET parser_expr_literal_num(PARSER_PARAMS) {
	wak_assert(tok->type == TOKEN_TYPE_LITERAL_NUM);

	const char* str = tok->literal_str;
	size_t str_len = strlen(str);

	if (str[0] == '0' && str_len > 1) {
		if (str[1] == 'x') return parser_expr_literal_hex(PARSER_ARGS);
		if (str[1] == 'b') return parser_expr_literal_binary(PARSER_ARGS);
	}
	

	return parser_expr_literal_decimal(PARSER_ARGS);
}

PARSER_RET parser_expr_literal(PARSER_PARAMS) {
	
	switch (tok->type)
	{
	case TOKEN_TYPE_LITERAL_NUM:
		return parser_expr_literal_num(PARSER_ARGS);
	case TOKEN_TYPE_LITERAL_STR:
	case TOKEN_TYPE_LITERAL_BOOL:
	case TOKEN_TYPE_LITERAL_CHAR:
		return parser_missing(PARSER_ERR_ARGS, "literal type");
	default:
		// token was not a literal
		wak_assert(false); 
		break;
	}

	return parser_err_fatal(PARSER_ARGS);
}



PARSER_RET parser_expr_iden(PARSER_PARAMS) {
	wak_assert( tok->type == TOKEN_TYPE_IDENTIFIER);

	Token* next_tok = tok + 1;
	if (next_tok->type == TOKEN_TYPE_TOKEN && next_tok->token == TOKEN_OPEN_PAREN)
		return parser_expr_call(PARSER_ARGS);
	
	if (next_tok->type == TOKEN_TYPE_TOKEN && next_tok->token == TOKEN_SEMICOLON)
		return parser_expr_var(PARSER_ARGS);

	return parser_missing(PARSER_ERR_ARGS, "identifier expression");
}


// TODO: fix how this searches
//		 this also wont handle constants
PARSER_RET parser_expr_var(PARSER_PARAMS) {
	wak_assert( tok->type == TOKEN_TYPE_IDENTIFIER);

	const char* name = tok->identifier;
	size_t size = vector_count((vector*)state->statements);
	// search backwards till we find a match
	for(size_t i = size; i > 0; i--) {
		IR_Statement* itr = state->statements->start + i;
		if (itr->type != IR_STATEMENT_TYPE_DECLARE) continue;

		int cmp = strcmp(name, itr->s_declare.name);

		if (cmp != 0) continue;

		tok++;

		IR_Statement var = { .type=IR_STATEMENT_TYPE_VAR_REF, .s_var_ref={i}};
		return parser_append(PARSER_ARGS, var);
	}

	printf("could not find variable: %s", name);
	return tok;
}

PARSER_RET parser_expr_call(PARSER_PARAMS) {
	wak_assert( tok->type == TOKEN_TYPE_IDENTIFIER);
	
	return parser_missing(PARSER_ERR_ARGS, "expression call");
}

PARSER_RET parser_return(PARSER_PARAMS) {
	wak_assert(tok->type == TOKEN_TYPE_TOKEN && tok->token == TOKEN_KEYWORD_RETURN);

	tok++;
	
	return parser_append(PARSER_ARGS, (IR_Statement){ IR_STATEMENT_TYPE_RETURN });
}

PARSER_RET parser_end_block(PARSER_PARAMS) {
	
	// IMPROVE: maybe keep a "stack" of blocks

	size_t size = vector_count((vector*)state->statements);
	// search backwards for a block
	for(size_t i = size; i > 0; i--) {
		//AST_Statement* itr = parser->statements->start + i;
		//if (!statement_is_block(*itr) || itr->) continue;


		tok++;
		IR_Statement statement = { .type=IR_STATEMENT_TYPE_BLOCK_END };
		parser_append_dispatch(PARSER_ARGS, statement);
	}

	//parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "unexpected '}'");
	return parser_dispatch(PARSER_ARGS);
}

PARSER_RET parser_find_variable(PARSER_PARAMS) {
	return parser_missing(PARSER_ERR_ARGS, "parser_find_variable");
}


// TODO: fatal error
// TODO: skip to end of declaration
PARSER_RET parser_var_decl_err_missing_infer(PARSER_PARAMS) {
	//

	return tok++;
}

// TODO: fatal error
// TODO: skip to end of declaration
PARSER_RET parser_var_decl_err_invalid_type(PARSER_PARAMS) {
	
	return tok++;
}

PARSER_RET parser_var_decl(PARSER_PARAMS) {
	wak_assert(tok->type == TOKEN_TYPE_IDENTIFIER);
	// TODO: check for repeat declarations
	const char* name = tok->identifier;
	tok++;

	wak_assert(tok->type == TOKEN_TYPE_TOKEN && tok->token == TOKEN_COLON);
	tok++;

	if (tok->type != TOKEN_TYPE_TOKEN || tok->token == TOKEN_EQUALS) {
		return parser_var_decl_err_missing_infer(PARSER_ARGS);
	}

	AST_Data_Type data_type = parser_data_type(PARSER_ARGS);
	if (data_type == AST_DATA_TYPE_UNKNOWN) {
		return parser_var_decl_err_invalid_type_type(PARSER_ARGS);
	}
	tok++;

	IR_Statement_Declare decl = {.data_type= data_type, .name= name };


	IR_Statement decl_statement = {.type=IR_STATEMENT_TYPE_DECLARE, .s_declare=decl};


	if (tok->type == TOKEN_TYPE_TOKEN && tok->token == TOKEN_SEMICOLON) {
		tok++;
		return parser_append_dispatch(PARSER_ARGS, decl_statement);
	}
	
	if (tok->type != TOKEN_TYPE_TOKEN || tok->token != TOKEN_EQUALS) {
		//parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "expected ';' or '='");
		return parser_dispatch(PARSER_ARGS);
	}
	tok++;
	
	size_t index = vector_count((vector*)state->statements);
	parser_append(PARSER_ARGS, decl_statement);

	AST_Value value = parser_expr(parser);

	if (value.type == AST_VALUE_TYPE_NULL)
		return (AST_Statement){};

	AST_Statement_Assign assign = (AST_Statement_Assign) { .dest_index = index, .value = value };

	return (AST_Statement){AST_STATEMENT_TYPE_ASSIGN, .s_assign=assign};
}


PARSER_RET parser_func_decl(PARSER_PARAMS) {
	wak_assert(tok->type == TOKEN_TYPE_TOKEN && tok->token == TOKEN_KEYWORD_FUNC);
	tok++;

	if (tok->type != TOKEN_TYPE_IDENTIFIER) {
		parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "missing function name");
		return (AST_Statement){};
	}
	
	const char* name = tok->identifier;
	tok++;


	if (tok->type != TOKEN_TYPE_TOKEN || tok->token != TOKEN_OPEN_PAREN) {
		parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "missing '(' after func name");
		return (AST_Statement){};
	}
	tok++;

	if (tok->type == TOKEN_TYPE_IDENTIFIER) {
		parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "function arguments");
		return (AST_Statement){};
	}
	if (tok->type != TOKEN_TYPE_TOKEN || tok->token != TOKEN_CLOSE_PAREN) {
		parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "missing ')' after function");
		return (AST_Statement){};
	}
	tok++;
	

	if (tok->type == TOKEN_TYPE_IDENTIFIER) {
		parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "only i32, f32, bool or void types implemented");
		return (AST_Statement){};
	}
	if (tok->type != TOKEN_TYPE_TOKEN) {
		parser_emit_error(parser, AST_ERROR_INVALID_SYNTAX, "expected token i32, f32, bool, or void");
		return (AST_Statement){};
	}

	AST_Data_Type ret_type = AST_DATA_TYPE_VOID;
	if (tok->token != TOKEN_OPEN_BRACKET) {
		ret_type = parser_data_type(parser);
		tok++;
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
	
	tok++;

	AST_Statement_Block_Function func;
	func.name = name;
	func.ret_type = ret_type;

	return (AST_Statement){AST_STATEMENT_TYPE_BLOCK_FUNCTION, .s_block_function=func};
}

AST_Data_Type parser_data_type(PARSER_PARAMS) {
	wak_assert(tok->type == TOKEN_TYPE_TOKEN);

	switch (tok->token)
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

PARSER_RET parser_eof(PARSER_PARAMS) {
	parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "parser_eof");
	return (AST_Statement){};
}
PARSER_RET parser_assign(PARSER_PARAMS) {
	parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "parser_assign");
	return (AST_Statement){};
}
PARSER_RET parser_type_decl(PARSER_PARAMS) {
	parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "parser_type");
	return (AST_Statement){};
}
PARSER_RET parser_void_call(PARSER_PARAMS) {
	parser_emit_error(parser, AST_ERROR_UNIMPLEMENTED, "parser_void_call");
	return (AST_Statement){};
}


Token parser_peek_token_skip_break(PARSER_PARAMS) {
	Token tok = parser_peek_token(parser);
	while(tok.type == TOKEN_TYPE_TOKEN && tok.token == TOKEN_BREAK) {
		parser_pop_token(parser);
		tok = parser_peek_token(parser);
	}
	return tok;
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


#endif