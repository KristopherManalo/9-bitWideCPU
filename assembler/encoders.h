#include "assembler.h"
#include "errors.h"

/**
 * Encodes a single line of freaky gojo assembly
 * @param gojoa     line of freaky gojo assembly to encode
 * @param file_out  pointer to output file
 * @param character pointer to int that contains current char in line
 * @param error     pointer to error message
 * @returns 0 if successful, else 1
 */
int processLine(char gojoa[BUF_SIZE], FILE *file_out, 
    int *character, char error[BUF_SIZE], int *lineOut);

/**
 * Scans the line for the next register
 * @param gojoa assembly line to scan
 * @param index index to start scanning,
 *                  index is updated to ending value
 * @returns found register,
 *          returns -1 if no register found
 */
int scanR(char[BUF_SIZE], int*);

/**
 * Scans the line for next immediate value
 * @param gojoa assembly line to scan
 * @param index index to start scanning
 *                  index is updated to ending value
 * @returns found immediate,
 *          returns -1 if no immediate found
 */
int scanV(char[BUF_SIZE], int*);

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

/**
 * Checks if register 1 is in range of register 0
 * A register is in range if r1 is in (*r0, (*r) + 4]
 * @param reg0  base register
 * @param reg1  offset register
 * 
 * @returns 1 if true, 0 if false, -1 if same register
 */
int regInRange(int, int);
