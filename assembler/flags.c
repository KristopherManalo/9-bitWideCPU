#include "flags.h"
#include "assembler.h"


void printHelp(char* exec_name) {
    printf("Usage: %s [input] [options] ...", exec_name);
    PRINT_DELIMITER
    printf("Options: ");
    PRINT_DELIMITER
    printf("    -o                      Declares output file");
    PRINT_DELIMITER
    printf("    -a                      Appends line with corresponding assembly line");
    PRINT_DELIMITER
    printf("    -b                      Prints only binary values");
    PRINT_DELIMITER
    printf("    -r                      Reorganizes assembly instructions");
    PRINT_DELIMITER
    printf("    -f                      Prints flags");
    PRINT_DELIMITER
}

void printFlags(__int32 flags) {
    printf("[");
    int i;
    for(i = 0; i < FLAGS_NUMBER - 1; i++) {
        printf("%d, ", ((flags >> (31-i))&0x00000001));
    }
    printf("%d", (flags >> (31-i)&0x00000001));
    printf("]");
}