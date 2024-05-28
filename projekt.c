#include <ctype.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <windows.h>

#define STR_SIZE 128
#define MENU_WIDTH 40

struct _Smartwatch
{
	char model[STR_SIZE];
	char brand[STR_SIZE];
	int gps;
	int nfc;
	double price;
};
typedef struct _Smartwatch Smartwatch;

struct DataBase
{
	FILE* file;
	char filename[STR_SIZE];
	Smartwatch* buffer;
	int size;
};
typedef struct DataBase DataBase;

void openDataBase(DataBase* db);
void createDataBase(DataBase* db);
void browseDataBase(DataBase* db);
void sortDataBase(DataBase* db);
void saveDataBase(DataBase* db);
void deleteDataBase(DataBase* db);
void closeDataBase(DataBase* db);
void comfirmCloseDataBase(DataBase* db);
void clearDataBase(DataBase* db);
void flushDataBase(DataBase* db);

void addRecord(DataBase* db);
void modifyRecord(DataBase* db, int index);
void removeRecord(DataBase* db, int index);

void sortByModel(DataBase* db);
void sortByBrand(DataBase* db);
void sortByPrice(DataBase* db);
void sortByNFC(DataBase* db);
void sortByGPS(DataBase* db);

int checkDataBaseSize(FILE* file);
int isDataBaseNameValid(const char* name);

enum MenuOptions
{
	OPEN_DATABASE = '1',
	CLOSE_DATABASE = '2',
	CREATE_DATABASE = '3',
	BROWSE_DATABASE = '4',
	SORT_DATABASE = '5',
	SAVE_DATABASE = '6',
	DELETE_DATABASE = '7',
	EXIT = '8'
};

enum Keys
{
	UP = 72,
	DOWN = 80,
	HOME = 71,
	END = 79,
	ESC = 27,
	ADD_RECORD = 'd',
	DELETE_RECORD = 'u',
	MODIFY_RECORD = 'm'
};

void createExampleDataBase();

void printHeader(const char* text, DataBase* db);
void printMenu(void);
void printStatus(DataBase* db);
void printSmartwatch(Smartwatch* watch);

void anyKey(void);
void clearScreen(void);
int getKey(void);
void safeLoadInt(int* input, const char* message);
void safeLoadDouble(double* input, const char* message);
void safeLoadString(char* input, const char* message);
void clearInputBuffer(void);
void printCentered(const char* text, ...);


int main(void)
{
	DataBase db;
	clearDataBase(&db);

	createExampleDataBase();

	int choice = 0;
	while (choice != EXIT)
	{
		clearScreen();
		printHeader("Baza danych Smartwatchow", &db);
		printMenu();

		choice = getKey();
		switch (choice)
		{
			case OPEN_DATABASE:
			{
				openDataBase(&db);
				break;
			}
			case CLOSE_DATABASE:
			{
				comfirmCloseDataBase(&db);
				break;
			}
			case CREATE_DATABASE:
			{
				createDataBase(&db);
				break;
			}
			case BROWSE_DATABASE:
			{
				browseDataBase(&db);
				break;
			}
			case SORT_DATABASE:
			{
				sortDataBase(&db);
				break;
			}
			case SAVE_DATABASE:
			{
				saveDataBase(&db);
				break;
			}
			case DELETE_DATABASE:
			{
				deleteDataBase(&db);
				break;
			}
			case EXIT:
			{
				if (db.file)
				{
					saveDataBase(&db);
					closeDataBase(&db);
				}

				clearScreen();
				printf("\nWychodzenie...");
				anyKey();
				break;
			}
			default:
			{
				printf("\nNiepoprawny wybor\n");
				anyKey();
				break;
			}
		}
	}
}

void openDataBase(DataBase* db)
{
	printHeader("Otwieranie bazy danych", db);

	char name[STR_SIZE] = { '\0' };
	printf("\n(Poprawny format nazwy pliku: bazaXX.dat)\n");
	while (1)
	{
		safeLoadString(name, "Podaj nazwe pliku: ");
		if (!isDataBaseNameValid(name))
			printf("Niepoprawna nazwa pliku\n");
		else
			break;
	}

	if (strcmp(db->filename, name) == 0)
	{
		printf("Ta baza danych jest juz otwarta!\n");
		anyKey();
		return;
	}

	FILE* file = fopen(name, "r+");
	if (!file)
	{
		printf("Nie udalo sie otworzyc bazy danych (byc moze nie istnieje)!\n");
		anyKey();
		return;
	}

	if (db->file)
	{
		printf("UWAGA! Otwieranie nowej bazy danych spowoduje zamkniecie aktualnie otwartej!\n");
		printf("Czy na pewno chcesz kontynuowac? (t/n): ");
		int choice = getKey();
		if (choice != 't')
		{
			printf("\nAnulowano otwieranie bazy danych!\n");
			anyKey();
			fclose(file);
			return;
		}

		saveDataBase(db);
		closeDataBase(db);
	}

	int size = checkDataBaseSize(file);
	if (size == -1)
	{
		printf("Baza danych jest uszkodzona!\n");
		closeDataBase(db);
		anyKey();
		return;
	}

	rewind(file);
	db->buffer = (Smartwatch*)malloc(size * sizeof(Smartwatch));
	fread(db->buffer, sizeof(Smartwatch), size, file);
	if (!db->buffer)
	{
		printf("Blad alokacji danych!\n");
		closeDataBase(db);
		anyKey();
		return;
	}

	db->size = size;
	db->file = file;
	strcpy(db->filename, name);
	rewind(db->file);
	printf("\nBaza danych %s zostala otwarta!\n", name);
	anyKey();
}

void createDataBase(DataBase* db)
{
	printHeader("Tworzenie nowej bazy danych", db);

	char name[STR_SIZE] = { '\0' };
	printf("\n(Poprawny format nazwy pliku: bazaXX.dat)\n");
	while (1)
	{
		safeLoadString(name, "Podaj nazwe pliku: ");
		if (!isDataBaseNameValid(name))
			printf("Niepoprawna nazwa pliku\n");
		else
			break;
	}

	if (strcmp(db->filename, name) == 0)
	{
		printf("Ta baza danych jest juz otwarta!\n");
		anyKey();
		return;
	}

	FILE* file = fopen(name, "r");
	if (file)
	{
		printf("Baza danych o tej nazwie juz istnieje!\n");
		fclose(file);
		anyKey();
		return;
	}

	file = fopen(name, "w+");
	if (!file)
	{
		printf("Nie udalo sie utworzyc bazy danych!\n");
		anyKey();
	}
	else
	{
		printf("Baza danych %s zostala utworzona!\n", name);
		anyKey();
	}
}

void browseDataBase(DataBase* db)
{
	clearScreen();
	printHeader("Przegladanie bazy danych", db);
	if (!db->file)
	{
		printf("Baza danych nie zostala otwarta!\n");
		anyKey();
		return;
	}

	int keyPressed = 0;
	int currentRecord = 0;
	while (keyPressed != ESC)
	{
		clearScreen();
		printHeader("Przegladanie bazy danych", db);
		printCentered("Strzalki gora/dol - 1 wiersz");
		printCentered("Home/End - poczatek/koniec");
		printCentered("D - dodaj rekord | u - usun rekord");
		printCentered("m - modyfikuj rekord | Esc - Cofnij");
		printf("----------------------------------------\n");

		if (db->size != 0)
		{
			if (currentRecord < 0)
				currentRecord = 0;
			else if (currentRecord >= db->size)
				currentRecord = db->size - 1;

			printCentered("Rekord: %d/%d", currentRecord + 1, db->size);
			printf("----------------------------------------\n");
			printSmartwatch(&db->buffer[currentRecord]);
		}
		else
		{
			printCentered("Baza danych jest pusta!");
		}

		keyPressed = getKey();
		switch (keyPressed)
		{
			case UP: currentRecord--; break;
			case DOWN: currentRecord++; break;
			case HOME: currentRecord = 0; break;
			case END: currentRecord = db->size - 1; break;
			case ADD_RECORD: addRecord(db); break;
			case DELETE_RECORD: removeRecord(db, currentRecord); break;
			case MODIFY_RECORD: modifyRecord(db, currentRecord); break;
		}
	}
}

void sortDataBase(DataBase* db)
{
	clearScreen();
	printHeader("Sortowanie bazy danych", db);
	if (!db->file)
	{
		printf("Baza danych nie zostala otwarta!\n");
		anyKey();
		return;
	}

	if (db->size <= 0)
	{
		printf("Baza danych jest pusta!\n");
		anyKey();
		return;
	}

	int choice = 0;
	while (choice != '6')
	{
		clearScreen();
		printHeader("Sortowanie bazy danych", db);
		printCentered("1. Sortuj po modelu");
		printCentered("2. Sortuj po marce");
		printCentered("3. Sortuj po cenie");
		printCentered("4. Sortuj po GPS");
		printCentered("5. Sortuj po NFC");
		printCentered("6. Cofnij");
		printf("----------------------------------------\n");
		printf("\nWybierz opcje: ");
		choice = getKey();
		printf("\n");

		switch (choice)
		{
			case '1': sortByBrand(db); printf("\nPosortowano!"); anyKey(); return;
			case '2': sortByModel(db); printf("\nPosortowano!"); anyKey(); return;
			case '3': sortByPrice(db); printf("\nPosortowano!"); anyKey(); return;
			case '4': sortByGPS(db); printf("\nPosortowano!"); anyKey(); return;
			case '5': sortByNFC(db); printf("\nPosortowano!"); anyKey(); return;
			default: break;
		}
	}
}

void saveDataBase(DataBase* db)
{
	clearScreen();
	printHeader("Zapisywanie bazy danych", db);

	if (!db->file)
	{
		printf("Baza danych nie zostala otwarta!\n");
		anyKey();
		return;
	}

	printf("Czy na pewno chcesz zapisac baze danych %s? (t/n): ", db->filename);
	int choice = getKey();
	if (choice != 't')
	{
		printf("\nAnulowano zapisywanie bazy danych %s!\n", db->filename);
		anyKey();
		return;
	}

	flushDataBase(db);
	printf("\nBaza danych %s zostala zapisana!\n", db->filename);
	anyKey();
}

void deleteDataBase(DataBase* db)
{
	clearScreen();
	printHeader("Usuwanie bazy danych", db);

	if (!db->file)
	{
		printf("Baza danych nie zostala otwarta!\n");
		anyKey();
		return;
	}

	printf("Czy na pewno chcesz usunac baze danych %s? (t/n): ", db->filename);
	int choice = getKey();
	if (choice == 't')
	{
		fclose(db->file);
		if (!remove(db->filename))
		{
			closeDataBase(db);
			printf("\nBaza danych %s zostala usunieta!\n", db->filename);
			anyKey();
		}
		else
		{
			printf("\nNie udalo sie usunac bazy danych %s!\n", db->filename);
			anyKey();
		}
	}
	else
	{
		printf("\nAnulowano usuwanie bazy danych %s!\n", db->filename);
		anyKey();
	}
}

void closeDataBase(DataBase* db)
{
	if (db->buffer)
		free(db->buffer);
	if (db->file)
		fclose(db->file);

	clearDataBase(db);
}

void comfirmCloseDataBase(DataBase* db)
{
	clearScreen();
	printHeader("Zamykanie bazy danych", db);
	if (!db->file)
	{
		printf("Baza danych nie zostala otwarta!\n");
		anyKey();
		return;
	}

	printf("Czy na pewno chcesz zamknac baze danych %s? (t/n): ", db->filename);
	int choice = getKey();
	if (choice == 't')
	{
		closeDataBase(db);
		printf("\nBaza danych %s zostala zamknieta!\n", db->filename);
		anyKey();
	}
	else
	{
		printf("\nAnulowano zamykanie bazy danych %s!\n", db->filename);
		anyKey();
	}
}

void clearDataBase(DataBase* db)
{
	memset((void*)db, 0, sizeof(DataBase));
}

void flushDataBase(DataBase* db)
{
	if (!db->file)
		return;

	freopen(db->filename, "w+", db->file);
	fwrite(db->buffer, sizeof(Smartwatch), db->size, db->file);
}

void addRecord(DataBase* db)
{
	clearScreen();
	printHeader("Dodawanie nowego rekordu", db);
	char model[STR_SIZE] = { '\0' };
	char brand[STR_SIZE] = { '\0' };
	int gps = 0;
	int nfc = 0;
	double price = 0.0;

	safeLoadString(model, "Podaj model: ");
	safeLoadString(brand, "Podaj marke: ");
	safeLoadInt(&gps, "Czy posiada GPS? (0 - NIE, 1 - TAK): ");
	safeLoadInt(&nfc, "Czy posiada NFC? (0 - NIE, 1 - TAK): ");
	safeLoadDouble(&price, "Podaj cene: ");

	Smartwatch watch;
	strcpy(watch.model, model);
	strcpy(watch.brand, brand);
	watch.gps = gps;
	watch.nfc = nfc;
	watch.price = price;

	db->buffer = (Smartwatch*)realloc(db->buffer, (db->size + 1) * sizeof(Smartwatch));
	db->buffer[db->size] = watch;
	db->size++;

	printf("\nRekord zostal dodany!\n");
	anyKey();
}

void modifyRecord(DataBase* db, int index)
{
	if (db->size <= 0)
		return;

	int choice = 0;
	while (choice != '6')
	{
		clearScreen();
		printHeader("Modyfikacja rekordu", db);
		printCentered("Rekord %d/%d\n", index + 1, db->size);
		printSmartwatch(&db->buffer[index]);
		printf("\n6. Cofnij\n");

		printf("\n\nWybierz pole do edycji: ");
		choice = getKey();
		printf("\n");

		switch (choice)
		{
			case '1':
			{
				char model[STR_SIZE] = { '\0' };
				safeLoadString(model, "Podaj model: ");
				strcpy(db->buffer[index].model, model);
				break;
			}
			case '2':
			{
				char brand[STR_SIZE] = { '\0' };
				safeLoadString(brand, "Podaj marke: ");
				strcpy(db->buffer[index].model, brand);
				break;
			}
			case '3':
			{
				int gps = 0;
				safeLoadInt(&gps, "Czy posiada GPS (0 - nie/1 - tak): ");
				db->buffer[index].gps = gps;
				break;
			}
			case '4':
			{
				int nfc = 0;
				safeLoadInt(&nfc, "Czy posiada NFC (0 - nie/1 - tak): ");
				db->buffer[index].nfc = nfc;
				break;
			}
			case '5':
			{
				double price = 0.0;
				safeLoadDouble(&price, "Podaj cene: ");
				db->buffer[index].price = price;
				break;
			}

		default: break;
		}
	}
}

void removeRecord(DataBase* db, int index)
{
	if (db->size <= 0)
		return;

	printf("Czy na pewno chcesz usunac rekord %d? (t/n): ", index + 1);
	int choice = getKey();
	if (choice != 't')
	{
		printf("\nAnulowano...");
		anyKey();
		return;
	}

	if (db->size == 1)
	{
		free(db->buffer);
		db->size = 0;
		db->buffer = NULL;
	}
	else
	{
		Smartwatch last = db->buffer[db->size - 1];
		db->buffer[index] = last;
		db->size--;
	}

	printf("\nRekord %d zostal usuniety!\n", index + 1);
	anyKey();
}

void sortByModel(DataBase* db)
{
	for (int i = 0; i < db->size; i++)
	{
		for (int j = 0; j < db->size - 1; j++)
		{
			if (strcmp(db->buffer[j].model, db->buffer[j + 1].model) < 0)
			{
				Smartwatch temp = db->buffer[j];
				db->buffer[j] = db->buffer[j + 1];
				db->buffer[j + 1] = temp;
			}
		}
	}
}

void sortByBrand(DataBase* db)
{
	for (int i = 0; i < db->size; i++)
	{
		for (int j = 0; j < db->size - 1; j++)
		{
			if (strcmp(db->buffer[j].brand, db->buffer[j + 1].brand) < 0)
			{
				Smartwatch temp = db->buffer[j];
				db->buffer[j] = db->buffer[j + 1];
				db->buffer[j + 1] = temp;
			}
		}
	}
}

void sortByPrice(DataBase* db)
{
	for (int i = 0; i < db->size; i++)
	{
		for (int j = 0; j < db->size - 1; j++)
		{
			if (db->buffer[j].price > db->buffer[j + 1].price)
			{
				Smartwatch temp = db->buffer[j];
				db->buffer[j] = db->buffer[j + 1];
				db->buffer[j + 1] = temp;
			}
		}
	}
}

void sortByNFC(DataBase* db)
{
	for (int i = 0; i < db->size; i++)
	{
		for (int j = 0; j < db->size - 1; j++)
		{
			if (db->buffer[j].nfc < db->buffer[j + 1].nfc)
			{
				Smartwatch temp = db->buffer[j];
				db->buffer[j] = db->buffer[j + 1];
				db->buffer[j + 1] = temp;
			}
		}
	}
}

void sortByGPS(DataBase* db)
{
	for (int i = 0; i < db->size; i++)
	{
		for (int j = 0; j < db->size - 1; j++)
		{
			if (db->buffer[j].gps < db->buffer[j + 1].gps)
			{
				Smartwatch temp = db->buffer[j];
				db->buffer[j] = db->buffer[j + 1];
				db->buffer[j + 1] = temp;
			}
		}
	}
}

int checkDataBaseSize(FILE* file)
{
	if (!file)
		return 0;

	fseek(file, 0L, SEEK_END);
	long size = ftell(file);

	if (size == 0)
		return 0;

	if (size % sizeof(Smartwatch) != 0)
		return -1;
	else
		return size / sizeof(Smartwatch);
}

int isDataBaseNameValid(const char* name)
{
	int lenght = (int)strlen(name);

	if (lenght != 9 && lenght != 10)
		return 0;

	if (name[0] != 'b' || name[1] != 'a' || name[2] != 'z' || name[3] != 'a')
		return 0;

	if (lenght == 10)
	{
		if (!isdigit(name[4]) || !isdigit(name[5]))
			return 0;

		if (name[6] != '.' || name[7] != 'd' || name[8] != 'a' || name[9] != 't')
			return 0;
	}
	else
	{
		if (!isdigit(name[4]))
			return 0;

		if (name[5] != '.' || name[6] != 'd' || name[7] != 'a' || name[8] != 't')
			return 0;
	}

	return 1;
}

void createExampleDataBase()
{
	FILE* file = fopen("baza01.dat", "w+");

	char* models[] = { "DX101", "DX103", "Watch Fit 3", "VENU 3s", "WATCH 4 PRO" };
	char* brands[] = { "KELLO", "KELLO", "HUAWEI", "GARMIN", "HUAWEI" };
	int gps[] = { 1, 1, 1, 1, 1 };
	int nfc[] = { 0, 0, 0, 0, 1 };
	double prices[] = { 229.00, 229.00, 599.00, 2129.00, 2599.00 };
	
	Smartwatch watches[5];
	for (int i = 0; i < 5; i++)
	{
		strcpy(watches[i].model, models[i]);
		strcpy(watches[i].brand, brands[i]);
		watches[i].gps = gps[i];
		watches[i].nfc = nfc[i];
		watches[i].price = prices[i];
	}

	fwrite(&watches, sizeof(Smartwatch), 5, file);
	fclose(file);
}

void printHeader(const char* text, DataBase* db)
{
	clearScreen();
	printf("----------------------------------------\n");
	printCentered(text);
	printf("----------------------------------------\n");
	printStatus(db);
}

void printMenu(void)
{
	printCentered("1. Otworz baze danych");
	printCentered("2. Zamknij baze danych");
	printCentered("3. Utworz nowa baze danych");
	printCentered("4. Przeglad bazy");
	printCentered("5. Sortowanie bazy");
	printCentered("6. Zapisywanie bazy");
	printCentered("7. Usun baze");
	printCentered("8. Zakoncz program");
	printf("----------------------------------------\n");
	printf("\nWybierz opcje: ");
}

void printStatus(DataBase* db)
{
	if (!db->file)
		return;

	printCentered("Aktualnie otwarta baza: %s", db->filename);
	printf("----------------------------------------\n");
}

void printSmartwatch(Smartwatch* watch)
{
	printf("1. Model: %s\n", watch->model);
	printf("2. Marka: %s\n", watch->brand);
	printf("3. GPS: %s\n", watch->gps ? "TAK" : "NIE");
	printf("4. NFC: %s\n", watch->nfc ? "TAK" : "NIE");
	printf("5. Cena: %.2fzl\n", watch->price);
	printf("\n");
}

void anyKey(void)
{
	printf("\n\nKontynuuj...");
	getch();
}

void clearScreen(void)
{
	system("cls");
}

int getKey(void)
{
	int key = getch();
	if (key == -32)
	{
		return getch();
	}

	return key;
}

void safeLoadInt(int* input, const char* message)
{
	char buffer[STR_SIZE];
	while (1)
	{
		printf("%s", message);
		if (fgets(buffer, sizeof(buffer), stdin) != NULL)
		{
			char* endptr;
			*input = strtol(buffer, &endptr, 10);
			if (buffer != endptr)
				break;
		}

		clearInputBuffer();
	}
}

void safeLoadDouble(double* input, const char* message)
{
	char buffer[STR_SIZE];
	while (1)
	{
		printf("%s", message);
		if (fgets(buffer, sizeof(buffer), stdin) != NULL)
		{
			char* endptr;
			*input = strtod(buffer, &endptr);
			if (buffer != endptr)
				break;
		}
		clearInputBuffer();
	}
}

void safeLoadString(char* input, const char* message)
{
	char buffer[STR_SIZE];
	while (1)
	{
		printf("%s", message);
		if (fgets(buffer, sizeof(buffer), stdin) != NULL)
		{
			strncat(input, buffer, strlen(buffer) - 1);
			break;
		}

		clearInputBuffer();
	}
}

void clearInputBuffer(void)
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {}
}

void printCentered(const char* text, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, sizeof(buffer), text, args);
	va_end(args);

	int textLength = (int)strlen(buffer);
	int padding = (MENU_WIDTH - textLength) / 2;
	for (int i = 0; i < padding; i++)
	{
		printf(" ");
	}
	printf("%s\n", buffer);
}
