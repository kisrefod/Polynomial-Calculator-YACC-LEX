
all:
	make clean
	lex gram.lex
	yacc -d gram.y
	gcc lex.yy.c source.c polynom.h y.tab.c y.tab.h -o compiler
	
clean:
	rm -rf compiler y.tab.h y.tab.c lex.yy.c