#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define K 2  //k nie bylo podane ile ma byc
#define N 100

#define e 2.7182818284590452353602874713527

struct Result
{
	int valid;

	double x;
	double fx;
};

struct Table
{
	int size;
	struct Result results[N];
};

enum FunctionKeys
{
	Up = 72,
	Down = 80,
	PgUp = 73,
	PgDn = 81,
	Home = 71,
	End = 79,
	Esc = 27
};

struct Result function(double x);

void lookUpTable1(struct Table* table);
void lookUpTable2(struct Table* table);

void printMenu();

void clearTable(struct Table* table);
void displayTable(const struct Table* table);
void printNavigationHelp(int scrollingSize);
void printTableContent(const struct Table* table, int start, int end);

int getKey();
void anyKey();
void clearScreen();

void read1(double* a, double* b, int* n);
void read2(double* a, double* delta, int* n);

int safeLoadInt(int* input);
int safeLoadDouble(double* input);

void printCenteredString(const char* str, int width);
void printCenteredInt(int value, int width);
void printCenteredDouble(double value, int width);

int main()
{
	struct Table table;
	clearTable(&table);

	int key = 0;
	while (key != '4')
	{
		clearScreen();
		printMenu();

		printf("Wybierz opcje: ");
		key = getKey();
		switch (key)
		{
			case '1': lookUpTable1(&table); break;
			case '2': lookUpTable2(&table); break;
			case '3': displayTable(&table); break;
		}
	}

	return 0;
}

struct Result function(double x)
{
	struct Result result;
	result.x = x;

	double nominator = cbrt(atan(x) * atan(x)) + pow(e, 5 * sin(x));

	if (x == 0 || x == 1)
	{
		result.valid = 0;
		return result;
	}

	double denominator = 1.0;
	for (int i = 1; i <= K; i++)
	{
		denominator += (pow(x, i) - 1) * ((1 / pow(x, i)) + 0.2);
	}


	result.valid = 1;
	result.fx = nominator / denominator;

	return result;
}

void lookUpTable1(struct Table* table)
{
	clearTable(table);

	double a, b;
	int n;

	clearScreen();
	printf("================================\n");
	printf("1. Tablicowanie od a do b\n");
	printf("================================\n");

	read1(&a, &b, &n);

	if (n < 0)
		n = 0;
	else if (n > N)
		n = N;

	table->size = n;

	for (int i = 0; i < n; i++)
	{
		if (n - 1 == 0)
		{
			table->results[i] = function(a);
			break;
		}

		double x = a + i * (b - a) / (n - 1);
		table->results[i] = function(x);
	}
}

void lookUpTable2(struct Table* table)
{
	clearTable(table);

	double a;
	double delta;
	int n;

	clearScreen();
	printf("================================\n");
	printf("2. Tablicowanie od a co delta\n");
	printf("================================\n");

	read2(&a, &delta, &n);

	if (n < 0)
		n = 0;
	else if (n > N)
		n = N;

	table->size = n;

	for (int i = 0; i < n; i++)
	{
		double x = a + i * delta;
		table->results[i] = function(x);
	}
}

void printMenu()
{
	printf("================================\n");
	printf("         MENU PROGRAMU\n");
	printf("================================\n");
	printf("1. Tablicowanie od a do b\n");
	printf("2. Tablicowanie od a co delta\n");
	printf("3. Wyswietl tabele\n");
	printf("4. Wyjscie\n");
	printf("===============================\n");
}


void clearTable(struct Table* table)
{
	memset(table, 0, sizeof(struct Table));
}

void displayTable(const struct Table* table)
{
	if (table->size == 0)
	{
		clearScreen();
		printf("Brak danych do wyswietlenia");
		anyKey();
		return;
	}

	int scrollingSize = table->size / 3;
	if (scrollingSize > 10)
	{
		scrollingSize = 10;
	}

	int start = 0;
	int end = scrollingSize;


	int key = 0;
	do
	{
		clearScreen();
		printTableContent(table, start, end);
		printNavigationHelp(scrollingSize);

		key = getKey();
		switch (key)
		{
			case Up: start--; end--; break;
			case Down: start++; end++; break;
			case PgUp: start -= scrollingSize; end -= scrollingSize; break;
			case PgDn: start += scrollingSize; end += scrollingSize; break;
			case Home: start = 0; end = scrollingSize; break;
			case End: start = table->size - scrollingSize; end = table->size; break;
			case Esc: break;	
		}

		if (start < 0)
		{
			start = 0;
			end = scrollingSize;
		}
		else if (end > table->size)
		{
			end = table->size;
			start = end - scrollingSize;
		}

	} while (key != Esc);

	anyKey();
}


void printTableContent(const struct Table* table, int start, int end)
{
	int width = 16;

	printf("----------------------------------------------------\n");
	printf("|                   Dane w tabeli                  |\n");
	printf("----------------------------------------------------\n");
	printf("|");
	printCenteredString("lp", width);
	printf("|");
	printCenteredString("x", width);
	printf("|");
	printCenteredString("f(x)", width);
	printf("|\n");

	printf("-%.*s|-%.*s|-%.*s-\n", width, "----------------", width, "---------------", width, "---------------");


	for (int i = start; i < end; i++)
	{

		printf("|");
		printCenteredInt(i + 1, width);
		printf("|");
		printCenteredDouble(table->results[i].x, width);
		printf("|");
		if (table->results[i].valid)
			printCenteredDouble(table->results[i].fx, width);
		else 
			printCenteredString("POZA DZIEDZINA", width);
		printf("|\n");

		if (i != end - 1)
		printf("-%.*s|-%.*s|-%.*s-\n", width, "----------------", width, "---------------", width, "---------------");
	}

	printf("----------------------------------------------------\n");
}

void printNavigationHelp(int scrollingSize)
{
	printf("\n\n--------------------------------------------------------------------------------------------------\n");
	printf("Strzalki gora/dol - 1 wiersz | PgUp/PgDn - %d wierszy | Home/End - poczatek/koniec | Esc - wyjscie\n", scrollingSize);
	printf("--------------------------------------------------------------------------------------------------\n");
}

int getKey()
{
	int key = getch();
	if (key == -32)
	{
		return getch();
	}

	return key;
}

void anyKey()
{
	printf("\n\nKontynuuj...");
	getch();
}

void clearScreen()
{
	system("cls");
}

void clearInputBuffer()
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {}
}

void read1(double* a, double* b, int* n)
{
	do
	{
		printf("Podaj a: ");
	} while (!safeLoadDouble(a));

	do
	{
		printf("Podaj b: ");
	} while (!safeLoadDouble(b));

	do
	{
		printf("Podaj n: ");
	} while (!safeLoadInt(n));
}

void read2(double* a, double* delta, int* n)
{
	do
	{
		printf("Podaj a: ");
	} while (!safeLoadDouble(a));

	do
	{
		printf("Podaj delte: ");
	} while (!safeLoadDouble(delta));

	do
	{
		printf("Podaj n: ");
	} while (!safeLoadInt(n));
}

int safeLoadInt(int* input)
{
	char buffer[64];
	if (fgets(buffer, sizeof(buffer), stdin) != NULL)
	{
		// Check if the input is a valid integer and nothing else
		if (sscanf(buffer, "%d", input) == 1)
		{
			// Check if the input has only one number and nothing else
			char* endptr;
			strtol(buffer, &endptr, 10);
			if (*endptr == '\n' || *endptr == '\0')
			{
				return 1;
			}
		}
	}
	return 0;
}

int safeLoadDouble(double* input)
{
	char buffer[64];
	if (fgets(buffer, sizeof(buffer), stdin) != NULL)
	{
		// Check if the input is a valid double and nothing else
		if (sscanf(buffer, "%lf", input) == 1)
		{
			// Check if the input has only one number and nothing else
			char* endptr;
			strtod(buffer, &endptr);
			if (*endptr == '\n' || *endptr == '\0')
			{
				return 1;
			}
		}
	}
	return 0;
}

void printCenteredString(const char* str, int width)
{
	int len = strlen(str);
	int padding = (width - len) / 2;
	printf("%*s%*s", padding + len, str, width - padding - len, "");
}

void printCenteredInt(int value, int width)
{
	char buffer[20];
	int len = snprintf(buffer, sizeof(buffer), "%d", value);
	int padding = (width - len) / 2;
	printf("%*s%*s", padding + len, buffer, width - padding - len, "");
}

void printCenteredDouble(double value, int width)
{
	char buffer[20];
	int len = snprintf(buffer, sizeof(buffer), "%.2lf", value);
	int padding = (width - len) / 2;
	printf("%*s%*s", padding + len, buffer, width - padding - len, "");
}
