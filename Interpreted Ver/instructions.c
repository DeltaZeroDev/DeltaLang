#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>

int64_t ADD(int64_t inp1, int64_t inp2) {return(inp1+inp2);};
int64_t SUB(int64_t inp1, int64_t inp2) {return(inp1-inp2);};
int64_t DIV(int64_t inp1, int64_t inp2) {return(inp1/inp2);};
int64_t MUL(int64_t inp1, int64_t inp2) {return(inp1*inp2);};
int64_t MOD(int64_t inp1, int64_t inp2) {return(inp1%inp2);};
int64_t RND(int64_t inp1, int64_t inp2) {return(rand() % inp2 + inp1);};

const int instructAmmount = 8;
char *intructionList[8] = {"ADD", "SUB", "DIV", "MUL", "MOD", "RND", "MOV", "DEF"};
typedef int64_t (*Maths_instruction)(int64_t, int64_t);
Maths_instruction MathsInstruct[6] = {ADD, SUB, DIV, MUL, MOD, RND};

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
    linked_extend("$RAX", 0); linked_extend("$RBX", 0); linked_extend("$RIP", 0); // Hard setting registers
}

// The horror
int Exec(char *Contents) {
    srand (time(NULL));
    linked_init(); // Initialise variable linked list
    // Define vars;
    char *temp = Contents;
    uint64_t size = 0;
    char *firstInput;
    char *secondInput;
    int currentLine = 0;
    int64_t RAX;
    // While there is a new line to read:
    while (temp) {
        int isValid = 0;
        char * next = strchr(temp, '\n'); // strchr returns mem ADDRESS of the next character, NOT INDEX!!!!!!
        if (next) *next = '\0';
        currentLine++;
        linked_find("$RIP")->data++; // Increment current process, since $RIP = (int) currentLine
        for(int i=0; i < instructAmmount; i++){ // This is dumb, InstructAmount is actually the amount of instructions in the largest instructionset, but who cares
            firstInput = strchr(temp, ' '); // set firstInput to everything after the instruction
            char * instruct = malloc(strlen(temp) + 2);
            strcpy(instruct, temp);
            // Instructions that do maths
            size = firstInput - temp; // set length of instruction
            if (memcmp(temp, intructionList[i], size) == 0 && i < 6) { // &&i<6 for maths instructions
                /*
                WARNING!!!
                THIS CODE IS FUCKING HORRIBLE, YOU MAY NEED THERAPY AFTER READING IT
                */
                firstInput++; // Get rid of space at the start of firstInput
                secondInput = strchr(firstInput, ' '); // Second input is everything after first input, including space
                if ((firstInput == NULL || firstInput[1] == '\0')  || (secondInput == NULL || secondInput[1] == '\0')){ // SegFault handler
                    printf("Segmentation fault! (NULL pointer dereference) at line %d in instruction \"%s\"! (this instruction requires 2 inputs)\n", currentLine, temp);
                    return -1;
                }

                *secondInput= '\0'; // null terminator, ends first input
                secondInput++; // so that secondInput isn't just NULL
                int64_t input1;
                int64_t input2;

                // Variable handling
                if (linked_find(firstInput) != NULL  || linked_find(secondInput) != NULL){ // For some reason if input1 is " " it thinks it's in the linked list, but if firstinput is " ", this is technically the correct error
                    if (linked_find(secondInput) != NULL){
                        input2 = linked_find(secondInput)->data;
                    } else {input2 = atoi(secondInput);}
                    if (linked_find(firstInput) != NULL){
                        input1 = linked_find(firstInput)->data;
                    } else {input1 = atoi(firstInput);}
                } else {input1 = atoi(firstInput); input2 = atoi(secondInput);}

                // Last bit of error handling and execution
                if ((i == 2 && input2 == 0) || (i == 4 && input2 == 0)){
                    printf("Floating point execption! (Div/Mod by zero error) at line %d in instruction \"%s\"!\n", currentLine, instruct);
                    return -1;
                }
                linked_find("$RAX")->data = MathsInstruct[i](input1, input2); // Transfer RAX's value to the linked list
                printf("%s - %ld\n", instruct, linked_find("$RAX")->data); // This will be deleted later after OUT instruction is made
                isValid = 1;
                free(instruct); // Memory leak who?
            }

            // Instructions that mutate variables/registers
            if (i == 6){ // special case for MOV rn
                if (memcmp(temp, intructionList[i], size) == 0){
                    firstInput++;
                    secondInput = strchr(firstInput, ' ');
                    *secondInput= '\0';
                    secondInput++;
                    if (firstInput == NULL   || secondInput == NULL){ // SegFault handler
                        printf("Segmentation fault! (NULL pointer dereference) at line %d in instruction \"%s\"! (this instruction requires 2 inputs)\n", currentLine, instruct);
                        return -1; 
                    }
                    if (linked_find(firstInput) == NULL){
                        printf("Error: Cannot mutate a non variable! At line %d in instruction \"%s\"!\n", currentLine, instruct);
                        return -1;
                    }
                    if (linked_find(secondInput) != NULL) {linked_find(firstInput)->data = linked_find(secondInput)->data;} else {linked_find(firstInput)->data = atoi(secondInput);}
                    secondInput++;
                    printf("%s - %ld\n",instruct, linked_find(firstInput)->data); // Deleted when OUT is made
                    isValid = 1;
                }
            }
            if((i == 7) && (isValid == 0)){
                printf("Invalid instruction at line %d!; \"%s\" is an unknown instruction!\n", currentLine, instruct);
                return -1;
            }
            if (i == 6) isValid = 0;
        }
        if (next) *next = '\n';
        temp = next ? (next+1) : NULL;
    }
    return 0;
}


