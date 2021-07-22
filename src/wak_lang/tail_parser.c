#include <wak_lang/parser.h>
#include <wak_lang/token_module.h>

#include <wak_lib/vector.h>
#include <wak_lib/assert.h>
#include <wak_lib/mem.h>
#include <wak_lib/def.h>

#include <wak_lang/ast/expression.h>
#include <wak_lang/ast/statement_debug.h>

#include <wak_lang/ir/statement.h>

#include <string.h>


/*
 * The goal is to have as many fuction calls as possible tail
 * another pass is most likely needed, at least to fix order of operations
 * 
 */

typedef struct {
	Token* limit;
	Token* start;

	IR_Statement* statements;
	size_t statement_count;
	size_t statement_capacity;

	Token_Module token_module;
} wak_parse_state;


#define PTC_PARAMS wak_parse_state* state, Token* tok 
#define PTC_ARGS state, tok
#define PTC_RET Token*

#define PTC_ERR_PARAMS PTC_PARAMS, unsigned int line
#define PTC_ERR_ARGS PTC_ARGS, __LINE__


inline PTC_RET ptc_append(PTC_PARAMS, IR_Statement statement);
PTC_RET ptc_append_dispatch(PTC_PARAMS, IR_Statement statement);


PTC_RET ptc_dispatch(PTC_PARAMS);


PTC_RET ptc_token(PTC_PARAMS);
PTC_RET ptc_identifier(PTC_PARAMS);

PTC_RET ptc_func_decl(PTC_PARAMS);
PTC_RET ptc_type_decl(PTC_PARAMS);
PTC_RET ptc_end_block(PTC_PARAMS);
PTC_RET ptc_return(PTC_PARAMS);
PTC_RET ptc_eof(PTC_PARAMS);
PTC_RET ptc_break(PTC_PARAMS);

void ptc_err_report(PTC_ERR_PARAMS, const char* message); 

PTC_RET ptc_err_fatal(PTC_ERR_PARAMS, const char* message);


Parsed_Module parse(Token_Module module) {

	wak_parse_state state = (wak_parse_state){ 
		.limit=module.tokens->end, 
		.start=module.tokens->start,

		.statements=array_alloc(10, IR_Statement),
		.statement_count=0,
		.statement_capacity=10,

		.token_module=module
		};
	
	ptc_dispatch(&state, state.start);


	return (Parsed_Module) {};
}

// call this for each iteration
// begining and end of loop
PTC_RET ptc_dispatch(PTC_PARAMS) {

	if (WAK_UNLIKELY(tok >= state->limit || tok == 0)) {
		return tok;
	}

	switch (tok->type)
	{
	case TOKEN_TYPE_TOKEN:
		return ptc_token(PTC_ARGS);
	case TOKEN_TYPE_IDENTIFIER:
		return ptc_identifier(PTC_ARGS);
	default:
		return ptc_err_fatal(PTC_ERR_ARGS, "unexpected literal `%s`");
	}
}


PTC_RET ptc_token(PTC_PARAMS) {
	wak_assert(tok->type == TOKEN_TYPE_TOKEN);
	
	switch (tok->token)
	{
		case TOKEN_KEYWORD_FUNC:
			return ptc_func_decl(PTC_ARGS);
		case TOKEN_KEYWORD_TYPE:
			return ptc_type_decl(PTC_ARGS);
		case '}':
			return ptc_end_block(PTC_ARGS);
		case TOKEN_KEYWORD_RETURN:
			return ptc_return(PTC_ARGS);
		case TOKEN_EOF:
			return ptc_eof(PTC_ARGS);
		case TOKEN_BREAK:
			return ptc_break(PTC_ARGS);
		default:
			return ptc_err_fatal(PTC_ERR_ARGS, "unknown token");
	}
}

PTC_RET ptc_break(PTC_PARAMS) {
	tok++;
	return ptc_dispatch(PTC_ARGS);
}

PTC_RET ptc_func_decl(PTC_PARAMS) {
	return ptc_err_fatal(PTC_ERR_ARGS, "unimplemented");
}

PTC_RET ptc_type_decl(PTC_PARAMS) {
	return ptc_err_fatal(PTC_ERR_ARGS, "unimplemented");
}

PTC_RET ptc_end_block(PTC_PARAMS) {
	return ptc_err_fatal(PTC_ERR_ARGS, "unimplemented");	
}

PTC_RET ptc_return(PTC_PARAMS) {
	return ptc_err_fatal(PTC_ERR_ARGS, "unimplemented");
}

PTC_RET ptc_eof(PTC_PARAMS) {
	return ptc_err_fatal(PTC_ERR_ARGS, "unimplemented");
}


PTC_RET ptc_identifier(PTC_PARAMS) {
	return ptc_err_fatal(PTC_ERR_ARGS, "identifier parsing not implemented");
}


PTC_RET ptc_err_fatal(PTC_ERR_PARAMS, const char* msg) {
	ptc_err_report(state, tok, line, msg);
	printf("\nparser reported a fatal syntax error\n");
	return 0;
}


void ptc_err_report(PTC_ERR_PARAMS, const char* msg) {
	
	Token_Pos* positions = state->token_module.positions->start;
	size_t tok_index = tok - state->start;
	Token_Pos pos = positions[tok_index];

	size_t front_space = 1;
	size_t front_tab = 0;
	// walk backwards to find start of line
	const char* line_start = pos.start;
	while (line_start > state->token_module.data_start && *(line_start) != '\n') {
		if (*line_start == '\t') front_tab++;
		else front_space++;
		line_start--;
	}
	// walk forwards to find end of line
	const char* line_end = pos.end;
	while (line_end < state->token_module.data_end && *line_end != '\n')
		line_end++;

	
	char buffer[4096];
	size_t start_size = pos.start - line_start;
	size_t token_str_size = pos.end - pos.start;
	size_t end_size = line_end - pos.end;
	wak_assert(start_size < sizeof(buffer) && token_str_size < sizeof(buffer) && end_size < sizeof(buffer));
	
	buffer[token_str_size] = 0;
	strncpy(buffer, pos.start, token_str_size);
	// print error msg
	printf("%i:%i: \033[31merror:\033[0m ", pos.line_number, pos.character_number);
	printf(msg, buffer);

	
	// print line error
	buffer[start_size] = 0;
	strncpy(buffer, line_start, start_size);
	printf("%s", buffer);

	buffer[token_str_size] = 0;
	strncpy(buffer, pos.start, token_str_size);
	printf("\033[31m%s\033[0m", buffer);

	buffer[end_size] = 0;
	strncpy(buffer, pos.end, end_size);
	printf("%s\n", buffer);

	for(size_t i = 0; i < front_tab; i++)
		printf("\t");
	for(size_t i = 0; i < front_space-1; i++)
		printf(" ");
	for(size_t i = 0; i < token_str_size; i++)
		printf("~");

	// print out location error created from
	printf("\ngenerated from:  %s:%i\n", __FILE__, line);
}



