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
                       if (!strcasecmp(yytext,"proc")) return PROC;
                       if (!strcasecmp(yytext,"options")) return OPTIONS;
                       if (!strcasecmp(yytext,"main")) return MAIN;
                       if (!strcasecmp(yytext,"end")) return END;
                       if (!strcasecmp(yytext,"dcl")) return DCL;
                       if (!strcasecmp(yytext,"bin")) return BIN;
                       if (!strcasecmp(yytext,"dec")) return DEC;
                       if (!strcasecmp(yytext,"fixed")) return FIXED;
                       if (!strcasecmp(yytext,"init")) return INIT;
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
