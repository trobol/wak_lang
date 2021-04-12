#include <wak_lang/util/assert.h>
#include <wak_lang/util/timers.h>
#include <wak_lang/util/file.h>

#include <wak_lang/lexer.h>
#include <wak_lang/parser.h>




void print_token(Token tok, Token_Pos pos) {
	printf("%i:%i ", pos.line_number, pos.character_number);
	if (tok.type == TOKEN_TYPE_TOKEN) {
		printf("%s ", token_value_to_name(tok.token));
	} else if (tok.type == TOKEN_TYPE_IDENTIFIER) {
		printf("%s ", tok.identifier);
	} else if (tok.type == TOKEN_TYPE_LITERAL) {
		printf("%s ", tok.literal.v_str);
	}
}

int main(int argc, char *argv[]) {
	//_wak_assert_msg(0, "test");
	timer_start(TIMER_TOTAL);

	if (argc != 2) {
		printf("invalid number of arguments\n");
		return 1;
	}

	timer_start(TIMER_FILE_READ);
	file_descriptor src_fd = fd_open(argv[1], 0);
	if (!src_fd) {
		printf("could not open file\n");
		return 1;
	}
	mmapped_file src_mmf = fd_mem_map(src_fd);
	timer_end(TIMER_FILE_READ);


	timer_start(TIMER_LEXETIZE);

	Token_Module module = lexetize(src_mmf.data, src_mmf.data+src_mmf.length);
	
	printf("\n\n TOKENIZED OUTPUT:\n");
	for (size_t i = 0; i < vector_count((vector*)module.tokens); i++) {
		Token tok = module.tokens->start[i];
		Token_Pos pos = module.positions->start[i];
		print_token(tok, pos);	
	}


	timer_end(TIMER_LEXETIZE);

	timer_start(TIMER_PARSE);
	printf("\n\nPARSER OUTPUT:\n");
	Parsed_Module parsed_mod = parse(module);
	


	timer_end(TIMER_PARSE);
	
	// free file resources
	fd_mem_unmap(&src_mmf);
	fd_close(src_fd);

	timer_end(TIMER_TOTAL);
	timer_report_all();
	return 0;
}