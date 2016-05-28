PL_I:  PL_I.tab.c lex.yy.c
	gcc PL_I.tab.h PL_I.tab.c lex.yy.c

lex.yy.c: PL_I.l
	flex PL_I.l

PL_I.tab.c: PL_I.y
	bison -d PL_I.y

clear: 
	rm -f PL_I.tab.c PL_I.tab.h lex.yy.c PL_I PL_I.output Result.ass 