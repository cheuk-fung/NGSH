%{
#include "ngsh.h"

char *yylinebuf;

#define YY_INPUT(buf,result,max_size) \
{ \
    char c = *yylinebuf++; \
    result = (c == '\0') ? YY_NULL : (buf[0] = c, 1); \
}
%}

%option header-file="lex.yy.h"
%option noyywrap

%%

"\n"		{ commit(); free_argv(); }
[<>|]		{ add_argv(yytext); }
[^ \t\n<>|]+	{ add_argv(yytext); }
[ \t]		{ }

%%

