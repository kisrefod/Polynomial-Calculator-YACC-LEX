#define _CRT_SECURE_NO_WARNINGS

#include "polynom.h"

extern int gLineNum;
#define bool char
#define false 0
#define true 1

struct Variable* gVariableList = NULL;

void addVariableToList(char *letter, struct Polynom polynom)
{
	struct Variable* var = (struct Variable *)malloc(sizeof(struct Variable));
	var->variable = (char*) malloc(32);
	strncpy(var->variable, letter, 32);
	var->polynom = polynom;
	var->next = NULL;
	var->prev = NULL;

	if (gVariableList == NULL)
	{
		gVariableList = var;
		return;
	}

	struct Variable *varList = gVariableList;
	for (; varList->next != NULL; varList = varList->next)
	{
		if (!strcmp(varList->variable, letter))
		{
			varList->polynom = polynom;
			return;
		}
	}

	if (!strcmp(varList->variable, letter))
	{
		varList->polynom = polynom;
		return;
	}

	var->prev = varList;
	varList->next = var;
}

struct Polynom* getVariable(char *variable)
{
	struct Variable* varList = gVariableList;
	while (varList != NULL)
	{
		if (!strcmp(varList->variable, variable))
		{
			return &(varList->polynom);
		}
		varList = varList->next;
	}
	printError("Not initialize variable ", variable);
}

struct Mononom* setMononom(int coefficient, char *letter, int power)
{
	struct Mononom *result = (struct Mononom *)malloc(sizeof(struct Mononom));

	result->coefficient = coefficient;

	if ((strcmp(letter, "") && power == 0) || coefficient == 0)
	{
		memset(result->variable, 0, 32);
		result->power = 0;
		return result;
	}

	strncpy(result->variable, letter, 32);
	result->power = power;
	return result;
}

struct Polynom* initPolynom(void)
{
	struct Polynom *result = (struct Polynom *)malloc(sizeof(struct Polynom));

	result->begin = (struct Node *) malloc(sizeof(struct Node));
	result->begin->prev = NULL;
	result->begin->next = NULL;
	result->count = 0;

	return result;
}

struct Polynom* addMononomToPolynom(struct Polynom polynom, struct Mononom mononom)
{
	struct Node * node;
	struct Polynom *result = (struct Polynom *)malloc(sizeof(struct Polynom));

	node = polynom.begin;

	if (polynom.count == 0)
	{
		node->item = mononom;
		polynom.count++;
		(*result) = polynom;
		return result;
	}

	while (node->next != NULL)
	{
		node = node->next;
	}

	node->next = (struct Node *)malloc(sizeof(struct Node));
	node->next->prev = node;
	node->next->next = NULL;
	node = node->next;
	node->item = mononom;

	polynom.count++;
	(*result) = polynom;
	return result;
}

struct Node * deleteNode(struct Polynom *polynom, struct Node *node)
{
	struct Node * result = node;

	if (polynom->begin == node)
	{
		if (node->next == NULL)
		{
			free(node);
			return NULL;
		}

		node->next->prev = NULL;
		result = node->next;
		free(node);

		polynom->begin = result;
		return result;
	}

	if (node->next == NULL)
	{
		node->prev->next = NULL;
		result = node->prev;

		free(node);
		return result;
	}

	node->next->prev = node->prev;
	node->prev->next = node->next;
	result = node->prev;
	free(node);
	return result;
}

struct Polynom * deleteSimilarSummands(struct Polynom polynom)
{
	struct Polynom * result = initPolynom();
	for (struct Node *i = polynom.begin; i != NULL;)
	{
		struct Mononom mononom = i->item;
		for (struct Node *j = polynom.begin; j != NULL; j = j->next)
		{
			if (strncmp(i->item.variable, j->item.variable, 32) == 0 &&
				i->item.power == j->item.power &&
				i != j)
			{
				mononom.coefficient += j->item.coefficient;
				if (mononom.coefficient == 0)
				{
					mononom.power = 0;
					strncpy(mononom.variable, "", 32);
				}

				j = deleteNode(&polynom, j);
			}
		}

		i = deleteNode(&polynom, i);
		struct Polynom *freePtr = result;
		result = addMononomToPolynom(*result, mononom);
		free(freePtr);
	}

	return result;
}

struct Polynom* sumPolynoms(struct Polynom p1, struct Polynom p2)
{
	for (struct Node* i = p1.begin; i != NULL; i = i->next)
	{
		char varI[32] = {0};
		strncpy(varI, i->item.variable, 32);
		for (struct Node* j = p2.begin; j != NULL; j = j->next)
		{
			char varJ[32] = {0};
			strncpy(varJ, j->item.variable, 32);

			if (strncmp(varI, varJ, 32) != 0 && strncmp(varI, "", 32) != 0 && strncmp(varJ, "", 32) != 0)//Если переменные не совпадают и ни одна из переменных не является числом, то ошибка
			{
				printError("different variables in polynom", "");
			}
		}
	}

	struct Polynom *result = (struct Polynom *)malloc(sizeof(struct Polynom));
	(*result) = p1;
	struct Polynom *freePtr;
	for (struct Node *node = p2.begin; node != NULL; node = node->next)
	{
		freePtr = result;
		result = addMononomToPolynom(*result, node->item);
		free(freePtr);
	}

	freePtr = result;
	result = deleteSimilarSummands(*result);
	free(freePtr);
	return result;
}

struct Polynom* unaryMinus(struct Polynom *polynom)
{
	for (struct Node *node = polynom->begin; node != NULL; node = node->next)
	{
		node->item.coefficient *= (-1);
	}

	struct Polynom * result = deleteSimilarSummands(*polynom);
	return result;
}

struct Polynom * subPolynoms(struct Polynom p1, struct Polynom p2)
{
	for (struct Node *i = p1.begin; i != NULL; i = i->next)
	{
		for (struct Node *j = p2.begin; j != NULL; j = j->next)
		{
			char varI[32] = {0};
			strncpy(varI, i->item.variable, 32);
			char varJ[32] = {0};
			strncpy(varJ, j->item.variable, 32);

			if (strncmp(varI, varJ, 32) != 0 && strncmp(varI, "", 32) != 0 && strncmp(varJ, "", 32) != 0) //Если переменные не совпадают и ни одна из переменных не является числом, то
			{
				printError("different variables in polynom", "");
			}
		}
	}

	struct Polynom *minusP2 = unaryMinus(&p2);
	struct Polynom * result = sumPolynoms(p1, *minusP2);
	result = deleteSimilarSummands(*result);

	return result;
}

struct Polynom * mulPolynoms(struct Polynom p1, struct Polynom p2)
{
	struct Polynom * result = initPolynom();
		
	for (struct Node *i = p1.begin; i != NULL; i = i->next)
	{
		for (struct Node *j = p2.begin; j != NULL; j = j->next)
		{
			struct Mononom mononom = i->item;

			mononom.coefficient *= j->item.coefficient;

			bool IisNumber = (strncmp(mononom.variable, "", 32) == 0);
			bool JisNumber = (strncmp(j->item.variable, "", 32) == 0);
			bool varMatch = (strcmp(mononom.variable, j->item.variable) == 0);

			if (!IisNumber && varMatch) //у обоих одинаковая переменная
			{
				mononom.power += j->item.power;
			}
			else if (IisNumber && !JisNumber)//У мононома нет переменной, у второго есть
			{
				strncpy(mononom.variable, j->item.variable, 32);
				mononom.power += j->item.power;
				}
			else if (!IisNumber && !JisNumber && !varMatch)//у обоих разные переменные
			{
				printError("different variables in polynom", "");
			}

			if (mononom.coefficient == 0)
			{
				strncpy(mononom.variable, "", 32);
				mononom.power = 0;
			}

			struct Polynom *freePtr = result;
			result = addMononomToPolynom(*result, mononom);
			free(freePtr);
		}
	}

	struct Polynom *freePtr = result;
	result = deleteSimilarSummands(*result);
	free(freePtr);
	return result;
}

void printMononom(struct Mononom *mononom)
{
	if (!strcmp(mononom->variable, ""))
	{
		printf("%d", mononom->coefficient);
	}
	else
	{
		if (abs(mononom->coefficient) == 1)
		{
			if (mononom->coefficient == -1)
			{
				printf("-");
			}
			if (mononom->power == 1)
			{
				printf("%s", mononom->variable);
			}
			else
			{
				printf("%s^%d", mononom->variable, mononom->power);
			}
		}
		else
		{
			if (mononom->power == 1)
			{
				printf("%d%s", mononom->coefficient, mononom->variable);
			}
			else
			{
				printf("%d%s^%d", mononom->coefficient, mononom->variable, mononom->power);
			}
		}
	}
}

void printPolynom(struct Polynom *polynom)
{
	struct Node *begin = polynom->begin;
	printf("= ");
	
	while (begin->item.coefficient == 0 && begin->next != NULL)
	{
		begin = begin->next;
	}

	int itemNum = 0;
	for (struct Node *node = begin; node != NULL; node = node->next)
	{
		itemNum++;
	}

	bool *itemWasPrinted = (bool *)calloc(sizeof(bool), itemNum);

	char varName[32] = { 0 };
	for (struct Node *node = begin; node != NULL; node = node->next)
	{
		if (strncmp(node->item.variable, "", 32) != 0)//Если это не число
		{
			strncpy(varName, node->item.variable, 32);	
		}
	}

	bool firstWasPrinted = false;
	//Вывести все одночлены с этой переменной
	for (int i = 0; i < itemNum; i++)
	{
		//Найти самую старшую невыведенную степень
		int maxPower = 0;
		int currentItemIndex = 0;
		int itemIndex = 0;
		struct Node *result = begin;
		for (struct Node *node = begin; node != NULL; node = node->next)
		{
			if (strncmp(node->item.variable, varName, 32) == 0 && //Если совпадает имя переменной
				!itemWasPrinted[currentItemIndex] &&					  //Эта переменная не была выведена
				node->item.power > maxPower)					  //Cтепень выше других
			{
				result = node;
				maxPower = node->item.power;
				itemIndex = currentItemIndex;
			}

			currentItemIndex++;
		}

		if (maxPower != 0)//Если был найден хоть один подходящий элемент, то
		{
			//Вывести найденный элемент
			if (firstWasPrinted && result->item.coefficient > 0)//Если уже были выведены значения и коэффициент положительный
			{
				printf("+");
			}

			firstWasPrinted = true;
			printMononom(&result->item);
			itemWasPrinted[itemIndex] = true;
		}
		else
		{
			break;//Если нового невыведенного элемента не было найдено, то можно перейти к следующей переменной
		}
	}
	
	//Вывести константное значение
	for (struct Node *node = begin; node != NULL; node = node->next)
	{
		if (strncmp(node->item.variable, "", 32) == 0 && node->item.coefficient != 0)
		{
			if (firstWasPrinted && node->item.coefficient > 0) //Если уже были выведены значения и коэффициент положительный
			{
				printf("+");
			}

			firstWasPrinted = true;
			printMononom(&node->item);
			break;
		}
	}

	//Вывести 0, если совсем ничего не было выведено
	if (!firstWasPrinted)
	{
		printf("0");
	}
	printf(".\n");
}

#ifdef __unix__
	#include "y.tab.h"
#elif defined _WIN32 
	#include "y_tab.h"
#endif

	void yyerror(char const *s)
{
	printf("ERROR in line %d: '%s'\n", gLineNum + 1, s);
}

void printError(const char *s1, const char *s2)
{
	printf("ERROR in line %d: %s%s\n", gLineNum + 1, s1, s2);
	exit(-1);
}