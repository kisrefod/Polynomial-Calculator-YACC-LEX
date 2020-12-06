#ifndef POLYC_H_
#define POLYC_H_

#include <stdio.h>	
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int	gLineNum;

struct Mononom
{
	int coefficient;
	char variable[32];
	int power;
};

struct Polynom
{
	struct Node* begin;
	int count;
};

struct Node
{
	struct Mononom item;
	struct Node* next;
	struct Node* prev;
};

struct Variable
{
	char* variable;	
	struct Polynom polynom; 
	struct Variable* next;
	struct Variable* prev;
};

void printError	(const char *, const char *);
void addVariableToList (char *, struct Polynom );
struct Polynom* getVariable (char *);
struct Mononom* setMononom (int, char *, int);
struct Polynom* initPolynom (void);
struct Polynom* addMononomToPolynom (struct Polynom, struct Mononom);
struct Node* deleteNode (struct Polynom *, struct Node *);
struct Polynom* deleteSimilarSummands (struct Polynom);
struct Polynom* sumPolynoms (struct Polynom, struct Polynom);
struct Polynom* subPolynoms (struct Polynom, struct Polynom);
struct Polynom* mulPolynoms (struct Polynom, struct Polynom);
struct Polynom* unaryMinus (struct Polynom *);
void printMononom (struct Mononom *);
void printPolynom (struct Polynom *);
#endif