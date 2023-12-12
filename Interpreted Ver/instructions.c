#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
long long int ADD(long long int inp1, long long int inp2) {return(inp1+inp2);};
long long int SUB(long long int inp1, long long int inp2) {return(inp1-inp2);};
long long int DIV(long long int inp1, long long int inp2) {return(inp1/inp2);};
long long int MUL(long long int inp1, long long int inp2) {return(inp1*inp2);};
long long int MOD(long long int inp1, long long int inp2) {return(inp1%inp2);};

const  int instructAmmount = 5;
char *mathsinstructionList[5] = {"ADD", "SUB", "DIV", "MUL", "MOD"};
typedef long long int (*Maths_instruction)(long long int, long long int);
Maths_instruction MathsInstruct[5] = {ADD, SUB, DIV, MUL, MOD};



int Exec(char * Contents) {
    char *temp = Contents;
    uint64_t size = 0;
    char *firstInput;
    char *secondInput;
    int currentLine = 0;
    while (temp) {
        long long int EAX;
        int isValid = 0;
        char * next = strchr(temp, '\n'); // strchr returns mem ADDRESS of the next character, NOT INDEX!!!!!!
        if (next) *next = '\0';
        currentLine++;
        for(int i=0; i < instructAmmount; i++){
            firstInput = strchr(temp, ' ');
            if (firstInput == NULL || firstInput[1] == '\0') {
                printf("Segmentation fault! (NULL pointer dereference) at line %d in instruction \"%s\"! (this instruction requires 2 inputs, you have none)\n", currentLine, temp);
                return -1; 
            }
            size = firstInput - temp; // takes mem addr of the ' ' takes it away from temp which gives the length of the instruction 
            if (memcmp(temp, mathsinstructionList[i], size) == 0) { // memcmp checks the line INCLUEDING the null terminators, so you have to give a length, which is `size`
                char * instruct = malloc(strlen(temp) + 2);
                strcpy(instruct, temp);
                firstInput++;
                secondInput = strchr(firstInput, ' ');
                // Error handling:
                if (secondInput == NULL || secondInput[1] == '\0') {
                    printf("Segmentation fault! (NULL pointer dereference) at line %d in instruction \"%s\"! (this instruction requires 2 inputs, you gave 1)\n", currentLine, instruct);
                    return -1;
                }
                *secondInput= '\0';
                secondInput++;
                for (int i = 0; i < strlen(firstInput); i++) {
                    if (isdigit(firstInput[i]) == 0) {
                        printf("Error! index %d in first input on line %d in instruction \"%s\" is a non digit!\n", i, currentLine, instruct);
                        return -1;
                    }
                }
                for (int i = 0; i < strlen(secondInput); i++) {
                    if (isdigit(secondInput[i]) == 0) {
                        printf("Error! index %d in second input on line %d in instruction \"%s\" is a non digit!\n", i, currentLine, instruct);
                        return -1;
                    }
                }
                // redefining inputs as their value, but as correct type
                long long int input1 = atoi(firstInput);
                long long int input2 = atoi(secondInput);
                // Last bit of error handling and execution
                if (i == 2 && input1 == 0 || i == 2 && input2 == 0 || i == 4 && input2 == 0 || i == 4 && input2 == 0){
                    printf("Floating point execption! (Div/Mod by zero error) at line %d in instruction \"%s\"!\n", currentLine, instruct);
                    return -1;
                }
                EAX = MathsInstruct[i](input1, input2);
                printf("%lld\n", EAX);
                isValid = 1;
                free(instruct);
            }
            if((i == 4) && (isValid == 0)){
                printf("Invalid instruction at line %d!; \"%s\" is an invalid instruction!\n", currentLine, temp);
                return -1;
            }
            if (i == 4) isValid = 0;
        }
        if (next) *next = '\n';
        temp = next ? (next+1) : NULL;
    }
    return 0;
}


