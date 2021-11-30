// Simple Calculator by
// Kenanya Keandra A P
// 5024211004
// 2021

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <gmp.h>

#define GETNEXTVALIDNUMBER(DEST, STR, INDEX)            \
if (!(isdigit(STR[INDEX]) || (STR[INDEX] == '('))) {    \
    printf("Invalid Input: %s\n", STR);                 \
    printf("%*s", 15 + INDEX, "");                      \
    printf("^");                                        \
    invalid = 1;                                        \
    return;                                             \
}                                                       \
GetNextNumber(DEST, STR, &INDEX);

#define GETPREVVALIDNUMBER(DEST, STR, INDEX)            \
if (!(isdigit(STR[INDEX]) || (STR[INDEX] == ')'))) {    \
    printf("Invalid Input: %s\n", STR);                 \
    printf("%*s", 15 + INDEX, "");                      \
    printf("^");                                        \
    invalid = 1;                                        \
    return;                                             \
}                                                       \
GetPrevNumber(DEST, STR, &INDEX);

#define INVALIDOPERATOR(LASTOPERATOR, STR, INDEX)       \
if (LASTOPERATOR != '\0'){                              \
    printf("Invalid Input: %s\n", STR);                 \
    printf("%*s", 15 + INDEX, "");                      \
    printf("^");                                        \
    invalid = 1;                                        \
    return;                                             \
}

// Create global variable
char temp[4096];
char invalid = 0;

void clrscr(){
    system("@cls||clear");
}

void RemoveSpace(char* str){
    char* temp = str;
    do {
        while (*temp == ' '){
            ++temp;
        }
    } while (*str++ = *temp++);
}

void GetNextNumber(char dest[], char str[], int *start){
    int add = 0;
    for (int i = *start; i < strlen(str); i++){
        if (str[i] == '(') {*start += 1; i++; continue;}
        if (str[i] == ')') add += 1;
        if (!isdigit(str[i])){
            strncpy(dest, &str[*start], i-*start); dest[i-*start] = '\0';
            *start = i + add;
            return;
        }
        else if (i == strlen(str)-1){
            strncpy(dest, &str[*start], (i+1)-*start); dest[(i+1)-*start] = '\0';
            *start = i + 1 + add;
            return;
        }
    }
}

void GetPrevNumber(char dest[], char str[], int *start){
    char findBracket = (str[*start] == ')');
    for (int i = *start; i >= 0; i--){
        if (findBracket && str[i] == '('){
            strncpy(dest, &str[(i+1)], *start-(i+1)); dest[*start-(i+1)] = '\0';
            *start = i;
            return;
        }
        else if (!isdigit(str[i]) && !findBracket){
            strncpy(dest, &str[(i+1)], (*start+1)-(i+1)); dest[(*start+1)-(i+1)] = '\0';
            *start = i + 1;
            return;
        }
        else if (i == 0){
            strncpy(dest, &str[0], *start+1); dest[*start+1] = '\0';
            *start = 0;
            return;
        }
    }
}

void ReplaceString(char str[], int start, int *end, mpz_t value){
    char temp2[4096];
    strncpy(temp, &str[0], start); temp[start] = '\0';
    strncpy(temp2, &str[*end], strlen(str) - *end); temp2[strlen(str) - *end] = '\0';
    
    char valChar[4096];
    mpz_get_str(valChar, 10, value);
    if (mpz_sgn(value) == -1){
      strcat(temp, "(");
      strcat(temp, valChar);
      strcat(temp, ")");
    }
    else strcat(temp, valChar);
    mpz_clear(value);
    
    *end = strlen(temp);
    strcat(temp, temp2);
    strcpy(str, temp);
    str[strlen(temp)] = '\0';
}

void Calculate(char str[]){
    // Initial Adjustment
    RemoveSpace(str);
    int index = 0;

    // Calculate arithmetic on () first
    index = 0;
    int ignored = 0;
    int startBracket = 0;
    while (index < strlen(str)){
        if (str[index] == '(' && str[index+1] != '-'){
            startBracket = ++index;
            for (int i = index; i < strlen(str); i++){
                if (str[i] == '(') {ignored += 1; continue;}
                if (str[i] == ')' && ignored != 0) {ignored -= 1; continue;}
                if (str[i] == ')' && ignored == 0){
                    // Parentheses found
                    char temp2[4096];
                    strncpy(temp2, &str[startBracket], i-startBracket); temp2[i-startBracket] = '\0';
                    Calculate(temp2);
                    if (invalid) return;

                    // Replace with Calculated Number
                    i += 1;
                    mpz_t res; mpz_init_set_str(res, temp2, 10);
                    ReplaceString(str, --startBracket, &i, res);
                    mpz_clear(res);
                    index = -1;
                    break;
                }
            }
        }
        index += 1;
    }

    // Calculate ^ first from behind
    index = strlen(str)-1;
    while (index >= 0){
        if (str[index] == '^'){
            // Get Previous Number, and Next Number
            int indexS = index++ - 1;
            mpz_t prev, next;
            GETPREVVALIDNUMBER(temp, str, indexS);
            mpz_init_set_str(prev, temp, 10);
            GETNEXTVALIDNUMBER(temp, str, index);
            mpz_init_set_str(next, temp, 10);

            // Replace with Calculated Number
            if (mpz_sgn(next) != -1) mpz_pow_ui(prev, prev, mpz_get_ui(next));
            else mpz_init_set_str(prev, "0", 10);
            ReplaceString(str, indexS, &index, prev);
            mpz_clear(prev);
            mpz_clear(next);
            continue;
        }
        index -= 1;
    }

    // Calculate * / % first
    index = 0;
    while (index < strlen(str)){
        if (str[index] == '*' || str[index] == '/' || str[index] == '%'){
            // Get the Operator, Previous Number, and Next Number
            char operator = str[index];
            int indexS = index++ - 1;
            mpz_t prev, next;
            GETPREVVALIDNUMBER(temp, str, indexS);
            mpz_init_set_str(prev, temp, 10);
            GETNEXTVALIDNUMBER(temp, str, index);
            mpz_init_set_str(next, temp, 10);

            // Replace with Calculated Number
            if (operator == '*') mpz_mul(prev, prev, next);
            else if (operator == '/') mpz_div(prev, prev, next);
            else if (operator == '%') mpz_mod(prev, prev, next);
            ReplaceString(str, indexS, &index, prev);
            mpz_clear(prev);
            mpz_clear(next);
            continue;
        }
        index += 1;
    }
    
    // Take first number and operator
    index = 0;
    mpz_t sum, tempMpz;
    GETNEXTVALIDNUMBER(temp, str, index);
    mpz_init_set_str(sum, temp, 10);
    mpz_init(tempMpz);
    char lastOperator = str[index++];

    // Then Calculate + -
    while (index < strlen(str)){
        switch (lastOperator){
        case '+':
            GETNEXTVALIDNUMBER(temp , str, index);
            mpz_init_set_str(tempMpz, temp, 10);
            mpz_add(sum , sum, tempMpz);
            break;
        case '-':
            GETNEXTVALIDNUMBER(temp, str, index);
            mpz_init_set_str(tempMpz, temp, 10);
            mpz_sub(sum, sum, tempMpz);
            break;
        default:
            printf("Invalid Input: %s\n", str);
            printf("%*s", 14 + index, "");
            printf("^");
            invalid = 1;
            return;
        }
        lastOperator = str[index++];
    }

    // Check is last operator not ""
    INVALIDOPERATOR(lastOperator, str, index-1);
    mpz_get_str(str, 10, sum);
    mpz_clear(sum);
    mpz_clear(tempMpz);
}

int main (){
    char input[4096] = "0";
	clrscr();
	printf( "Simple Calculator by\n"
            "Kenanya Keandra A P\n5024211004\n"
            "\nFeatures:\n"
            "- Available Operator: ^ * / %% + -\n"
            "- Use bracket on negative number ex: (-4)\n"
            "- Parentheses Support ()\n"
            "- Big Number Support up to 4096 Digits\n"
            "\nPress Enter to Continue...");
	getchar();

	char retry = 'y';
	while (retry != 'n'){
		invalid = 0;
		clrscr();
		printf("Arithmetic Input: ");
		scanf("%[^\n]%*c", input);
		Calculate(input);
		if (!invalid) printf("Result: %s", input);
		printf("\n\nRetry? [y/n]: ");
		scanf("%c", &retry);
	}
    return 0;
}