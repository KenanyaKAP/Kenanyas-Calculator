//==================================
// Simple Calculator v2 by
// Kenanya Keandra Adriel Prasetyo
// 5024211004
// https://github.com/KenanyaKAP/
//==================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <gmp.h>

#define ISNEXTNUMBERVALID(STR, INDEX)                   \
if (!(isdigit(STR[INDEX]) || (STR[INDEX] == '('))) {    \
    printf("Invalid Input: %s\n", STR);                 \
    printf("%*s", 15 + INDEX, "");                      \
    printf("^\n");                                      \
    invalid = 1;                                        \
    return STR;                                         \
}

#define ISPREVNUMBERVALID(STR, INDEX)                   \
if (!(isdigit(STR[INDEX]) || (STR[INDEX] == ')'))) {    \
    printf("Invalid Input: %s\n", STR);                 \
    printf("%*s", 15 + INDEX, "");                      \
    printf("^\n");                                      \
    invalid = 1;                                        \
    return STR;                                         \
}

#define ISLASTNULL(LASTOPERATOR, STR, INDEX)            \
if (LASTOPERATOR != '\0'){                              \
    printf("Invalid Input: %s\n", STR);                 \
    printf("%*s", 14 + INDEX, "");                      \
    printf("^\n");                                      \
    invalid = 1;                                        \
    return STR;                                         \
}

// Global Variable
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

void GetNextNumber(char *str, mpz_t dest, int *index){
    int add = 0;
    char *temp = (char*)calloc(strlen(str) + 8, sizeof(char));
    for (int i = *index; i < strlen(str); i++){
        if (str[i] == '(') {*index += 1; i++; continue;}
        if (str[i] == ')') add += 1;
        if (!isdigit(str[i])){
            strncpy(temp, &str[*index], i-*index);
            mpz_set_str(dest, temp, 10);
            *index = i + add;
            free(temp);
            return;
        }
        else if (i == strlen(str)-1){
            strncpy(temp, &str[*index], (i+1)-*index);
            mpz_set_str(dest, temp, 10);
            *index = i + 1 + add;
            free(temp);
            return;
        }
    }
}

void GetPrevNumber(char *str, mpz_t dest, int *index){
    char findBracket = (str[*index] == ')');
    char *temp = (char*)calloc(strlen(str) + 8, sizeof(char));
    for (int i = *index; i >= 0; i--){
        if (findBracket && str[i] == '('){
            strncpy(temp, &str[(i+1)], *index-(i+1));
            mpz_set_str(dest, temp, 10);
            *index = i;
            free(temp);
            return;
        }
        else if (!isdigit(str[i]) && !findBracket){
            strncpy(temp, &str[(i+1)], (*index+1)-(i+1));
            mpz_set_str(dest, temp, 10);
            *index = i + 1;
            free(temp);
            return;
        }
        else if (i == 0){
            strncpy(temp, &str[0], *index+1);
            mpz_set_str(dest, temp, 10);
            *index = 0;
            free(temp);
            return;
        }
    }
}

char* ReplaceString(char *str, int start, int *end, mpz_t value){
    // Get previous and next string
    char *temp = (char*)calloc((strlen(str) + (int)mpz_sizeinbase(value, 10) + 8), sizeof(char));
    char *temp2 = (char*)calloc((strlen(str) + 8), sizeof(char));
    strncpy(temp, &str[0], start);
    strncpy(temp2, &str[*end], strlen(str) - *end);

    // Convert mpz value to string
    char *valChar = (char*)calloc(((int)mpz_sizeinbase(value, 10) + 8), sizeof(char));
    mpz_get_str(valChar, 10, value);

    // Merge everything
    if (mpz_sgn(value) == -1){
        strcat(temp, "(");
        strcat(temp, valChar);
        strcat(temp, ")");
    }
    else strcat(temp, valChar);
    *end = strlen(temp);
    strcat(temp, temp2);

    // Reallocate Memory and copy to str
    str = (char*)realloc(str, (strlen(temp) + 8) * sizeof(char));
    strcpy(str, temp); 
    str[strlen(temp)] = '\0';

    // Deallocate Dynammic Memory
    free(temp);
    free(temp2);
    free(valChar);
    return str;
}

char* Calculate(char *str){
    // Initialize Variable
    int index;
    mpz_t prev, next;

    // Calculate arithmetic inside ()
    index = 0;
    int ignored = 0;
    int startBracket = 0;
    while (index < strlen(str)){
        if (str[index] == '(' && str[index+1] != '-'){
            startBracket = ++index;
            for (int i = index; i < strlen(str); i++){
                if (str[i] == '(') {ignored += 1; continue;}
                if (str[i] == ')' && ignored != 0) {ignored -= 1; continue;}
                if (str[i] == ')' && ignored == 0){ // Parentheses found
                    // Copy arithmetic inside () to dynamic string
                    char *temp = (char*)calloc(strlen(str) + 8, sizeof(char));
                    strncpy(temp, &str[startBracket], i-startBracket);
                    
                    // Calculate arithmetic inside ()
                    printf("Calculate %s\n", temp);
                    temp = Calculate(temp);
                    if (invalid) return str;

                    // Replace
                    mpz_t value; 
                    mpz_init_set_str(value, temp, 10);
                    i += 1;
                    str = ReplaceString(str, --startBracket, &i, value);

                    // Deallocate Dynamic Memory and Setup for next loop
                    free(temp);
                    mpz_clear(value);
                    index -= 1;
                    break;
                }
            }
        }
        index += 1;
    }
    
    // Calculate ^ from behind
    index = strlen(str)-1;
    mpz_init(prev); mpz_init(next);
    while (index >= 0){
        if (str[index] == '^'){
            // Get previous and next number
            int indexS = index++ - 1;
            ISPREVNUMBERVALID(str, indexS);
            GetPrevNumber(str, prev, &indexS);
            ISNEXTNUMBERVALID(str, index);
            GetNextNumber(str, next, &index);

            // Calculate the number and replace
            if (mpz_sgn(next) != -1) mpz_pow_ui(prev, prev, mpz_get_ui(next));
            else mpz_set_str(prev, "0", 10);
            str = ReplaceString(str, indexS, &index, prev);
            continue;
        }
        index -= 1;
    }
    mpz_clear(prev); mpz_clear(next);

    // Calculate * / %
    index = 0;
    mpz_init(prev); mpz_init(next);
    while (index < strlen(str)){
        if (str[index] == '*' || str[index] == '/' || str[index] == '%'){
            // Get the Operator, Previous Number, and Next Number
            char operator = str[index];
            int indexS = index++ - 1;
            ISPREVNUMBERVALID(str, indexS);
            GetPrevNumber(str, prev, &indexS);
            ISNEXTNUMBERVALID(str, index);
            GetNextNumber(str, next, &index);

            // Calculate the number and replace
            if (operator == '*') mpz_mul(prev, prev, next);
            else if (operator == '/') mpz_div(prev, prev, next);
            else if (operator == '%') mpz_mod(prev, prev, next);
            str = ReplaceString(str, indexS, &index, prev);
            continue;
        }
        index += 1;
    }
    mpz_clear(prev); mpz_clear(next);
    

    // Take first number and operator
    index = 0;
    mpz_t sum, temp; mpz_init(sum); mpz_init(temp);
    ISNEXTNUMBERVALID(str, index);
    GetNextNumber(str, sum, &index);
    char lastOperator = str[index++];

    // Calculate + -
    while (index < strlen(str)){
        ISNEXTNUMBERVALID(str, index);
        GetNextNumber(str, temp, &index);
        switch (lastOperator){
        case '+':
            mpz_add(sum, sum, temp);
            break;
        case '-':
            mpz_sub(sum, sum, temp);
            break;
        default:
            printf("Invalid Input: %s\n", str);
            printf("%*s", 14 + index - (int)mpz_sizeinbase(temp, 10), "");
            printf("^\n");
            invalid = 1;
            return str;
        }
        lastOperator = str[index++];
    }

    // Convert sum to string and Deallocating mpz
    ISLASTNULL(lastOperator, str, index);
    mpz_get_str(str, 10, sum);
    mpz_clear(sum);
    mpz_clear(temp);
    return str;
}

int main (){
    // Opening
    clrscr();
	printf( "Simple Calculator by\n"
            "Kenanya Keandra A P\n5024211004\n"
            "\nFeatures:\n"
            "- Available Operator: ^ * / %% + -\n"
            "- Use bracket on negative number ex: (-4)\n"
            "- Parentheses Support ()\n"
            "- Big Number Support up to memory limit\n"
            "\nPress Enter to Continue...");
	getchar();

    // Looping through program
    char retry = 'y';
	while (retry != 'n'){
        // Initialize program and Input
        invalid = 0;
        char *input = (char*)calloc(4096, sizeof(char));
        clrscr();
        printf("Arithmetic Input: ");
        scanf("%[^\n]%*c", input);
        RemoveSpace(input);
        
        // Reallocate, Calculate, Print, then Deallocate
        input = (char*)realloc(input, (strlen(input) + 8) * sizeof(char));
        input = Calculate(input);
        if (!invalid) printf("Result: %s", input);
        free(input);

        // Retry ?
        printf("\n\nRetry? [y/n]: ");
	scanf("%c", &retry);
    }
    return 0;
}

// 2021 - Kenanya Keandra A P
