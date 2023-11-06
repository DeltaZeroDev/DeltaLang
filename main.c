#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// I think the integers are long, just a guess tho.
long long int ADD(long long int inp1, long long int inp2) {return(inp1+inp2);};
long long int SUB(long long int inp1, long long int inp2) {return(inp1-inp2);};
long long int DIV(long long int inp1, long long int inp2) {return(inp1/inp2);};
long long int MUL(long long int inp1, long long int inp2) {return(inp1*inp2);};
long long int MOD(long long int inp1, long long int inp2) {return(inp1%inp2);};

char *instructionList[5] = {"ADD", "SUB", "DIV", "MUL", "MOD"};
typedef long long int (*Maths_instruction)(long long int, long long int);
Maths_instruction MathsInstruct[5] = {ADD, SUB, DIV, MUL, MOD};

int Exec(){
    char *Instructs[3];// = Proccess(Contents); Process input to 3 seperate inputs put in an array
    Instructs[0] = (char*) (malloc(16));
    strcpy(Instructs[0], "MUL");
    for(int i=0; i < 5; i++){
        if (strcmp(Instructs[0], instructionList[i]) == 0){
            long long int result = MathsInstruct[i](3, 2);        
        }
    }
    return 0;
}
