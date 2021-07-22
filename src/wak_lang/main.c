#include <wak_lib/assert.h>
#include <wak_lib/timers.h>
#include <wak_lib/file.h>

#include <wak_lang/lexer.h>
#include <wak_lang/parser.h>




void print_token(FILE* fp, Token tok, Token_Pos pos) {
	fprintf(fp, "%i:%i ", pos.line_number, pos.character_number);
	if (tok.type == TOKEN_TYPE_TOKEN) {
		fprintf(fp, "%s \n", token_value_to_name(tok.token));
	} else if (tok.type == TOKEN_TYPE_IDENTIFIER) {
		fprintf(fp, "%s \n", tok.identifier);
	} else if (tok.type >= TOKEN_TYPE_LITERAL_STR) {
		fprintf(fp, "%s \n", tok.literal_str);
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
	if (src_fd == -1) {
		printf("could not open file\n");
		return 1;
	}
	mmapped_file src_mmf = fd_mem_map(src_fd);
	timer_end(TIMER_FILE_READ);


	timer_start(TIMER_LEXETIZE);

	Token_Module module = lexetize(src_mmf.data, src_mmf.data+src_mmf.length+1);
	
	timer_end(TIMER_LEXETIZE);

	FILE* token_fp = fopen("tokens.txt", "w");

	fprintf(token_fp, "\n\n TOKENIZED OUTPUT: %zu\n",  vector_count((vector*)module.tokens));
	for (size_t i = 0; i < vector_count((vector*)module.tokens); i++) {
		Token tok = module.tokens->start[i];
		Token_Pos pos = module.positions->start[i];
		print_token(token_fp, tok, pos);	
	}


	

	timer_start(TIMER_PARSE);
	printf("\n\nPARSER OUTPUT:\n");
	Parsed_Module parsed_mod = parse(module);
	
	fclose(token_fp);
	token_module_free(&module);

	timer_end(TIMER_PARSE);
	
	// free file resources
	fd_mem_unmap(&src_mmf);
	fd_close(src_fd);

	timer_end(TIMER_TOTAL);
	timer_report_all();
	return 0;
}