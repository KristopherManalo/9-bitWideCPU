#ifndef OPCODES
#define OPCODES
    #define AND "0000"
    #define ADD "0001"
    #define LSL "0010"
    #define LSR "0011"
    #define XOR "0011"
    #define LWD "010"
    #define SWD "011"
    #define BEQ "100"
    #define BLT "101"
    #define JMP "1100"
    #define ADI "1110"
    #define BCO "1101"
    // #define GOL "110100000"
    // #define GAL "110100000"
    #define GRL "110110000"
    #define NOP "111111000"
    #define DON "000111100"
#endif

#ifndef REGISTERS
#define REGISTERS
    #define r0  "000"
    #define r1  "001"
    #define r2  "010"
    #define r3  "011"
    #define r4  "100"
    #define r5  "101"
    #define r6  "110"
    #define r7  "111"
#endif

#ifndef OFFSET 
#define OFFSET 
    #define off1 "00"
    #define off2 "01"
    #define off3 "10"
    #define off4 "11"
#endif

#ifndef IMMEDIATES
#define IMMEDIATES
    #define i0    "000"
    #define ipos1 "001"
    #define ipos2 "010"
    #define ipos3 "011"
    #define ineg1 "111"
    #define ineg2 "110"
    #define ineg3 "101"
    #define ineg4 "100"
#endif

#ifndef CONSTANTS
#define CONSTANTS
    #define NUM_LINES       1024
    #define MAX_FILE_NAME   255
    #define BUF_SIZE        255
    #define IMM_SIZE        10
#endif


#ifndef DELIMITER
#define DELIMITER
    #if __WIN32
        #define FPRINT_DELIMITER(FILE) fprintf(FILE, "\n");
        #define PRINT_DELIMITER printf("\n");
    #else
        #define FPRINT_DELIMITER(FILE) fprintf(FILE, "\r\n");
        #define PRINT_DELIMITER printf("\r\n");
    #endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef LABELS
#define LABELS
    #define MAX_LABELS 100
    struct label {
        char name[BUF_SIZE];
        int pc;
        char jumps;
    };
    typedef struct {
        struct label names[MAX_LABELS];
        int num_labels;
    } all_labels;

    typedef struct {
        char outputs[NUM_LINES][BUF_SIZE];
    } all_instructions;
#endif