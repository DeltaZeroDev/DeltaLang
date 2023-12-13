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

const  int instructAmmount = 6;
char *mathsinstructionList[6] = {"ADD", "SUB", "DIV", "MUL", "MOD", "MOV"};
typedef long long int (*Maths_instruction)(long long int, long long int);
Maths_instruction MathsInstruct[5] = {ADD, SUB, DIV, MUL, MOD};

// Variable handling shit, linked lists hurt my head
struct linked_t {
  char name[100]; // 100 bytes for the name, can change
  uint64_t data;
  struct linked_t * address;
};

struct linked_t * bottom = NULL;

// find the top of the linked lists
struct linked_t *linked_top() {
    struct linked_t *p = bottom;
    while (1) {
        if (p->address == NULL) {
            break;
        }
        p = p->address;
    }
    return p;
}

// find a variable by name, or return NULL if it isn't found
struct linked_t * linked_find(char *name) {
    struct linked_t * p = bottom;
    while (1) {
        if (strcmp(p->name, name) == 0) {
            return p;
        }
        if (p->address == NULL) {
            return NULL;
        }
        p = p->address;
    }
}

// add a variable, returns the address if you want it
struct linked_t * linked_extend(char *name, uint64_t data) {
    struct linked_t *list = (struct linked_t *) malloc(sizeof(struct linked_t) + 8);
    struct linked_t *top = linked_top();
    top->address = list;
    list->address = NULL;
    list->data = data;
    strcpy(list->name, name);
    return list;
}

// call me to initialize the linked lists
void linked_init() {
    struct linked_t *list = (struct linked_t*) malloc(sizeof(struct linked_t) + 8);
    bottom = list;
    list->address = NULL;
    memset(list->name, 0, 100);
    linked_extend("$RAX", 0); linked_extend("$RIP", 0); // Hard setting registers
}


// The horror
int Exec(char *Contents) {
    linked_init(); // Initialise variable linked list
    // Define vars;
    char *temp = Contents;
    uint64_t size = 0;
    char *firstInput;
    char *secondInput;
    int currentLine = 0;
    long long int RAX;
    // While there is a new line to read:
    while (temp) {
        int isValid = 0;
        char * next = strchr(temp, '\n'); // strchr returns mem ADDRESS of the next character, NOT INDEX!!!!!!
        if (next) *next = '\0';
        currentLine++;   
        linked_find("$RIP")->data++; // Increment current process, since $RIP = (int) currentLine
        for(int i=0; i < instructAmmount; i++){ // This is dumb, InstructAmount is actually the amount of instructions in the largest instructionset, but who cares
            firstInput = strchr(temp, ' ');

            // Instructions that do maths
            size = firstInput - temp;
            if (memcmp(temp, mathsinstructionList[i], size) == 0) { // memcmp checks the line INCLUDING the null terminators, so you have to give a length, which is `size`
                /*
                WARNING!!!
                THIS CODE IS FUCKING HORRIBLE, YOU MAY NEED THERAPY AFTER READING IT
                */
                firstInput++;
                if (firstInput == NULL || firstInput[1] == '\0') { // SegFault handler
                    printf("Segmentation fault! (NULL pointer dereference) at line %d in instruction \"%s\"! (this instruction requires 2 inputs, you have none)\n", currentLine, temp);
                    return -1; 
                }
                char * instruct = malloc(strlen(temp) + 2);
                strcpy(instruct, temp); // temp gets mangled when handling instruction, so this is just for better interpretor errors
                secondInput = strchr(firstInput, ' ');
                // Error handling:
                if (secondInput == NULL) {
                    printf("Segmentation fault! (NULL pointer dereference) at line %d in instruction \"%s\"! (this instruction requires 2 inputs, you gave 1)\n", currentLine, temp);
                    return -1;
                }
                *secondInput= '\0'; // split first and second input
                secondInput++;
                long long int input1;
                long long int input2;
                if (linked_find(firstInput) != NULL){
                    if (linked_find(secondInput) != NULL){
                        input2 = linked_find(secondInput)->data;
                    }else{
                        for (int i = 0; i < strlen(secondInput); i++) {
                            if (isdigit(secondInput[i]) == 0) {
                                printf("Error! index %d in second input on line %d in instruction \"%s\" is a non digit!\n", i, currentLine, instruct);
                                return -1;
                            }
                        }
                    }
                    input1 = linked_find(firstInput)->data;
                }else{
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
                    input1 = atoi(firstInput);
                    input2 = atoi(secondInput);
                }
                // Last bit of error handling and execution
                if ((i == 2 && input2 == 0) || (i == 4 && input2 == 0)){
                    printf("Floating point execption! (Div/Mod by zero error) at line %d in instruction \"%s\"!\n", currentLine, instruct);
                    return -1;
                }
                RAX = MathsInstruct[i](input1, input2);
                printf("%lld\n", RAX);
                isValid = 1;
                free(instruct);
            }

            // Instructions that mutate variables/registers
            if (i > 4){
                if (memcmp(temp, mathsinstructionList[i], size) == 0){
                    firstInput++;
                    if (firstInput == NULL || firstInput[1] == '\0') {
                        printf("3 Segmentation fault! (NULL pointer dereference) at line %d in instruction \"%s\"! (this instruction requires 2 inputs, you have none)\n", currentLine, temp);
                        return -1; 
                    }
                    char * instruct = malloc(strlen(temp) + 2);
                    strcpy(instruct, temp);
                    secondInput = strchr(firstInput, ' ');
                    // Error handling:
                    if (secondInput == NULL) {
                        printf("4 Segmentation fault! (NULL pointer dereference) at line %d in instruction \"%s\"! (this instruction requires 2 inputs, you gave 1)\n", currentLine, temp);
                        return -1;
                    }
                    *secondInput= '\0';
                    secondInput++;
                    isValid = 1;

                }
            }
            if((i == 4) && (isValid == 0)){
                printf("Invalid instruction at line %d!; \"%s\" is an unknown instruction!\n", currentLine, temp);
                return -1;
            }
            if (i == 4) isValid = 0;
        }
        if (next) *next = '\n';
        temp = next ? (next+1) : NULL;
    }
    return 0;
}


