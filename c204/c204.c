/* ******************************* c204.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c204 - Převod infixového výrazu na postfixový (s využitím c202)     */
/*  Referenční implementace: Petr Přikryl, listopad 1994                      */
/*  Přepis do jazyka C: Lukáš Maršík, prosinec 2012                           */
/*  Upravil: Kamil Jeřábek, září 2019                                         */
/*           Daniel Dolejška, září 2021                                       */
/* ************************************************************************** */
/*
** Implementujte proceduru pro převod infixového zápisu matematického výrazu
** do postfixového tvaru. Pro převod využijte zásobník (Stack), který byl
** implementován v rámci příkladu c202. Bez správného vyřešení příkladu c202
** se o řešení tohoto příkladu nepokoušejte.
**
** Implementujte následující funkci:
**
**    infix2postfix ... konverzní funkce pro převod infixového výrazu
**                      na postfixový
**
** Pro lepší přehlednost kódu implementujte následující pomocné funkce:
**
**    untilLeftPar ... vyprázdnění zásobníku až po levou závorku
**    doOperation .... zpracování operátoru konvertovaného výrazu
**
** Své řešení účelně komentujte.
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c204.h"
#include <math.h>
#include <string.h>

bool solved;

/**
 * Pomocná funkce untilLeftPar.
 * Slouží k vyprázdnění zásobníku až po levou závorku, přičemž levá závorka bude
 * také odstraněna.
 * Pokud je zásobník prázdný, provádění funkce se ukončí.
 *
 * Operátory odstraňované ze zásobníku postupně vkládejte do výstupního pole
 * znaků postfixExpression.
 * Délka převedeného výrazu a též ukazatel na první volné místo, na které se má
 * zapisovat, představuje parametr postfixExpressionLength.
 *
 * Aby se minimalizoval počet přístupů ke struktuře zásobníku, můžete zde
 * nadeklarovat a používat pomocnou proměnnou typu char.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param postfixExpression Znakový řetězec obsahující výsledný postfixový výraz
 * @param postfixExpressionLength Ukazatel na aktuální délku výsledného postfixového výrazu
 */
void untilLeftPar(Stack *stack, char *postfixExpression, unsigned *postfixExpressionLength)
{
	if (stack->topIndex == -1)
	{
		return;
	}
	char c = '\0';
	while (true)
	{
		Stack_Top(stack, &c);
		Stack_Pop(stack);
		if (c == '(')
			return;
		postfixExpression[(*postfixExpressionLength)++] = c;
	}
}

/**
 * Pomocná funkce doOperation.
 * Zpracuje operátor, který je předán parametrem c po načtení znaku ze
 * vstupního pole znaků.
 *
 * Dle priority předaného operátoru a případně priority operátoru na vrcholu
 * zásobníku rozhodneme o dalším postupu.
 * Délka převedeného výrazu a taktéž ukazatel na první volné místo, do kterého
 * se má zapisovat, představuje parametr postfixExpressionLength, výstupním
 * polem znaků je opět postfixExpression.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param c Znak operátoru ve výrazu
 * @param postfixExpression Znakový řetězec obsahující výsledný postfixový výraz
 * @param postfixExpressionLength Ukazatel na aktuální délku výsledného postfixového výrazu
 */
void doOperation(Stack *stack, char c, char *postfixExpression, unsigned *postfixExpressionLength)
{
	while (true)
	{
		if (Stack_IsEmpty(stack))
		{
			// fprintf(stderr,"%c|\n",c);
			Stack_Push(stack, c);
			return;
		}
		char topChar = '\0';
		Stack_Top(stack, &topChar);
		if (topChar == '(')
		{
			Stack_Push(stack, c);
			return;
		}
		// if there is operator push it to stack so that the * or / will have higher precedence
		if ((topChar == '+' || topChar == '-') && (c == '*' || c == '/'))
		{
			Stack_Push(stack, c);
			return;
		}
		// otherwise add top char and continue looking for next
		Stack_Pop(stack);
		postfixExpression[(*postfixExpressionLength)++] = topChar;
	}
}

/**
 * Konverzní funkce infix2postfix.
 * Čte infixový výraz ze vstupního řetězce infixExpression a generuje
 * odpovídající postfixový výraz do výstupního řetězce (postup převodu hledejte
 * v přednáškách nebo ve studijní opoře).
 * Paměť pro výstupní řetězec (o velikosti MAX_LEN) je třeba alokovat. Volající
 * funkce, tedy příjemce konvertovaného řetězce, zajistí korektní uvolnění zde
 * alokované paměti.
 *
 * Tvar výrazu:
 * 1. Výraz obsahuje operátory + - * / ve významu sčítání, odčítání,
 *    násobení a dělení. Sčítání má stejnou prioritu jako odčítání,
 *    násobení má stejnou prioritu jako dělení. Priorita násobení je
 *    větší než priorita sčítání. Všechny operátory jsou binární
 *    (neuvažujte unární mínus).
 *
 * 2. Hodnoty ve výrazu jsou reprezentovány jednoznakovými identifikátory
 *    a číslicemi - 0..9, a..z, A..Z (velikost písmen se rozlišuje).
 *
 * 3. Ve výrazu může být použit předem neurčený počet dvojic kulatých
 *    závorek. Uvažujte, že vstupní výraz je zapsán správně (neošetřujte
 *    chybné zadání výrazu).
 *
 * 4. Každý korektně zapsaný výraz (infixový i postfixový) musí být uzavřen
 *    ukončovacím znakem '='.
 *
 * 5. Při stejné prioritě operátorů se výraz vyhodnocuje zleva doprava.
 *
 * Poznámky k implementaci
 * -----------------------
 * Jako zásobník použijte zásobník znaků Stack implementovaný v příkladu c202.
 * Pro práci se zásobníkem pak používejte výhradně operace z jeho rozhraní.
 *
 * Při implementaci využijte pomocné funkce untilLeftPar a doOperation.
 *
 * Řetězcem (infixového a postfixového výrazu) je zde myšleno pole znaků typu
 * char, jenž je korektně ukončeno nulovým znakem dle zvyklostí jazyka C.
 *
 * Na vstupu očekávejte pouze korektně zapsané a ukončené výrazy. Jejich délka
 * nepřesáhne MAX_LEN-1 (MAX_LEN i s nulovým znakem) a tedy i výsledný výraz
 * by se měl vejít do alokovaného pole. Po alokaci dynamické paměti si vždycky
 * ověřte, že se alokace skutečně zdrařila. V případě chyby alokace vraťte namísto
 * řetězce konstantu NULL.
 *
 * @param infixExpression vstupní znakový řetězec obsahující infixový výraz k převedení
 *
 * @returns znakový řetězec obsahující výsledný postfixový výraz
 */
char *infix2postfix(const char *infixExpression)
{
	if (infixExpression == NULL)
	{
		fprintf(stderr, "no string passed\n");
		return NULL;
	}
	Stack stack;
	Stack_Init(&stack);
	size_t i = 0;
	char *postFixExpr = (char *)malloc(MAX_LEN);
	// failed malloc check
	if (!postFixExpr)
	{
		fprintf(stderr, "malloc fail postfix\n");
		return NULL;
	}
	// postFixExpr[MAX_LEN - 1] = '\0';
	unsigned postCounter = 0;
	size_t len = strlen(infixExpression);
	while (i < len)
	{
		// save char and chose what to do with switch
		char c = infixExpression[i];
		switch (c)
		{
		case '+':
		case '-':
		case '*':
		case '/':
			doOperation(&stack, c, postFixExpr, &postCounter);
			break;
		case ')':
			untilLeftPar(&stack, postFixExpr, &postCounter);
			break;
		case '(':
			Stack_Push(&stack, c);
			break;
		case '=':
			// end of expresion push stack and add = could be ended but it doesnt matter
			while (!Stack_IsEmpty(&stack))
			{
				Stack_Top(&stack, &c);
				Stack_Pop(&stack);
				postFixExpr[postCounter++] = c;
			}
			postFixExpr[postCounter++] = '=';
			postFixExpr[postCounter] = '\0';
			break;
		default:
			postFixExpr[postCounter++] = c;
			break;
		}
		i++;
	}
	// fprintf(stderr, "end\n");
	// postFixExpr[postCounter] = '\0';
	fprintf(stderr,"%s v11s  %s\n",infixExpression,postFixExpr);
	return postFixExpr;
}

/**
 * Pomocná metoda pro vložení celočíselné hodnoty na zásobník.
 *
 * Použitá implementace zásobníku aktuálně umožňuje vkládání pouze
 * hodnot o velikosti jednoho byte (char). Využijte této metody
 * k rozdělení a postupné vložení celočíselné (čtyřbytové) hodnoty
 * na vrchol poskytnutého zásobníku.
 *
 * @param stack ukazatel na inicializovanou strukturu zásobníku
 * @param value hodnota k vložení na zásobník
 */
void expr_value_push(Stack *stack, int value)
{
	// findout how many digits does value have
	int max_digits = snprintf(NULL, 0, "%d", value) + 1;
	// Allocate memory for the character array
	char *tmp = (char *)malloc(max_digits);
	if (tmp == NULL)
	{
		fprintf(stderr, "Memory allocation failed.\n");
		return;
	}
	int len = snprintf(tmp, max_digits, "%d", value);
	// Convert the integer to a character string
	if (len < 0)
	{
		fprintf(stderr, "Error converting integer to string.\n");
		free(tmp);
		return;
	}
	Stack_Push(stack, ';');
	for (int i = 0; i < len; i++)
		Stack_Push(stack, tmp[i]);
	// Don't forget to free the allocated memory
	free(tmp);
}

/**
 * Pomocná metoda pro extrakci celočíselné hodnoty ze zásobníku.
 *
 * Využijte této metody k opětovnému načtení a složení celočíselné
 * hodnoty z aktuálního vrcholu poskytnutého zásobníku. Implementujte
 * tedy algoritmus opačný k algoritmu použitému v metodě
 * `expr_value_push`.
 *
 * @param stack ukazatel na inicializovanou strukturu zásobníku
 * @param value ukazatel na celočíselnou proměnnou pro uložení
 *   výsledné celočíselné hodnoty z vrcholu zásobníku
 */
void expr_value_pop(Stack *stack, int *value)
{
	if (Stack_IsEmpty(stack))
	{
		*value = 0;
		return;
	}
	*value = 0;
	char c = '\0';
	int multiplier = 0;
	while (true)
	{
		Stack_Top(stack, &c);
		// fprintf(stderr, "|%c| ", c);
		//  end of number
		if (c == ';')
		{
			Stack_Pop(stack);
			break;
		}
		int tmp = c - '0';
		if (c != '-')
			*value += (int)pow(10, multiplier++) * tmp;
		else
			*value *= -1;
		// fprintf(stderr, "val %d vs %d |\n", *value, tmp);
		Stack_Pop(stack);
	}
	// fprintf(stderr, "\nextracted val : %d\n", *value);
}
void popTwo(Stack *stack, int *a, int *b)
{
	expr_value_pop(stack, b);
	expr_value_pop(stack, a);
}
/**
 * Tato metoda provede vyhodnocení výrazu zadaném v `infixExpression`,
 * kde hodnoty proměnných použitých v daném výrazu jsou definovány
 * v poli `variableValues`.
 *
 * K vyhodnocení vstupního výrazu využijte implementaci zásobníku
 * ze cvičení c202. Dále také využijte pomocných funkcí `expr_value_push`,
 * respektive `expr_value_pop`. Při řešení si můžete definovat libovolné
 * množství vlastních pomocných funkcí.
 *
 * Předpokládejte, že hodnoty budou vždy definovány
 * pro všechy proměnné použité ve vstupním výrazu.
 *
 * @param infixExpression vstpní infixový výraz s proměnnými
 * @param variableValues hodnoty proměnných ze vstupního výrazu
 * @param variableValueCount počet hodnot (unikátních proměnných
 *   ve vstupním výrazu)
 * @param value ukazatel na celočíselnou proměnnou pro uložení
 *   výsledné hodnoty vyhodnocení vstupního výrazu
 *
 * @return výsledek vyhodnocení daného výrazu na základě poskytnutých hodnot proměnných
 */
bool eval(const char *infixExpression, VariableValue variableValues[], int variableValueCount, int *value)
{
	Stack stack;
	Stack_Init(&stack);
	char *postFix = infix2postfix(infixExpression);
	size_t len = strlen(postFix);
	for (size_t i = 0; i < len; i++)
	{
		char c = postFix[i];
		int a = 0, b = 0;
		switch (c)
		{
		case '+':
			popTwo(&stack, &a, &b);
			expr_value_push(&stack, a + b);
			// fprintf(stderr, "%d + %d = %d|\n", a, b, a + b);
			break;
		case '-':
			popTwo(&stack, &a, &b);
			expr_value_push(&stack, a - b);
			// fprintf(stderr, "%d - %d = %d|\n", a, b, a - b);
			break;
		case '*':
			popTwo(&stack, &a, &b);
			expr_value_push(&stack, a * b);
			// fprintf(stderr, "%d * %d = %d|\n", a, b, a * b);
			break;
		case '/':
			popTwo(&stack, &a, &b);
			expr_value_push(&stack, a / b);
			// fprintf(stderr, "%d / %d = %d|\n", a, b, a / b);
			break;
		case '=':
			expr_value_pop(&stack, value);
			Stack_Dispose(&stack);
			fprintf(stderr,"%s vs  %s = %d\n",infixExpression,postFix,*value);
			free(postFix);
			return true;
		default:
			for (int i = 0; i < variableValueCount; i++)
			{
				if (variableValues[i].name == c)
				{
					expr_value_push(&stack, variableValues[i].value);
				}
			}

			break;
		}
	}
	Stack_Dispose(&stack);
	free(postFix);
	return NULL;
}

/* Konec c204.c */
