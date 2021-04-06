#ifndef _WAK_LANG_LEXER_H
#define _WAK_LANG_LEXER_H
#include <wak_lang/token.h>
#include <wak_lang/token_module.h>

#include <stddef.h>

Token_Module lexetize(const char* start, const char* end);

#endif