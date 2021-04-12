#include <wak_lang/parser.h>
#include <wak_lang/token_module.h>
#include <wak_lang/util/vector.h>
#include <wak_lang/util/assert.h>
#include <wak_lang/ast/expression.h>
#include <wak_lang/ast/statement_debug.h>

#include <string.h>

DEFINE_VECTOR(vector_statement, AST_Statement);

typedef struct {
	Token_Module tok_mod;
	size_t tok_index;
	vector_statement* statements;

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


Ref_AST_Statement parser_expr(Parser* parser, AST_Statement* err);
Ref_AST_Statement parser_expr_literal(Parser* parser, AST_Statement* err);
Ref_AST_Statement parser_expr_iden(Parser* parser, AST_Statement* err);
Ref_AST_Statement parser_expr_call(Parser* parser, AST_Statement* err);
Ref_AST_Statement parser_expr_var(Parser* parser, AST_Statement* err);

void parser_print_error(Parser* parser, AST_Statement_Error error);

Parsed_Module parse(Token_Module module) {
	Parser parser = (Parser){ module, 0, vector_statement_new()};

	AST_Statement statement;
	do
	{
		statement = parser_next_statement(&parser);

		if (statement.type == AST_STATEMENT_TYPE_ERROR) {
			parser_print_error(&parser, statement.s_error);
			return (Parsed_Module) {};
		} else {
			vector_statement_append(parser.statements, statement);
		}
		// TODO: add positions
		

	} while(statement.type != AST_STATEMENT_TYPE_EOF);

	return (Parsed_Module) {};
}

AST_Statement parser_next_statement(Parser* parser) {
	Token tok = parser_peek_token(parser);
	switch (tok.type)
	{
	case TOKEN_TYPE_TOKEN:
		return parser_token(parser);
	case TOKEN_TYPE_IDENTIFIER:
		return parser_identifier(parser);
	default:
		return make_ast_error(AST_ERROR_INVALID_SYNTAX, "cannot have empty literal");
	}
	printf("error: \n");
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
			return make_ast_error(AST_ERROR_UNSUPPORTED, "unknown token");
	}
}
AST_Statement parser_identifier(Parser* parser) {
	Token tok = parser_peek_token(parser);
	wak_assert_msg(tok.type == TOKEN_TYPE_IDENTIFIER, "expected token of type identifier");
	Token next_tok = parser_peek_token_n(parser, 1);
	if (next_tok.type != TOKEN_TYPE_TOKEN) { 
		return make_ast_error(AST_ERROR_INVALID_SYNTAX, "invalid operation on token");
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
			return make_ast_error(AST_ERROR_UNIMPLEMENTED, "dot operator");
		case '{':
			// TODO object construction
			return make_ast_error(AST_ERROR_UNIMPLEMENTED, "obj constr");
		case '[':
			// TODO array access
			return make_ast_error(AST_ERROR_UNIMPLEMENTED, "array operator");
		case '*':
		case '/':
		case '+':
		case '-':
			// TODO math operation
			// assignment (+=...)
			return make_ast_error(AST_ERROR_UNIMPLEMENTED, "math operation");
		default:
			// TODO syntax error
			return make_ast_error(AST_ERROR_INVALID_SYNTAX, "unknown operation");
	}

}

Ref_AST_Statement parser_expr(Parser* parser, AST_Statement* err) {
	Token tok = parser_peek_token(parser);
	switch (tok.type)
	{
	case TOKEN_TYPE_LITERAL:
		return parser_expr_literal(parser, err);
	case TOKEN_TYPE_IDENTIFIER:
		return parser_expr_iden(parser, err);
	default:
		return (Ref_AST_Statement){0}; // TODO: error
	}
}

Ref_AST_Statement parser_expr_literal(Parser* parser, AST_Statement* err) {
	Token tok = parser_peek_token(parser);
	wak_assert_msg( tok.type == TOKEN_TYPE_LITERAL, "expected a token of type literal");
	

	*err = make_ast_error(AST_ERROR_UNSUPPORTED, "literal");
	return (Ref_AST_Statement){0}; // TODO: error
}

Ref_AST_Statement parser_expr_iden(Parser* parser, AST_Statement* err) {
	Token tok = parser_peek_token(parser);
	wak_assert_msg( tok.type == TOKEN_TYPE_IDENTIFIER, "expected a token of type identifier");

	Token next_tok = parser_peek_token_n(parser, 1);
	if (next_tok.type == TOKEN_TYPE_TOKEN && next_tok.token == TOKEN_OPEN_PAREN)
		return parser_expr_call(parser, err);
	
	if (next_tok.type == TOKEN_TYPE_TOKEN && next_tok.token == TOKEN_SEMICOLON)
		return parser_expr_var(parser, err);

	return (Ref_AST_Statement){0}; // TODO: error
}

// TODO: fix how this searches
//		 this also wont handle constants
Ref_AST_Statement parser_expr_var(Parser* parser, AST_Statement* err) {
	Token tok = parser_peek_token(parser);
	wak_assert_msg( tok.type == TOKEN_TYPE_IDENTIFIER, "expected a token of type identifier");

	const char* name = tok.identifier;
	size_t size = vector_count((vector*)parser->statements);
	// search backwards till we find a match
	for(size_t i = size; i > 0; i--) {
		AST_Statement* itr = parser->statements->start + i;
		if (itr->type != AST_STATEMENT_TYPE_DECLARE) break;

		int cmp = strcmp(name, itr->s_declare.name);

		if (cmp != 0) break;

		return (Ref_AST_Statement){i};
	}

	printf("could not find variable: %s", name);
	return (Ref_AST_Statement){0}; // TODO: error
}

Ref_AST_Statement parser_expr_call(Parser* parser, AST_Statement* err) {
	Token tok = parser_peek_token(parser);
	wak_assert_msg( tok.type == TOKEN_TYPE_IDENTIFIER, "expected a token of type identifier");
	
	return (Ref_AST_Statement){0}; // TODO: error
}

AST_Statement parser_return(Parser* parser) {
	return make_ast_error(AST_ERROR_UNIMPLEMENTED, "parser_return");
}

AST_Statement parser_end_block(Parser* parser) {
	return make_ast_error(AST_ERROR_UNIMPLEMENTED, "parser_end_block");
}

AST_Statement parser_find_variable(Parser* parser) {
	return make_ast_error(AST_ERROR_UNIMPLEMENTED, "parser_find_variable");
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
	if (type_tok.type != TOKEN_TYPE_TOKEN)
		return make_ast_error(AST_ERROR_INVALID_SYNTAX, "expected token");
	if (type_tok.token == TOKEN_EQUALS)
		return make_ast_error(AST_ERROR_UNIMPLEMENTED, "type inference");

	AST_Data_Type data_type = parser_data_type(parser);
	if (data_type == AST_DATA_TYPE_UNKNOWN) 
		return make_ast_error(AST_ERROR_INVALID_SYNTAX, "not a type");	
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
	
	if (next.type != TOKEN_TYPE_TOKEN || next.token != TOKEN_EQUALS)
		return make_ast_error(AST_ERROR_INVALID_SYNTAX, "expected ';' or '='");
	parser_pop_token(parser);
	size_t index = vector_count((vector*)parser->statements);
	vector_statement_append(parser->statements, decl_statement);

	AST_Statement err = make_ast_error(AST_ERROR_NONE, "");
	Ref_AST_Statement value = parser_expr(parser, &err);

	if (err.s_error.code != AST_ERROR_NONE)
		return err;

	AST_Statement_Assign assign;
	assign.dest = (Ref_AST_Statement){index};
	assign.value = value;
	return (AST_Statement){AST_STATEMENT_TYPE_ASSIGN, .s_assign=assign};
}


AST_Statement parser_func_decl(Parser* parser) {
	{
		Token func_tok = parser_peek_token(parser);
		wak_assert_msg(func_tok.type == TOKEN_TYPE_TOKEN && func_tok.token == TOKEN_KEYWORD_FUNC, "expected tok: func");
		parser_pop_token(parser);
	}

	Token name_tok = parser_peek_token(parser);
	if (name_tok.type != TOKEN_TYPE_IDENTIFIER)
		return make_ast_error(AST_ERROR_INVALID_SYNTAX, "missing function name");
	
	const char* name = name_tok.identifier;
	parser_pop_token(parser);

	{
		Token open_paren = parser_peek_token(parser);
		if (open_paren.type != TOKEN_TYPE_TOKEN || open_paren.token != TOKEN_OPEN_PAREN)
			return make_ast_error(AST_ERROR_INVALID_SYNTAX, "missing '(' after func name");
		parser_pop_token(parser);

		Token close_paren = parser_peek_token(parser);	
		if (close_paren.type == TOKEN_TYPE_IDENTIFIER)
			return make_ast_error(AST_ERROR_UNIMPLEMENTED, "function arguments");
		if (close_paren.type != TOKEN_TYPE_TOKEN || close_paren.token != TOKEN_CLOSE_PAREN) 
			return make_ast_error(AST_ERROR_INVALID_SYNTAX, "missing ')' after function");
		parser_pop_token(parser);
	}

	
	Token type_or_brack = parser_peek_token(parser);
	if (type_or_brack.type == TOKEN_TYPE_IDENTIFIER)
		return make_ast_error(AST_ERROR_UNIMPLEMENTED, "only i32, f32, bool or void types implemented");
	if (type_or_brack.type != TOKEN_TYPE_TOKEN)
		return make_ast_error(AST_ERROR_INVALID_SYNTAX, "expected token i32, f32, bool, or void");
	
	AST_Data_Type ret_type = AST_DATA_TYPE_VOID;
	if (type_or_brack.token != TOKEN_OPEN_BRACKET) {
		ret_type = parser_data_type(parser);
		parser_pop_token(parser);
	}

	if (ret_type == AST_DATA_TYPE_UNKNOWN)
		return make_ast_error(AST_ERROR_INVALID_SYNTAX, "expected a type");

	Token open_bracket = parser_peek_token_skip_break(parser);
	if (open_bracket.type != TOKEN_TYPE_TOKEN || open_bracket.token != TOKEN_OPEN_BRACKET)
			return make_ast_error(AST_ERROR_INVALID_SYNTAX, "missing '{' after func decl");
	
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
	return make_ast_error(AST_ERROR_UNIMPLEMENTED, "parser_eof");
}
AST_Statement parser_assign(Parser* parser) {
	return make_ast_error(AST_ERROR_UNIMPLEMENTED, "parser_assign");
}
AST_Statement parser_type_decl(Parser* parser) {
	return make_ast_error(AST_ERROR_UNIMPLEMENTED, "parser_type");
}
AST_Statement parser_void_call(Parser* parser) {
	return make_ast_error(AST_ERROR_UNIMPLEMENTED, "parser_void_call");
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


void parser_print_error(Parser* parser, AST_Statement_Error err) {
	const char* name = get_ast_error_name(err.code);
	Token_Pos* positions = parser->tok_mod.positions->start;
	Token* tokens = parser->tok_mod.tokens;
	Token_Pos pos = positions[parser->tok_index];

	size_t front_space = 0;
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
