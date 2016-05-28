%{
#include <stdio.h>
#include <string.h>

#define YYERROR_VERBOSE 1
#define YYSTYPE char*

int yylineno;

YYSTYPE yylval;

#define MAX_LENGTH_ASS_PROG 50

#define MAX_LENGTH_DCL_PART 20

#define MAX_LENGTH_IMP_PART 20

char s1[80];

char Prolog[3][80];
char Epilog[80];

char AssProg[MAX_LENGTH_ASS_PROG][80];
int pAssProg;

char DclPart[MAX_LENGTH_DCL_PART][80];
int pDclPart;

char ImpPart[MAX_LENGTH_IMP_PART][80];
int pImpPart;

char AssProgName[9];
char ErrorMessage[100];

int IsDclName( char* pName, int length) {
int i, j;
char etalon[9];

 for (i=0; i<pDclPart; i++) {

  j=0;
  while (DclPart[i][j] != ' ')  etalon[j++]=DclPart[i][j];
  etalon[j]=0;

  if (length != j) continue;

  if (memcmp(&DclPart[i][0], pName, length)) {
   continue;
  }
  else {
   return 0;
  }
 }
 return 1;
}

void yyerror(const char *str) {
 fprintf(stderr, "\n error: string N  %u, %s\n", yylineno, str);
}

void pro();
void odi(char *tpe, char *rzr, char *lit);
void odr(char *tpe, char *rzr);
void opr(char *pr_name);
int  oen(char *pr_name);
int  opa(char *ipe);
void avi_lit(char *lit);
int  avi_ipe(char *ipe);
void avi_avi_znk_lit(char *znk, char *lit);
int  avi_avi_znk_ipe(char *znk, char *ipe);

%}
%debug
%verbose
%token IDENT PROC OPTIONS MAIN END DCL BIN FIXED NUM INIT
%left ZNK
%start pro
%%
pro: opr tel oen { pro(); }
     ;
tel: dec imp
     ;
dec: odc
    | dec odc
     ;
odc: odi
    | odr
     ;
odi:  DCL ipe BIN FIXED '(' rzr ')' INIT '(' lit ')' ';' { odi($2, $6, $10); }
     ;
odr:  DCL ipe BIN FIXED '(' rzr ')' ';'                  { odr($2, $6); }
     ;
ipe: IDENT {$$=$1;}
     ;
rzr: NUM {$$=$1;}
     ;
lit: NUM {$$=$1;}
     ;
opr: IDENT ':' PROC OPTIONS '(' MAIN ')' ';'             { opr($1); }
     ;
oen: END IDENT ';'                                       { if ( oen($2) ) YYABORT; }
     ;
imp: opa
    | imp opa
     ;
opa: ipe '=' avi ';'                                     { if ( opa($1) ) YYABORT; }
     ;
avi: lit                                                 { avi_lit($1); }
    | ipe                                                { if ( avi_ipe($1) ) YYABORT;}
    | avi ZNK lit                                        { avi_avi_znk_lit($2, $3); }
    | avi ZNK ipe                                        { if ( avi_avi_znk_ipe($2, $3) ) YYABORT; }
     ;
%%
/*
***************************************************************************************************************
*                   Начало библиотеки семантических программ                                                  *
***************************************************************************************************************
*/

/*
...............................................................................................................
. Программа void pro()                                                                                        .
. Соединяет в массиве AssProg вместе следующие части ассемблеровского эквивалента:                            .
.  - пролог (из массива Prolog),                                                                              ,
.  - императивную часть (из массива ImpPart),                                                                 .
.  - декларативную часть (из массива DeclPart),                                                               .
.  - эпилог (из массива epilog).                                                                              .
. Распечатывает результат                                                                                     .
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
*/

void pro()
{int i,j;

                    memcpy(&AssProg[pAssProg++][0], &Prolog[0][0], 80);
                    memcpy(&AssProg[pAssProg++][0], &Prolog[1][0], 80);
                    memcpy(&AssProg[pAssProg++][0], &Prolog[2][0], 80);


                    for (i=0; i<pImpPart; i++) {
                     memcpy(&AssProg[pAssProg++][0], &ImpPart[i][0], 80);
                    }

                    for (i=0; i<pDclPart; i++) {
                     memcpy(&AssProg[pAssProg++][0], &DclPart[i][0], 80);
                    }

                    memcpy(&AssProg[pAssProg++], &Epilog[0], 80);

                    printf("\nR e s u l t   of   G e n e r a t i o n\n");
                    for (i=0; i<pAssProg; i++) {
                      printf("\n");
                      for (j=0; j<79; j++) {
                       printf("%c", AssProg[i][j]);
                      }
                    }

                    printf("\n");

                    if(yydebug) {

                     printf("\nDclPart");
                     for (i=0; i<pDclPart; i++) {
                      printf("\n");
                      for (j=0; j<79; j++) {
                       printf("%c", DclPart[i][j]);
                      }
                     }

                     printf("\n");

                     printf("\nImpPart");
                     for (i=0; i<pImpPart; i++) {
                      printf("\n");
                      for (j=0; j<79; j++) {
                       printf("%c", ImpPart[i][j]);
                      }
                     }

                     printf("\n");
                    }
}

/*
...............................................................................................................
. Программа void odi(char *ipe, char *rzr, char *lit)                                                         .
.                                                                                                             .
. Формирует и дописывает в свободную строку массива DclPart ассемблеровский эквивалент оператора ЯВУ,         .
. который  объявляет и инициализирует именованный объект (dcl с инициализацией).                              .
.                                                                                                             .
. Параметры: ipe (имя переменной), rzr (разрядность), lit (инициализирующее значение).                        .
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
*/

void odi(char *ipe, char *rzr, char *lit) {
               memset(&s1[0], ' ', 80);
               memcpy(&s1[0], ipe, strlen(ipe));
               memcpy(&s1[9], "DC", 2);
               if(!memcmp(rzr, "31", 2))
                s1[15]='F';
               else
                s1[15]='H';
               s1[16]='\'';
               memcpy(&s1[17], lit, strlen(lit));
               s1[17+strlen(lit)]='\'';
               memcpy(&s1[30], "Variable declaration with initialization", 40);

               memcpy(&DclPart[pDclPart][0], &s1[0], 80);
               pDclPart++;
              }

/*
...............................................................................................................
. Программа void odr(char *ipe, char *rzr)                                                                    .
.                                                                                                             .
. Формирует и дописывает в свободную строку массива DclPart ассемблеровский эквивалент оператора ЯВУ,         .
. который  объявляет именованный объект (dcl без инициализации).                                              .
.                                                                                                             .
. Параметры: ipe (имя переменной), rzr (разрядность).                                                         .
.                                                                                                             .
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
*/

void odr(char *ipe, char *rzr) {
               memset(&s1[0], ' ', 80);
               memcpy(&s1[0], ipe, strlen(ipe));
               memcpy(&s1[9], "DS", 2);
               if(!memcmp(rzr, "31", 2))
                s1[15]='F';
               else
                s1[15]='H';
               memcpy(&s1[30], "Variable declaration without initialization", 43);

               memcpy(&DclPart[pDclPart][0], &s1[0], 80);
               pDclPart++;
}

/*
...............................................................................................................
. Программа void opr(char *pr_name)                                                                           .
.                                                                                                             .
. Формирует и дописывает в свободную строку массива Prolog  ассемблеровский эквивалент оператора ЯВУ,         .
. который  объявляет пролог программы на ЯВУ (proc options ...).                                              .
.                                                                                                             .
. Параметры: pr_name (имя программы).                                                                         .
.                                                                                                             .
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
*/


void opr(char *pr_name) {
               memset(&s1[0], ' ', 80);
               memcpy(&s1[0], pr_name, strlen(pr_name));
               memcpy(&s1[9], "START 0", 7);
               memcpy(&s1[30], "Programm start", 14);
               memcpy(&Prolog[0][0], &s1[0], 80);

               memset(&s1[0], ' ', 80);
               memcpy(&s1[9], "BALR  RBASE,0", 13);
               memcpy(&s1[30], "Base initialization", 19);
               memcpy(&Prolog[1][0], &s1[0], 80);


               memset(&s1[0], ' ', 80);
               memcpy(&s1[9], "USING *,RBASE", 13);
               memcpy(&s1[30], "Base declaration", 16);
               memcpy(&Prolog[2][0], &s1[0], 80);

               memcpy(&AssProgName[0], pr_name, strlen(pr_name));
}

/*
...............................................................................................................
. Программа int oen(char *pr_name)                                                                            .
.                                                                                                             .
. Формирует и дописывает в массив Epilog ассемблеровский эквивалент оператора ЯВУ,                            .
. который  объявляет эпилог программы на ЯВУ (end ...).                                                       .
.                                                                                                             .
. Параметры: pr_name (имя программы).                                                                         .
.                                                                                                             .
. Возвращаемый результат 0 (если pr_name совпадает с pr_name пролога) и 1 (в противном случае)                .
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
*/

int oen(char *pr_name) {

               if (!memcmp(&Prolog[0][0], pr_name, strlen(pr_name))){

                memset(&s1[0], ' ', 80);
                memcpy(&s1[9], "END", 3);
                memcpy(&s1[30], "Programm end", 12);
                memcpy(&Epilog[0], &s1[0], 80);

                memset(&s1[0], ' ', 80);
                memcpy(&s1[9], "BCR   15,RVIX", 13);
                memcpy(&s1[30], "Return from programm", 20);

                memcpy(&ImpPart[pImpPart][0], &s1[0], 80);
                pImpPart++;
                return 0; 
               }
               else {
                strcpy(&ErrorMessage[0], " invalid identificator ");
                strcat(&ErrorMessage[0], pr_name);
                strcat(&ErrorMessage[0], " ");
                strcat(&ErrorMessage[0], "in oen\n");
                yyerror(&ErrorMessage[0]);
                return 1;
               }
}

/*
...............................................................................................................
. Программа int opa(char *ipe)                                                                                .
.                                                                                                             .
. Формирует и дописывает в в свободную строку массива ImpPart ассемблеровский эквивалент оператора ЯВУ,       .
. который  присваивает переменной ipe значение вычисленного арифметического выражения, находящегося в регистре.
. RRAB  (имя перемеменной =  ...).                                                                            .
.                                                                                                             .
. Параметры: pr_name (имя программы).                                                                         .
.                                                                                                             .
. Возвращаемый результат 0 (если ipe объявлена ранее в операторе dcl) и 1 (в противном случае)                .
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
*/

int opa(char *ipe) {
                if (IsDclName(ipe, strlen(ipe))){
                strcpy(&ErrorMessage[0], " invalid identificator ");
                strcat(&ErrorMessage[0], ipe);
                strcat(&ErrorMessage[0], " ");
                strcat(&ErrorMessage[0], "in left part of opa\n");
                yyerror(&ErrorMessage[0]);
                return 1;
                }
                memset(&s1[0], ' ', 80);
                memcpy(&s1[9], "ST", 2);
                memcpy(&s1[15], "RRAB,", 5);
                memcpy(&s1[20], ipe, strlen(ipe));
                memcpy(&s1[30], "Result storage", 14);

                memcpy(&ImpPart[pImpPart][0], &s1[0], 80);
                 pImpPart++;

                return 0;
}

/*
...............................................................................................................
. Программа void avi_lit(char *lit)                                                                           .
.                                                                                                             .
. Формирует и дописывает в в свободную строку массива ImpPart ассемблеровский эквивалент арифметического      .
. выражения - правой части оператора присваивания ЯВУ (имя переменной = аифм.выражение), когда арифметическое .
. выражение представлено литералом.                                                                           .
.                                                                                                             .
. Параметры: lit (литерал).                                                                                   .
.                                                                                                             .
.....,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
*/

void avi_lit(char *lit) {
                memset(&s1[0], ' ', 80);
                memcpy(&s1[9], "L", 1);
                memcpy(&s1[15], "RRAB,=F\'", 8);
                memcpy(&s1[23], lit, strlen(lit));
                memcpy(&s1[23+strlen(lit)], "\'", 1);
                memcpy(&s1[30], "Literal loading", 15);

                memcpy(&ImpPart[pImpPart][0], &s1[0], 80);
                pImpPart++;
}

/*
...............................................................................................................
. Программа int avi_ipe(char *ipe)                                                                            .
.                                                                                                             .
. Формирует и дописывает в в свободную строку массива ImpPart ассемблеровский эквивалент арифметического      .
. выражения - правой части оператора присваивания ЯВУ (имя переменной = аифм.выражение), когда арифметическое .
. выражение представлено именем переменной ipe.                                                               .
.                                                                                                             .
. Параметры: ipe (имя переменной).                                                                            .
.                                                                                                             .
. Возвращаемый результат 0 (если ipe объявлена ранее в операторе dcl) и 1 (в противном случае)                .
.                                                                                                             .
.....,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
*/

int  avi_ipe(char *ipe) {
                if (IsDclName(ipe, strlen(ipe))){
                strcpy(&ErrorMessage[0], " invalid identificator ");
                strcat(&ErrorMessage[0], ipe);
                strcat(&ErrorMessage[0], " ");
                strcat(&ErrorMessage[0], "in avi of opa\n");
                yyerror(&ErrorMessage[0]);
                return 1;
                }
                memset(&s1[0], ' ', 80);
                memcpy(&s1[9], "L", 1);
                memcpy(&s1[15], "RRAB,", 5);
                memcpy(&s1[20], ipe, strlen(ipe));
                memcpy(&s1[30], "Variable value loading", 22);

                memcpy(&ImpPart[pImpPart][0], &s1[0], 80);
                pImpPart++;

                return 0;
}

/*
...............................................................................................................
. Программа void avi_avi_znk_lit(char *lit)                                                                   .
.                                                                                                             .
. Формирует и дописывает в в свободную строку массива ImpPart ассемблеровский эквивалент арифметического      .
. выражения - правой части оператора присваивания ЯВУ (имя переменной = аифм.выражение), когда арифметическое .
. выражение представлено как леворекурсивное с литералом справа.                                              .
.                                                                                                             .
. Параметры: znk (знак + или -) lit (литерал).                                                                .
.                                                                                                             .
.                                                                                                             .
.....,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
*/

void avi_avi_znk_lit(char *znk, char *lit) {
                memset(&s1[0], ' ', 80);
                if(!memcmp(znk, "+", 1)) {
                 memcpy(&s1[9], "A", 1);
                 memcpy(&s1[30], "Literal\'s value adding", 22);
                }
                if(!memcmp(znk, "-", 1)) {
                 memcpy(&s1[9], "S", 1);
                 memcpy(&s1[30], "Literal\'s value substracting", 28);
                }
                memcpy(&s1[15], "RRAB,=F\'", 8);
                memcpy(&s1[23], lit, strlen(lit));
                memcpy(&s1[23+strlen(lit)], "\'", 1);

                memcpy(&ImpPart[pImpPart][0], &s1[0], 80);
                pImpPart++;
}

/*
...............................................................................................................
. Программа int avi_avi_znk_ipe(char *znk, char *ipe)                                                         .
.                                                                                                             .
. Формирует и дописывает в в свободную строку массива ImpPart ассемблеровский эквивалент арифметического      .
. выражения - правой части оператора присваивания ЯВУ (имя переменной = аифм.выражение), когда арифметическое .
. выражение представлено как леворекурсивное с именем переменной справа.                                      .
.                                                                                                             .
. Параетры: lit (литерал).                                                                                    .
.                                                                                                             .
. Возвращаемый результат 0 (если ipe объявлена ранее в операторе dcl) и 1 (в противном случае)                .
.                                                                                                             .
.....,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
*/

int  avi_avi_znk_ipe(char *znk, char *ipe) {
                if (IsDclName(ipe, strlen(ipe))){
                strcpy(&ErrorMessage[0], " invalid identificator ");
                strcat(&ErrorMessage[0], ipe);
                strcat(&ErrorMessage[0], " ");
                strcat(&ErrorMessage[0], "in avi of opa\n");
                yyerror(&ErrorMessage[0]);
                return 1;
                }
                memset(&s1[0], ' ', 80);
                if(!memcmp(znk, "+", 1)) {
                 memcpy(&s1[9], "A", 1);
                 memcpy(&s1[30], "Variable\'s value adding", 23);
                }
                if(!memcmp(znk, "-", 1)) {
                 memcpy(&s1[9], "S", 1);
                 memcpy(&s1[30], "Variable\'s value substracting", 29);
                }
                memcpy(&s1[15], "RRAB,", 5);
                memcpy(&s1[20], ipe, strlen(ipe));

                memcpy(&ImpPart[pImpPart][0], &s1[0], 80);
                pImpPart++;

                return 0;
}

/*
***************************************************************************************************************
*                   Конец  библиотеки семантических программ                                                  *
***************************************************************************************************************
*/

/*
...............................................................................................................
. Программа int yywrap()                                                                                      .
.                                                                                                             .
. Вызывается по достижении EOF входного потока                                                                .
.                                                                                                             .
. Возвращаемый результат 0 (если работа д.б. продолжена) и 1 (в противном случае)                             .
.                                                                                                             .
.....,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
*/

int yywrap() {
 return 1;
}

/*
...............................................................................................................
. Программа int main()                                                                                        .
.                                                                                                             .
. Главная программа. Начинает работу, проводит начальную инициализацию, обращается к парсеру (yyparse) и      .
. при отсутствии ошибок разгружает результат компиляции из массива AssProg в выходной файл Results.ass,       .
. а в случае преждевременного завершения парсера - выдает аварийное сообщение                                 .
.                                                                                                             .
. Возвращаемый результат 0 (компиляция успешна) и 1 (в противном случае)                                      .
.                                                                                                             .
.....,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
 pAssProg=0;
 memset(&DclPart[0][0], ' ', 80);
 memcpy(&DclPart[0][0], "RBASE    EQU   5", 16);
 memset(&DclPart[1][0], ' ', 80);
 memcpy(&DclPart[1][0], "RVIX     EQU   14", 17);
 memset(&DclPart[2][0], ' ', 80);
 memcpy(&DclPart[2][0], "RRAB     EQU   3", 16);
 pDclPart=3;
 pImpPart=0;
// yydebug=1;
 if (!yyparse()) {
  int fn, i;
  if ( 0 >= (fn=open("Result.ass", O_CREAT|O_WRONLY|O_TRUNC, S_IWUSR))) {
   printf("\n*** error it is not impossible to open Result.ass file\n");
  }
  else {
   for (i=0; i<pAssProg; i++) {
    if (80 != write(fn, &AssProg[i][0], 80)){
     printf("\n*** error during writing Result.ass file\n");
     break;
    }
   }
   close(fn);
   printf("\n*** Compilation is successfull\n");
  }
 return 0;
 }
 else {
  printf("\n*** Compilation is not successfull\n");
 }
 return 1;
}
