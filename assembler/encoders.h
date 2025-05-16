#include "assembler.h"
#include "errors.h"
#include "flags.h"

/**
 * Encodes a single line of freaky gojo assembly
 * @param gojoa     line of freaky gojo assembly to encode
 * @param file_out  pointer to output file
 * @param character pointer to int that contains current char in line
 * @param error     pointer to error message
 * @param lineOut   pointer to line number processed
 * @param flags     bit flags for behavior
 * @param labels    array of labels for control flow
 * @param num_labels number of labels in array
 * @returns 0 if successful, else 1
 */
int processLine(char gojoa[BUF_SIZE], /*FILE *file_out, */
    int *character, char error[BUF_SIZE], int *lineOut, 
    int flags, all_labels *labels,
    char current_label[BUF_SIZE],
    all_instructions *outputs);

/**
 * Scans the line for the next register
 * @param gojoa assembly line to scan
 * @param index index to start scanning,
 *                  index is updated to ending value
 * @returns found register,
 *          returns -1 if no register found
 */
int scanR(char[BUF_SIZE], int*, char[BUF_SIZE], int*);

/**
 * Scans the line for next immediate value
 * @param gojoa assembly line to scan
 * @param index index to start scanning
 *                  index is updated to ending value
 * @returns found immediate,
 *          returns -1 if no immediate found
 */
int scanV(char[BUF_SIZE], int*, char[BUF_SIZE], int*);

/**
 * Detects label in provided buffer
 * 
 * @param gojoa assembly line to scan
 * @param index index to start scanning
 * 
 * @returns 1 if successful, 0 if no label detected
 */
int scanL(char[BUF_SIZE], int*, char[BUF_SIZE], int*, char[BUF_SIZE]);

/**
 * Prints to file register address
 * @param file_in   pointer to output file
 * @param reg       reg to write
 */
void fprintfReg(FILE*, int);

/**
 * Prints to file offset from reg0
 * reg0 never equals reg1
 * @param file_in   pointer to output file
 * @param reg0      base register
 * @param reg1      register from offset
 */
void fprintfOff(FILE*, int, int);

void encodeOff(int, int, char[BUF_SIZE], int *pt);

/**
 * Checks if register 1 is in range of register 0
 * A register is in range if r1 is in (*r0, (*r) + 4]
 * @param reg0  base register
 * @param reg1  offset register
 * 
 * @returns 1 if true, 0 if false, -1 if same register
 */
int regInRange(int, int);

/**
 * Pads instructions with NOPs to properly space
 * instructions for branches
 * @param init      go from position
 * @param goal      label definition
 * @param currPC    current lines in program
 * 
 * @returns 1 if successful, 0 if failed, -1 if illegal
 */
int padInstructions(int, int, int*, 
    all_instructions*, all_labels*);

void encodeReg(int*, char[BUF_SIZE], int);

void encodeImm(int*, char[BUF_SIZE], const char[IMM_SIZE]);

// OLD MACROS:
/*
#define encAND() { \
    int reg0; \
    CHECK_REG(reg0); \
    int reg1; \
    CHECK_REG(reg1); \
    int valid = regInRange(reg0, reg1); \
    if(valid == 1) { \
        fprintf(file_out, AND); \
        fprintfReg(file_out, reg0); \
        fprintfOff(file_out, reg0, reg1); \
    } \
    else { \
        strcpy(error, REGISTER_OFFSET_ERR); \
        return 1; \
    } \
}

#define encADD() { \
    int reg0; \
    CHECK_REG(reg0) \
    int reg1; \
    CHECK_REG(reg1) \
    int valid = regInRange(reg0, reg1); \
    if(valid == 0) { \
        strcpy(error, REGISTER_OFFSET_ERR); \
        return 1; \
    } \
    else if(valid == -1) { \
        fprintf(file_out, LSL); \
        fprintfReg(file_out, reg0); \
        fprintf(file_out, off1); \
    } \
    else { \
        fprintf(file_out, ADD); \
        fprintfReg(file_out, reg0); \
        fprintfOff(file_out, reg0, reg1); \
    } \
} 

#define encLSL() { \
    int reg0; \
    CHECK_REG(reg0); \
    int imm; \
    CHECK_IMM(imm) \
    char bit_shift[3]; \
    switch(imm) { \
        case 1: strcpy(bit_shift, off1); break; \
        case 2: strcpy(bit_shift, off2); break; \
        case 3: strcpy(bit_shift, off3); break; \
        case 4: strcpy(bit_shift, off4); break; \
        default: strcpy(error, IMMEDIATE_ERR); return 1; \
    } \
    fprintf(file_out, LSL); \
    fprintfReg(file_out, reg0); \
    fprintf(file_out, bit_shift); \
}

#define encLSR() { \
    int reg0; \
    CHECK_REG(reg0) \
    int imm; \
    CHECK_IMM(imm) \
    char bit_shift[3]; \
    switch(imm) { \
        case 1: strcpy(bit_shift, off1); break; \
        case 2: strcpy(bit_shift, off2); break; \
        case 3: strcpy(bit_shift, off3); break; \
        case 4: strcpy(bit_shift, off4); break; \
        default: strcpy(error, IMMEDIATE_ERR); return 1; \
    } \
    fprintf(file_out, LSR); \
    fprintfReg(file_out, reg0); \
    fprintf(file_out, bit_shift); \
}

#define encLWD() { \
    int reg0; \
    CHECK_REG(reg0) \
    int reg1; \
    CHECK_REG(reg1) \
    fprintf(file_out, LWD); \
    fprintfReg(file_out, reg0); \
    fprintfReg(file_out, reg1); \
}

#define encSWD() { \
    int reg0; \
    CHECK_REG(reg0) \
    int reg1; \
    CHECK_REG(reg1) \
    fprintf(file_out, SWD); \
    fprintfReg(file_out, reg0); \
    fprintfReg(file_out, reg1); \
}

#define encBEQ() { \
    int reg0; \
    CHECK_REG(reg0) \
    char out[BUF_SIZE]; \
    int imm; \
    CHECK_IMM_BRANCH(imm) \
    if(imm != -1) { \
        GET_BRANCH(); \
    } \
    fprintf(file_out, BEQ); \
    fprintfReg(file_out, reg0); \
    fprintf(file_out, out); \
}

#define encBLT() { \
    int reg0; \
    CHECK_REG(reg0) \
    char out[BUF_SIZE]; \
    int imm; \
    CHECK_IMM_BRANCH(imm) \
    if(imm != -1) { \
        GET_BRANCH(); \
    } \
    fprintf(file_out, BLT); \
    fprintfReg(file_out, reg0); \
    fprintf(file_out, out); \
}

#define encJMP() { \
    int imm; \
    CHECK_IMM(imm); \
    char out[7]; \
    GET_JUMP() \
    out[6] = '\0'; \
    fprintf(file_out, JMP); \
    fprintf(file_out, out); \
}

#define encADI() { \
    int reg0; \
    CHECK_REG(reg0) \
    int imm; \
    CHECK_IMM(imm) \
    char imm_out[4]; \
    switch(imm) { \
        case -4: \
            strcpy(imm_out, ineg4); \
            break; \
        case -3: \
            strcpy(imm_out, ineg3); \
            break; \
        case -2: \
            strcpy(imm_out, ineg2); \
            break; \
        case -1: \
            strcpy(imm_out, ineg1); \
            break; \
        case 0: \
            strcpy(imm_out, i0); \
            break; \
        case 1: \
            strcpy(imm_out, ipos1); \
            break; \
        case 2: \
            strcpy(imm_out, ipos2); \
            break; \
        case 3: \
            strcpy(imm_out, ipos3); \
            break; \
        default: \
            strcpy(error, IMMEDIATE_ERR); \
            return 1; \
    } \
    fprintf(file_out, ADI); \
    fprintfReg(file_out, reg0); \
    fprintf(file_out, imm_out); \
}
    */