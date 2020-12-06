%{
	#include "polynom.h"
			
	void yyerror(char const *s);
%}

%union
{
	struct Polynom* poly;
	struct Mononom*	mono;
	int 				num;
	char 				str[32];
}

%type <str>		variable
%type <poly> 	plusminusPriority muldivPriority powPriority firstPriority
%type <mono> 	mononom

%token <num> 	DIGIT
%token <str> 	STRING PRINT

%left '+' '-'
%left '*'
%right UMINUS
%right '^'

%start start
%token POW EQ DOLLAR PLUS MINUS LB RB MUL DIV COMMENT EOC EOL
%%

start:
	//empty
	| start EOC
	| start EOL
	| start '\r'
	| start command
	;
command:
	variable EQ plusminusPriority EOC
	{
		addVariableToList($1, *$3);
		free($3);
	}
	| PRINT variable EOC
	{
		struct Polynom *tmp;
		printf("$%s ", $2);
		tmp = getVariable($2);
		printPolynom(tmp);
	}
	| PRINT plusminusPriority EOC
	{
		printf("Expression ");
		printPolynom($2);
		free($2);
	}
//Errors
	| variable EQ plusminusPriority EOL
	{
		printError("forgot ';'", "");
	}
	| PRINT variable EOL
	{
		printError("forgot ';'", "");
	}
	| PRINT plusminusPriority EOL
	{
		printError("forgot ';'", "");
	}
	;
plusminusPriority:
	plusminusPriority PLUS muldivPriority
	{
		$$ = sumPolynoms(*$1, *$3);
		free($1);
		free($3);
	}
	| plusminusPriority MINUS muldivPriority
	{		
		$$ = subPolynoms(*$1, *$3);
		$$ = deleteSimilarSummands(*$$);
		free($1);
		free($3);
	}
	| MINUS plusminusPriority %prec UMINUS
	{
		$$ = unaryMinus($2);
	}
	| muldivPriority
	{
		$$ = $1;
	}
	| plusminusPriority PLUS PLUS
	{
		free($1);
		printError("excess +","");
	}
	;
muldivPriority:
	muldivPriority MUL powPriority
	{
		$$ = mulPolynoms(*$1, *$3);
		free($1);
		free($3);
	}
	| muldivPriority powPriority
	{
		$$ = mulPolynoms(*$1, *$2);
		free($1);
		free($2);
	}
	| powPriority
	{
		$$ = $1;
	}
//ERRORS
	| muldivPriority MUL MUL
	{
		free($1);
		printError("excess *","");
	}
	| LB MUL muldivPriority
	{
		free($3);
		printError("excess *","");
	}
	| muldivPriority MUL RB
	{
		free($1);
		printError("excess *","");
	}
	;
powPriority:
	powPriority POW firstPriority
	{
		if (strcmp($3->begin->item.variable, "") != 0)
		{
			printError("incorrect pow: ^", $3->begin->item.variable);
			free($1);
			free($3);
			return -1;
		}
		
		int pow = $3->begin->item.coefficient;
		if (pow == 0)
		{
			if ($1->begin->item.coefficient == 0)
			{
				printError("uncertainty (0^0)", "");
			}

			$$ = initPolynom();
			struct Mononom *mono = setMononom(1, "", 0);
			$$ = addMononomToPolynom(*$$, *mono);
			free(mono);
			free($1);
			free($3);
		}
		else if (pow == 1)
		{
			$$ = $1;
			free($3);
		}
		else if (pow > 1)
		{
			$$ = $1;
			for (int i = 0; i < pow - 1; i++)
			{
				$$ = mulPolynoms(*$$, *$1);
			}
			free($1);
			free($3);
		}
		else
		{
			printError("power cannot be < 0", "");
		}
	}
	| firstPriority
	{
		$$ = $1;
	}
//ERRORS
	| powPriority POW POW
	{
		free($1);
		printError("incorrect ^","");
	}
	| powPriority POW MINUS
	{
		free($1);
		printError("incorrect ^-","");
	}
firstPriority:
	mononom
	{
		$$ = initPolynom();
		$$ = addMononomToPolynom(*$$, *$1);
		free($1);
	}
	| variable
	{ 
		struct Polynom * poly;
		poly = initPolynom();
		struct Mononom *mono = setMononom(1, "", 0);
		poly = addMononomToPolynom(*poly, *mono);
		free(mono);

		$$ = getVariable($1); 

		$$ = mulPolynoms(*$$, *poly);
		free(poly);
	}
	| LB plusminusPriority RB
	{
		$$ = $2;
	}
	;
mononom:	
	DIGIT
	{
		$$ = setMononom($1, "", 0);
	}
	| STRING
	{
		$$ = setMononom(1, $1, 1);
	}
	;
variable:
	DOLLAR STRING
	{
		strncpy($$, $2, 32);
	}
	;
%%
