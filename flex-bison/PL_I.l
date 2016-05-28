%{
#include <stdio.h>
#include <string.h>
#define YYSTYPE char *
#include "./PL_I.tab.h"
extern YYSTYPE yylval;
%}
%option yylineno
%%
":"                  {
//                       printf("\n***lex: term=%s;\n", yytext);
                       return ':';
                     }
"("                  {
//                       printf("\n***lex: term=%s;\n", yytext);
                       return '(';
                     }
")"                  {
//                       printf("\n***lex: term=%s;\n", yytext);
                       return ')';
                     }
";"                  {
//                       printf("\n***lex: term=%s;\n", yytext);
                       return ';';
                     }

"="                  {
//                       printf("\n***lex: term=%s;\n", yytext);
                       return '=';
                     }

[+-]                 {
//                       printf("\n***lex: term=%s;\n", yytext);
                       yylval=strdup(yytext); return ZNK;
                     }

[a-zA-Z][a-zA-Z0-9]* {
//                       printf("\n***lex: term=%s;\n", yytext);
                       if (!memcmp(yytext,"proc", 4)) return PROC;
                       if (!memcmp(yytext,"options", 7)) return OPTIONS;
                       if (!memcmp(yytext,"main", 4)) return MAIN;
                       if (!memcmp(yytext,"end", 3)) return END;
                       if (!memcmp(yytext,"dcl", 3)) return DCL;
                       if (!memcmp(yytext,"bin", 3)) return BIN;
                       if (!memcmp(yytext,"fixed", 3)) return FIXED;
                       if (!memcmp(yytext,"init", 3)) return INIT;
                       yylval=strdup(yytext); return IDENT;
                     }
[0-9][0-9]*          {
//                       printf("\n***lex: term=%s;\n", yytext);
                       yylval=strdup(yytext); return NUM;
                     }
[ \t\n]+             {
//                      printf("\n***lex: token is WightSpace\n");
                     }
%%
