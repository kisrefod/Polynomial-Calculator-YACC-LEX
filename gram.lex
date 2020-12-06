%{	
	#include "polynom.h"
	#include "y.tab.h"
	#include<stdio.h>
	
	extern int yylex();

	FILE *inputStream;

	#define yyin inputStream
	#define tmp_scanf(f_, ...) fscanf(yyin, (f_), __VA_ARGS__)

	int main(int argc, char* argv[])
	{
		inputStream = fopen(argv[1], "r");
		
		if (inputStream == NULL)
		{
			printf("File wasn't found\n");
			return -1;
		}

		yyparse();

		fclose(inputStream);
	}
%}

%option yylineno
%option noyywrap
%%
[/][/].*      ;
[ \t\r]       ;
[\n] {gLineNum++; return EOL;}
[';'] {return EOC;}//End of command
['^'] {return POW;}
[=] {return EQ;}
['$'] {return DOLLAR;}
[+]	{return PLUS;}
[-]	{return MINUS;}
[(]	{return LB;}
[)]	{return RB;}
[0-9]+ {yylval.num = atoi(yytext); return DIGIT;}
[*] {return MUL;}
[/] {return DIV;}
(PRINT|print)+ {return PRINT;}
[a-zA-Z]+ {memset(yylval.str, 0, 32); strncpy(yylval.str, yytext, 32); return STRING;}
%%
