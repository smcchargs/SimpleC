#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Keyword Functions
void readTokens(FILE* fin);
void printFunction(FILE* fin);
void intFunction(FILE* fin);
void readFunction(FILE* fin);
void symbolFunction(FILE* fin, int leftSide);
void ifFunction(FILE* fin);
void elseFunction(FILE* fin);
void whileFunction(FILE* fin);
void returnFunction(FILE* fin);
void printName(FILE *fin);

// Functions simply write to the file the appropriate information for each operation
void addFunc(char* var1, char* var2);
void subFunc(char* var1, char* var2);
void mulFunc(char* var1, char* var2);
void divFunc(char* var1, char* var2);
void modFunc(char* var1, char* var2);
void printConst(int constant);
void checkOp(char* var1, char* var2, char operation);
void allocateVar();
void readVar(char* symbol);
void loadVar(char* symbol);
void storeVar(char* varNum, char* symbol);
void eqFunc(char* bool1, char* bool2);
void andFunc(char* bool1, char* bool2);
void orFunc(char* bool1, char* bool2);
void notFunc(char* bool1);
void neqFunc(char* bool1, char* bool2);
void ltFunc(char* bool1, char* bool2);
void gtFunc(char* bool1, char* bool2);
void printCall(char* name);

// Calculating Functions
char* deleteArray(char* array);
char* getPostFix(char *infix);
void eval(char* postfix);
int isOperand(char ch);
int isOperator(char ch);
int precedence(char ch);
char* formatString(char* expression);
int isConstant(char* expression);


// Prints the template.ll format for compiler
void printTemplate();
void printMain();
void finishTemplate();

// Stack for strings
struct Node
{
	char* string;
	struct Node* next;
};

char* popS(struct Node** stack);
struct Node* pushS(struct Node* head, char* string);
char* peekS(struct Node** stack);
int isEmptyS(struct Node* stack);

// Hashtable of strings for symbol table
struct DataItem
{
	char* data;
	char* key;
};

struct DataItem* symbolTable[101];
struct DataItem* emptyItem; 

int hashCode(char* key);
struct DataItem* search(char* key);
void insert(char* key, char* data);
struct DataItem* delete(struct DataItem* item);

int varNum = 1;
int labelNum = 1;



int main(int argc, char *argv[])
{
	// File to read simple c code from
	FILE *fin = fopen(argv[1], "r");
	if (fin == NULL)
	{
		printf("File is NULL");
		return 0;
	}

	printTemplate();
	readTokens(fin);

	finishTemplate();
	fclose(fin);

	return 0;
}


void readTokens(FILE* fin)
{

	char* c = malloc(1000 * sizeof(char));
	int i = 0;

	while(1)
	{
		if (feof(fin))
			break;

		c[i] = fgetc(fin);
		 printf("%s\n", c);

		if (c[i] == '\n')
			continue;
		if (c[i] == ' ')
			continue;
		if (c[i] == '\t')
			continue;


		if (strcmp(c, "print") == 0)
		{
			printFunction(fin);
			i = 0;
			c = deleteArray(c);
			continue;
		}

		else if (strcmp(c, "int") == 0)
		{
			intFunction(fin);
			i = 0;
			c = deleteArray(c);
			continue;
		}

		else if (strcmp(c, "read") == 0)
		{
			readFunction(fin);
			i = 0;
			c = deleteArray(c);
			continue;
		}

		else if (search(c) != NULL)
		{
			symbolFunction(fin, hashCode(c));
			i = 0;
			c = deleteArray(c);
			continue;	
		}

		else if (strcmp(c, "if") == 0)
		{
			ifFunction(fin);
			i = 0;
			c = deleteArray(c);
			continue;	
		}

		else if (strcmp(c, "else") == 0)
		{
			elseFunction(fin);
			i = 0;
			c = deleteArray(c);
			continue;	
		}

		else if (strcmp(c, "while") == 0)
		{
			whileFunction(fin);
			i = 0;
			c = deleteArray(c);
			continue;		
		}

		else if (strcmp(c, "return") == 0)
		{
			returnFunction(fin);
			i = 0;
			c = deleteArray(c);
			continue;		
		}

		i++;
	}

	free(c);
	return;
}


void printFunction(FILE* fin)
{
	char* c = malloc(1000 * sizeof(char));
	int i = 0;

	while (1)
	{
		c[i] = fgetc(fin);
		
		// ignore new lines and spaces
		if (c[i] == '\n')
			continue;
		if (c[i] == ' ')
			continue;

		if (c[i] == ';')
		{
			c[i] = '\0';


			if (i == 1 && search(c) == NULL)
			{
				if (!isdigit(c[0]) && search(c) == NULL)
				{
					fprintf(stderr, "error: use of undeclared variable %s\n", c);
					exit(-1);
				}
				printConst((c[0] - '0'));
				break;
			}

			if (search(c) != NULL)
			{
				loadVar(search(c)->data);
				printf("   call void @print_integer(i32 %%t%d)\n", (varNum));
				varNum++;
				break;
			}

			eval(c);
			printf("   call void @print_integer(i32 %%t%d)\n", (varNum - 1));
			break;

		}
		
		i++;
	}

	free(c);
	return;
}

void intFunction(FILE* fin)
{

	char* c = malloc(1000 * sizeof(char));
	int i = 0;
	char* temp = malloc(21 * sizeof(char));
	char* temp2 = malloc(21 * sizeof(char));

	while (1)
	{
		c[i] = fgetc(fin);

		if (c[i] == '\n')
					continue;
		if (c[i] == ' ')
					continue;

		if (c[i] == ';')
		{
			c[i] = '\0';
			temp = strcpy(temp, "%t");
			sprintf(temp2, "%d", varNum);
			temp = strcat(temp, temp2);
			
			if (search(c) != NULL)
			{
				fprintf(stderr, "error: multiple definitions of %s\n", c);
				exit(-1);
			}
			insert(c, temp);
			allocateVar(varNum);
			varNum++;
			break;
		}

		else if (c[i] == '(')
		{
			c[i] = '\0';
			printf("define i32 @%s(i32", c);
			insert(c, c);
			printName(fin);
			break;
		}

		i++;
	}

	free(temp);
	free(temp2);
	free(c);
	return;
}

void readFunction(FILE* fin)
{
	char* c = malloc(1000 * sizeof(char));
	int i = 0;

	while (1)
	{
		c[i] = fgetc(fin);

		if (c[i] == '\n')
			continue;
		if (c[i] == ' ')
			continue;

		if (c[i] == ';')
		{
			c[i] = '\0';

			if (search(c) == NULL)
			{
				fprintf(stderr, "error: use of undeclared variable %s\n", c);
				exit(-1);
			}

			readVar(search(c)->data);
			varNum++;
			break;
		}

		i++;
	}

	free(c);
	return;
}

void symbolFunction(FILE* fin, int leftSide)
{
	char* c = malloc(1000 * sizeof(char));
	char *assignVar = malloc(100 * sizeof(char));
	char* temp = malloc(21 * sizeof(char));
	char* temp2 = malloc(21 * sizeof(char));

	int i = 0;

	while(1)
	{
		c[i] = fgetc(fin);

		if (c[i] == '\n')
			continue;
		if (c[i] == ' ')
			continue;

		if (c[i] == '=')
		{
			assignVar = symbolTable[leftSide]->data;
			c = deleteArray(c);
			i = 0;
			continue;
		}

		if (c[i] == ';')
		{
			c[i] = '\0';
			if (isConstant(c))
			{
				int k = 0;
				char* cons = malloc(100 * sizeof(char));

				// for loop just to remove parenthesis for some test cases
				for (int j = 0; c[j]; j++)
				{
					if (c[k] == ')' || c[k] == '(')
						k++;
					cons[j] = c[k++];
				}

				storeVar(cons, assignVar);
				break;
			}
					
			eval(c);

			temp = strcpy(temp, "%t");
			sprintf(temp2, "%d", (varNum-1));
			temp = strcat(temp, temp2);

			storeVar(temp, assignVar);
			break;
		}

		i++;
	}

	free(assignVar);
	free(c);
	return;
}

void ifFunction(FILE* fin)
{
	char* c = malloc(1000 * sizeof(char));
	int i = 0;

	int openP = 0;
	int closeP = 0;

	while (1)
	{
		c[i] = fgetc(fin);
		printf("%s\n", c);
		// ignore new lines and spaces
		if (c[i] == '\n')
			continue;
		if (c[i] == ' ')
			continue;

		if (c[i] == '(')
			openP++;
		if (c[i] == ')')
			closeP++;

		if (openP == closeP)
		{
			eval(c);
			printf("   br i1 %%t%d, label %%label%d, label %%label%d\n", varNum, labelNum, labelNum+1);
			printf("\nlabel%d:\n", labelNum);
			labelNum++;
			break;
		}

		i++;
	
	}

	free(c);
	return;
}

void elseFunction(FILE* fin)
{
	char* c = malloc(1000 * sizeof(char));
	int i = 0;

	while (1)
	{
		c[i] = fgetc(fin);
		if (c[i] == '\n')
			continue;
		else if (c[i] == ' ')
			continue;

		if (strcmp(c, "if") == 0)
		{
			ifFunction(fin);
			break;
		}

		else if (c[i] == '{')
		{
			printf("   br label %%label%d\n", labelNum+1);
			printf("\nlabel%d:\n", labelNum);
			labelNum++;
			break;			
		}

		i++;
	}

	free(c);
	return;
}

void whileFunction(FILE* fin)
{
	char* c = malloc(1000 * sizeof(char));
	int i = 0;
	int openP = 0;
	int closeP = 0;

	while (1)
	{
		c[i] = fgetc(fin);

		if (c[i] == '\n')
			continue;
		else if (c[i] == ' ')
			continue;

		if (c[i] == '(')
			openP++;
		if (c[i] == ')')
			closeP++;

		if (openP == closeP)
		{
			printf("   br label %%head\n\nhead:\n");
			eval(c);
			printf("   br i1 %%t%d, label %%body, label %%exit", varNum);
			printf("\n\nbody:\n");
			break;
		}

		i++;
	}

	free(c);
	return;
}

void returnFunction(FILE* fin)
{
	char* c = malloc(1000 * sizeof(char));
	int i = 0;

	while (1)
	{
		c[i] = fgetc(fin);

		if (c[i] == '\n')
			continue;
		if (c[i] == ' ')
			continue;

		if (c[i] == ';')
		{
			c[i] == '\0';
			loadVar(c);
			break;
		}

		i++;
	}

	if (search(c) != NULL)
		printf("   ret i32 %s\n", search(c)->data);
	else
		printf("   ret i32 %s\n", c);
	
	varNum++;

	free(c);
	return;
}

void printName(FILE *fin)
{
	int i = 0;
	int varCount = 1;
	char *f = malloc(100 * sizeof(char));
	char *var = malloc(100 * sizeof(char));
	char readAhead = ' ';
	char *varHead = malloc(10 * sizeof(char));
	char *v = malloc(100 * sizeof(char));

	while (1)
	{
		f[i] = fgetc(fin);
		//printf("f: %s\n", f);
		if (f[i] == '\n')
			continue;
		if (f[i] == ' ')
			continue;
		if (f[i] == '{')
		{
			printf(" {\n");
			int n = 0;
			while(n < varCount)
			{
				allocateVar();
				printf("   store i32 %%0, i32* %%t%d, align 4\n\n", varNum);
				varNum++;
				n++;
			}
			
			free(f);
			free(v);
			free(var);
			free(varHead);
			return;
		}

		if (f[i] == ',')
		{
			varCount++;
			printf(", i32\n");
		}

		if (strcmp(f, "int") == 0)
		{
			char *v = malloc(100 * sizeof(char));
			int j = 0;
			while (1)
			{
				v[j] = fgetc(fin);

				if (v[j] == '\n')
					continue;
				if (v[j] == ' ')
					continue;
				if (v[j] == ',' || v[j] == ')')
				{
					readAhead = v[j];
					v[j] = '\0';
					sprintf(var, "%d", varNum);
					strcpy(varHead, "%t");
					strcat(varHead, var);
					printf("this is inserted: %s\nthis is var: %s\n", v, varHead);
					insert(v, varHead);
					varNum++;

					if(readAhead == ')')
					{
						printf(")");
						break;
					}

				}

				j++;
			}
		}

		i++;

	}

	free(f);
	free(v);
	free(var);
	free(varHead);
	return;
}






















































char * deleteArray(char* array)
{
	int i = 0;
  	while (array[i] != '\0')
  	{
    	array[i] = '\0';
    	i++;
  	}

  	return array;
}


char* getPostFix(char *infix)
{
	char *postfix = malloc(100);
	char *temp = malloc(10);

	struct Node *stack = NULL;

	for (int i = 0; infix[i]; ++i)
	{
		printf("postfix: %s\n", postfix);
		if (isOperand(infix[i]))
		{
			sprintf(temp, "%c", infix[i]);
			postfix = strcat(postfix, temp);
		}

		else if (infix[i] == '(')
		{

			sprintf(temp, "%c", infix[i]);
			stack = pushS(stack, temp);
		}

		else if (infix[i] == ')')
		{

			while (!isEmptyS(stack) && strcmp(peekS(&stack), "(") != 0)
				postfix = strcat(postfix, popS(&stack));

			if (!isEmptyS(stack) && strcmp(peekS(&stack), "(") != 0)
				return NULL; // invalid input
			else
				popS(&stack);

		}

		else if (isOperator(infix[i]))
		{			
			if (infix[i] == '-')
			{
				int k = 1;
				
				while (infix[i-k] == ' ')
					k++;

				if (i == 0)
				{
					postfix = strcat(postfix, "-");
					continue;
				}
								
				else if (infix[i-k] == '(' || isOperator(infix[i-k]))
				{
					postfix = strcat(postfix, "-");
					continue;
				}
			
			}

			while (!isEmptyS(stack))
			{
				if (precedence(infix[i]) <= precedence(peekS(&stack)[0]))
					postfix = strcat(postfix, popS(&stack));
				else
					break;
			}
		
			sprintf(temp, "%c", infix[i]);
			stack = pushS(stack, temp);
		}

		else if (infix[i] == ' ') // Space
		{

			if (postfix[strlen(postfix)-1] == ' ')
				continue;
			else
				postfix = strcat(postfix, " ");
		}

		else // it is a symbol
		{

			while (infix[i] != ' ')
			{
				sprintf(temp, "%c", infix[i]);
				postfix = strcat(postfix, temp);
				i++;
			}
		}
	
	}

	while (!isEmptyS(stack))
	{
		postfix = strcat(postfix, popS(&stack));
	}

	return postfix;
}


int isOperand(char ch)
{
	if (isdigit(ch))
		return 1;
	else
		return 0;
}

int isOperator(char ch)
{
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '!' || ch == '=' || ch == '>' || ch == '<' || ch == '&' || ch == '|' || ch == '@')
		return 1;
	else
		return 0;
}

int precedence(char ch)
{
	switch (ch) 
    {

    case '|':
    	return 1;

    case '&':
    	return 2;

    case '=':
    case '!':
    	return 3;

   	case '>':
   	case '<':
   		return 4;

    case '+': 
    case '-': 
        return 5; 
  
    case '*': 
    case '/':
    case '%':
        return 6;

    case '@':
    	return 7;
  
    }

    return -1;
}

int isConstant(char* expression)
{
	for (int i = 0; expression[i]; i++)
	{
		if (expression[i] == '-' || expression[i] == '(' || expression[i] == ')')
			continue;
		if (!isOperand(expression[i]))
			return 0;
	}

	return 1;
}

char* formatString(char* expression)
{
	int k = 0;
	int i;
	char* formS = malloc(100);

	for (i = 0; expression[i]; i++)
	{
		if (isOperand(expression[i]))
		{
			while (isOperand(expression[i]))
			{
				formS[k] = expression[i];
				i++;
				k++;
			}

			formS[k] = ' ';
			i--;
			k++;
		}

		else
		{
			if (!isOperator(expression[i]) && expression[i] != '(' && expression[i] != ')')
			{
				while (!isOperator(expression[i]) && expression[i] != '\0' && expression[i] != '(' && expression[i] != ')')
				{
					formS[k] = expression[i];
					k++;
					i++;
				}
				formS[k] = ' ';
				i--;
				k++;
				continue;
			}

			if (expression[i] == '-')
			{
				int neg = 1;
				while (expression[i-neg] == ' ')
					neg++;

				if (i == 0 || isOperator(expression[i-neg]) || expression[i-neg] == '(')
				{
					formS[k] = expression[i];
					k++;
					formS[k] = ' ';
					i++;
					while (isOperand(expression[i]))
					{
						formS[k] = expression[i];
						i++;
						k++;
					}
					formS[k] = ' ';
					i--;
					k++;
					continue;
				}
			}

			if (expression[i] == '=' || expression[i] == '!' || expression[i] == '&' || expression[i] == '|')
			{
				if (expression[i] == '!' && expression[i+1] != '=')
				{
					formS[k] = '@';
					k++;
					formS[k] = ' ';
					k++;
					continue;
				}

				formS[k] = expression[i];
				k++;
				formS[k] = ' ';
				k++;
				i++;
				continue;

			}
			formS[k] = expression[i];
			k++;
			formS[k] = ' ';
			k++;
		}
	}

	return formS;
}


void eval(char* postfix)
{
	printf("1st: %s\n", postfix);

	postfix = formatString(postfix);
	printf("2nd: %s\n", postfix);

	postfix = getPostFix(postfix);
	printf("3rd: %s\n", postfix);

	char *var = malloc(sizeof(char) * 5);
	char* temp1 = malloc(sizeof(char) * 5);
	char* temp2 = malloc(sizeof(char) * 5);
	char* temp3 = malloc(sizeof(char) * 5);
	char* temp4 = malloc(sizeof(char) * 5);
	char* negTerm = malloc(sizeof(char) * 5);
	char* symbol = malloc(sizeof(char) * 100);

	struct Node* stack = NULL;

	for (int i = 0; postfix[i]; i++)
	{
		if (postfix[i] == ' ')
			continue;

		else if (isOperand(postfix[i]))
		{
			int n = 0;
			
			while (isOperand(postfix[i]))
			{
				n = n * 10 + (int)(postfix[i] - '0');
				i++;
			}

			i--;
			sprintf(temp3, "%d", n); // converts n into a string
			stack = pushS(stack, temp3);
		}

		else if (isOperator(postfix[i]))
		{
			if (postfix[i] == '-')
			{

				if (isOperand(postfix[i+1]))
				{
					strcpy(negTerm, "-");
					while (isOperand(postfix[i+1]))
					{
						sprintf(temp3, "%c", postfix[i+1]);
						strcat(negTerm, temp3);
						i++;
					}
					stack = pushS(stack, negTerm);
					deleteArray(negTerm);
					continue;
				}

			}

			temp1 = popS(&stack);
			//printf("temp1: %s\n", temp1);
			temp2 = popS(&stack);
			//printf("temp2: %s\n", temp2);

			if (search(temp1) != NULL)
			{
				if (strcmp(search(temp1)->key, search(temp1)->data) == 0)
				{
					printCall(search(temp1)->data);
					i--;
					pushS(stack, temp2);
					sprintf(temp4, "%d", varNum);
					strcpy(var, "%t");
					strcat(var, temp4);


					stack = pushS(stack, var);
					varNum++;
					continue;
				}
			}

			if (search(temp2) != NULL)
			{
				if (strcmp(search(temp2)->key, search(temp2)->data) == 0)
				{
					printCall(search(temp2)->data);
					i--;
					pushS(stack, temp1);
					sprintf(temp4, "%d", varNum);
					strcpy(var, "%t");
					strcat(var, temp4);


					stack = pushS(stack, var);
					varNum++;
					continue;
				}
			}

			checkOp(temp2, temp1, postfix[i]);
			
			sprintf(temp4, "%d", varNum);
			strcpy(var, "%t");
			strcat(var, temp4);


			stack = pushS(stack, var);
			varNum++;
		}

		else // is a symbol
		{
			symbol = deleteArray(symbol);
			while (postfix[i] != ' ' && !isOperator(postfix[i]) && !isOperand(postfix[i]))
			{
				sprintf(temp3, "%c", postfix[i]);
				strcat(symbol, temp3);
				printf("symbol: %s\n", symbol);
				i++;
			}

			i--;
			if (search(symbol) == NULL)
			{
				fprintf(stderr, "error: use of undeclared variable %s\n", symbol);
				exit(-1);
			}

			if (strcmp(search(symbol)->key, search(symbol)->data) == 0)
			{
				stack = pushS(stack, search(symbol)->data);
				continue;
			}
		
			loadVar(search(symbol)->data);
			sprintf(temp3, "%d", varNum);
			strcpy(var, "%t");
			strcat(var, temp3);
			stack = pushS(stack, var);
			varNum++;
		}
	}

	free(var);
	free(temp1);
	free(temp2);
	free(temp3);

	return;
}











// Functions to print the LLVM
void printTemplate()
{
	printf("target triple = \"x86_64-pc-linux-gnu\"\n\n");

	printf("declare i32 @printf(i8*, ...) #1\n@.str = private unnamed_addr constant [4 x i8] c\"%%d\\0A\\00\", align 1\n\n");

	printf("define void @print_integer(i32) #0\n{\n");

	printf("  %%2 = alloca i32, align 4\n  store i32 %%0, i32* %%2, align 4\n  %%3 = load i32, i32* %%2, align 4\n  %%4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]*\n  @.str, i32 0, i32 0), i32 %%3)\n  ret void\n}\n\n");

	printf("%%struct._IO_FILE = type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %%struct._IO_marker*, %%struct._IO_FILE*, i32, i32, i64, i16, i8, [1 x i8], i8*, i64, %%struct._IO_codecvt*, %%struct._IO_wide_data*, %%struct._IO_FILE*, i8*, i64, i32, [20 x i8] }\n%%struct._IO_marker = type opaque\n%%struct._IO_codecvt = type opaque\n%%struct._IO_wide_data = type opaque\n\n");

	printf("@stderr = external dso_local global %%struct._IO_FILE*, align 8\n@.str.1 = private unnamed_addr constant [25 x i8] c\"please enter an integer\\0A\\00\", align 1\n@.str.2 = private unnamed_addr constant [3 x i8] c\"%%d\\00\", align 1\n@.str.3 = private unnamed_addr constant [6 x i8] c\"scanf\\00\", align 1\n@.str.4 = private unnamed_addr constant [24 x i8] c\"no matching characters\\0A\\00\", align 1\n\n");

	printf("declare i32* @__errno_location() #2\ndeclare i32 @__isoc99_scanf(i8*, ...) #1\ndeclare void @perror(i8*) #1\ndeclare void @exit(i32) #3\ndeclare i32 @fprintf(%%struct._IO_FILE*, i8*, ...) #1\n\n");

	printf("define i32 @read_integer() #0 \n{\n     %%1 = alloca i32, align 4\n     %%2 = alloca i32, align 4\n     %%3 = call i32* @__errno_location() #4\n     store i32 0, i32* %%3, align 4\n     %%4 = load %%struct._IO_FILE*, %%struct._IO_FILE** @stderr, align 8\n     %%5 = call i32 (%%struct._IO_FILE*, i8*, ...) @fprintf(%%struct._IO_FILE* %%4, i8* getelementptr inbounds ([25 x i8], [25 x i8]* @.str.1, i32 0, i32 0))\n     %%6 = call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.2, i32 0, i32 0), i32* %%1)\n     store i32 %%6, i32* %%2, align 4\n     %%7 = load i32, i32* %%2, align 4\n     %%8 = icmp eq i32 %%7, 1\n     br i1 %%8, label %%9, label %%11\n\n");

	printf("; <label>:9:                                      ; preds = %%0\n  %%10 = load i32, i32* %%1, align 4\n  ret i32 %%10\n\n; <label>:11:                                     ; preds = %%0\n  %%12 = call i32* @__errno_location() #4\n  %%13 = load i32, i32* %%12, align 4\n  %%14 = icmp ne i32 %%13, 0\n  br i1 %%14, label %%15, label %%16\n\n; <label>:15:                                     ; preds = %%11\n  call void @perror(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.3, i32 0, i32 0))\n  call void @exit(i32 1) #5\n  unreachable\n\n; <label>:16:                                     ; preds = %%11\n  %%17 = load %%struct._IO_FILE*, %%struct._IO_FILE** @stderr, align 8\n  %%18 = call i32 (%%struct._IO_FILE*, i8*, ...) @fprintf(%%struct._IO_FILE* %%17, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.4, i32 0, i32 0))\n  call void @exit(i32 1) #5\n  unreachable\n}\n\n");
}

void printMain()
{
	printf("define i32 @main() #0\n{\n\n");
}

void finishTemplate()
{
	printf("\n   ret i32 0\n}\n");
}

void checkOp(char* var1, char* var2, char operation)
{
	if (operation == '+')
		addFunc(var1, var2);

	if (operation == '-')
		subFunc(var1, var2);

	if (operation == '*')
		mulFunc(var1, var2);

	if (operation == '/')
		divFunc(var1, var2);

	if (operation == '%')
		modFunc(var1, var2);

	if (operation == '=')
		eqFunc(var1, var2);

	if (operation == '@')
		notFunc(var2);

	if (operation == '!')
		neqFunc(var1, var2);
	
	if (operation == '|')
		orFunc(var1, var2);
	
	if (operation == '&')
		andFunc(var1, var2);
	
	if (operation == '>')
		gtFunc(var1, var2);
	
	if (operation == '<')
		ltFunc(var1, var2);
}

void addFunc(char* var1, char* var2)
{
	printf("   %%t%d = add nsw i32 %s, %s\n", varNum, var1, var2);
}

void subFunc(char* var1, char* var2)
{
	printf("   %%t%d = sub nsw i32 %s, %s\n", varNum, var1, var2);
}

void mulFunc(char* var1, char* var2)
{
	printf("   %%t%d = mul nsw i32 %s, %s\n", varNum, var1, var2);
}

void divFunc(char* var1, char* var2)
{
	printf("   %%t%d = sdiv i32 %s, %s\n", varNum, var1, var2);
}

void modFunc(char* var1, char* var2)
{
	printf("   %%t%d = srem i32 %s, %s\n", varNum, var1, var2);
}

void printConst(int constant)
{
	printf("   call void @print_integer(i32 %d)\n", constant);
}

void allocateVar()
{
	printf("   %%t%d = alloca i32\n", varNum);
}

void readVar(char* symbol)
{
	printf("   %%t%d = call i32 @read_integer()\n   store i32 %%t%d, i32* %s\n", varNum, varNum, symbol);
}

void loadVar(char* symbol)
{
	printf("   %%t%d = load i32, i32* %s\n", varNum, symbol);
}

void storeVar(char* var, char* symbol)
{
	printf("   store i32 %s, i32* %s\n", var, symbol);
}

void eqFunc(char* bool1, char* bool2)
{
	printf("   %%t%d = icmp eq i32 %s, %s\n", varNum, bool1, bool2);
}

void andFunc(char* bool1, char* bool2)
{
	printf("   %%t%d = and i1 %s, %s\n", varNum, bool1, bool2);
}

void orFunc(char* bool1, char* bool2)
{
	printf("   %%t%d = or i1 %s, %s\n", varNum, bool1, bool2);
}

void notFunc(char* bool1)
{
	printf("   %%t%d = xor i1 %s, 1\n", varNum, bool1);
}

void neqFunc(char* bool1, char* bool2)
{
	printf("   %%t%d = icmp ne i32 %s, %s\n", varNum, bool1, bool2);
}

void ltFunc(char* bool1, char* bool2)
{
	printf("   %%t%d = icmp slt i32 %s, %s\n", varNum, bool1, bool2);
}

void gtFunc(char* bool1, char* bool2)
{
	printf("   %%t%d = icmp sgt i32 %s, %s\n", varNum, bool1, bool2);
}

void printCall(char* name)
{
	printf("   %%t%d = call i32 @%s(i32 %%t%d)\n", varNum, name, varNum-1);
}



// Functions for stack

// STacks for a String

// Pop
char* popS(struct Node** stack)
{
	if (isEmptyS((*stack)))
		return NULL;

	char* string = malloc(sizeof(char) * 6);

	strcpy(string, (*stack)->string);

	*stack = (*stack)->next;

	return string;
}



// Push
struct Node* pushS(struct Node* head, char* string)
{
	struct Node *temp = malloc(sizeof(struct Node));
	temp->string = malloc(sizeof(char) * 6);
	strcpy(temp->string, string);

	temp->next = head;

	return temp;
}

char* peekS(struct Node** stack)
{
	if (isEmptyS((*stack)))
		return NULL;

	return (*stack)->string;

}

int isEmptyS(struct Node* stack)
{
	if (stack == NULL)
		return 1;
	else 
		return 0;
}

// Hash Table functions

int hashCode(char* key)
{
	int val = 0;
	for (int i = 0; i < strlen(key); i++)
		val += (key[i]);

	return val % 100;
}

struct DataItem* search(char* key)
{
	int hashIndex = hashCode(key);

	while (symbolTable[hashIndex] != NULL)
	{
		if (strcmp(symbolTable[hashIndex]->key, key) == 0)
			return symbolTable[hashIndex];

		++hashIndex;

		hashIndex %= 100;
	}

	return NULL;
}

void insert(char* key, char* data)
{
	struct DataItem *item = malloc(sizeof(struct DataItem));

	item->data = malloc(sizeof(data));
	item->key = malloc(sizeof(key));

	strcpy(item->data, data);
	strcpy(item->key, key);

	int hashIndex = hashCode(key);


	while (symbolTable[hashIndex] != NULL && strcmp(symbolTable[hashIndex]->key, "-1") == 0)
	{
		++hashIndex;

		hashIndex %= 100;
	}

	symbolTable[hashIndex] = item;
}

struct DataItem* delete(struct DataItem* item)
{
	char* key = malloc(sizeof(item->key));
	strcpy(key, item->key);

	int hashIndex = hashCode(key);

	while (symbolTable[hashIndex] != NULL)
	{
		if (strcmp(symbolTable[hashIndex]->key, key) == 0)
		{
			struct DataItem* temp = symbolTable[hashIndex];

			symbolTable[hashIndex] = emptyItem;
			return temp;
		}

		++hashIndex;
		hashIndex %= 100;
	}

	return NULL;
}