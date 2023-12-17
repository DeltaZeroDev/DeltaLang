#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "instructions.c"

FILE *DeltaCode;
char filename[256];

char *contentFetch(char * filename){
    uint64_t size;
    DeltaCode = fopen(filename, "r");
    if(DeltaCode == NULL)return (char *) 0;
    fseek(DeltaCode, 0L, SEEK_END);
    size = ftell(DeltaCode);
    fseek(DeltaCode, 0L, SEEK_SET);
    char * Raw = (char*) (malloc(size + 2));
    fread(Raw, size + 1, 1, DeltaCode);
    fclose(DeltaCode);
    return Raw;
}

int main(){
    char * Contents;
    int rv;
    printf("Enter name of file you are interpreting: ");
    scanf("%s", filename);
    Contents = contentFetch(filename);
    if (Contents == NULL) {
        printf("File does not exist!\n");
        return 0;
    }
    rv = Exec(Contents);
    free(Contents);
    return 0;
}
