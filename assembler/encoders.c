#include "encoders.h"

#define CHECK_REG(reg) { \
    if((reg = scanR(gojoa, index)) == -1) { \
        strcpy(error, REGISTER_ERR); \
        return 1; \
    } \
    else if(reg < 0) { \
        strcpy(error, REGISTER_ERR); \
        return 1; \
    } \
    else if(reg > 7) { \
        strcpy(error, REGISTER_ERR); \
        return 1; \
    } \
}

#define GET_BRANCH() {\
    switch(imm) { \
        case -5: \
            strcpy(out, "000"); break; \
        case 5: \
            strcpy(out, "001"); break; \
        case 20: \
            strcpy(out, "010"); break; \
        case -20: \
            strcpy(out, "011"); break; \
        case 10: \
            strcpy(out, "100"); break; \
        case -10: \
            strcpy(out, "101"); break; \
        case 50: \
            strcpy(out, "110"); break; \
        case -50: \
            strcpy(out, "111"); break; \
        default: \
            strcpy(error, BRANCH_ERR); \
            return 1; \
    } \
}

#define GET_JUMP() { \
    if(imm > 63 || imm < 0) { \
        strcpy(error, JUMP_ERR); \
        return -1; \
    } \
    int diff = sizeof(int) * 8 - 6; \
    for(int i = 0; i < 6; ++i) { \
        out[i] = (char)('0' + (char)((((__UINT32_TYPE__)imm << (i + diff))) >> (sizeof(int) * 8 - 1))); \
    } \
}

#define encAND() { \
    int reg0; \
    CHECK_REG(reg0); \
    int imm = scanV(gojoa, index); \
    char out[3]; \
    switch(imm) { \
        case 0: \
            strcpy(out, off1); break; \
        case 1: \
            strcpy(out, off2); break; \
        case 2: \
            strcpy(out, off3); break; \
        case 3: \
            strcpy(out, off4); break; \
        default: return 1; \
    } \
    fprintf(file_out, AND); \
    fprintfReg(file_out, reg0); \
    fprintf(file_out, out); \
    FPRINT_DELIMITER(file_out) \
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
        FPRINT_DELIMITER(file_out) \
    } \
    else { \
        fprintf(file_out, ADD); \
        fprintfReg(file_out, reg0); \
        fprintfOff(file_out, reg0, reg1); \
        FPRINT_DELIMITER(file_out) \
    } \
} 

#define encLSL() { \
    int reg0; \
    CHECK_REG(reg0); \
    int imm = scanV(gojoa, index); \
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
    FPRINT_DELIMITER(file_out) \
}

#define encLSR() { \
    int reg0; \
    CHECK_REG(reg0) \
    int imm = scanV(gojoa, index); \
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
    FPRINT_DELIMITER(file_out) \
}

#define encLWD() { \
    int reg0; \
    CHECK_REG(reg0) \
    int reg1; \
    CHECK_REG(reg1) \
    fprintf(file_out, LWD); \
    fprintfReg(file_out, reg0); \
    fprintfReg(file_out, reg1); \
    FPRINT_DELIMITER(file_out) \
}

#define encSWD() { \
    int reg0; \
    CHECK_REG(reg0) \
    int reg1; \
    CHECK_REG(reg1) \
    fprintf(file_out, LWD); \
    fprintfReg(file_out, reg0); \
    fprintfReg(file_out, reg1); \
    FPRINT_DELIMITER(file_out) \
}

#define encBEQ() { \
    int reg0; \
    CHECK_REG(reg0) \
    char out[4]; \
    int imm = scanV(gojoa, index); \
    GET_BRANCH(); \
    fprintf(file_out, BEQ); \
    fprintfReg(file_out, reg0); \
    fprintf(file_out, out); \
    FPRINT_DELIMITER(file_out) \
}

#define encBLT() { \
    int reg0; \
    CHECK_REG(reg0) \
    char out[4]; \
    int imm = scanV(gojoa, index); \
    GET_BRANCH(); \
    fprintf(file_out, BEQ); \
    fprintfReg(file_out, reg0); \
    fprintf(file_out, out); \
    FPRINT_DELIMITER(file_out) \
}

#define encJMP() { \
    int imm = scanV(gojoa, index); \
    char out[7]; \
    GET_JUMP() \
    fprintf(file_out, JMP); \
    fprintf(file_out, out); \
    FPRINT_DELIMITER(file_out) \
}

#define encADI() { \
    int reg0 = scanR(gojoa, index); \
    int imm = scanV(gojoa, index); \
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
    FPRINT_DELIMITER(file_out) \
}

int processLine(char gojoa[BUF_SIZE], FILE *file_out, 
            int *index, char error[BUF_SIZE]) {
    // printf("Processing line\r\n");
    char OPWORD[4];
    OPWORD[0] = gojoa[0];
    OPWORD[1] = gojoa[1];
    OPWORD[2] = gojoa[2];
    OPWORD[3] = '\0';
    // printf("OPWORD = %s\r\n", OPWORD);
    *index = 3;
    if(OPWORD[0] == '\r' || OPWORD[0] == '\n') {
        return 0;
    }
    else if(OPWORD[0] == '/' && OPWORD[1] == '/') {
        return 0;
    }
    else if(strcmp(OPWORD, "AND") == 0) {
        encAND()
    }
    else if(strcmp(OPWORD, "ADD") == 0) {
        encADD()
    }
    else if(strcmp(OPWORD, "LSL") == 0) {
        encLSL()
    }
    else if(strcmp(OPWORD, "LSR") == 0) {
        encLSR()
    }
    else if(strcmp(OPWORD, "LWD") == 0) {
        encLWD()
    }
    else if(strcmp(OPWORD, "SWD") == 0) {
        encSWD()
    }
    else if(strcmp(OPWORD, "BEQ") == 0) {
        encBEQ()
    }
    else if(strcmp(OPWORD, "BLT") == 0) {
        encBLT()
    }
    else if(strcmp(OPWORD, "JMP") == 0) {
        encJMP()
    }
    else if(strcmp(OPWORD, "ADI") == 0) {
        encADI()
    }
    else if(strcmp(OPWORD, "NOP") == 0) {
        fprintf(file_out, NOP);
        FPRINT_DELIMITER(file_out)
    }
    else {
        *index = 0;
        // printf("index = %d\n", *index);
        strcpy(error, INSTRUCTION_ERR);
        return 1;
    }
    return 0;
}

int scanR(char gojoa[BUF_SIZE], int *index) {
    int temp = *index;
    // printf("index = %d", *index);
    for(; *index < BUF_SIZE; (*index)++) {
        if(gojoa[*index] == '\0') {
            return -1;
        }
        if(gojoa[*index] == 'r') {
            // Set new index to the char after register
            *index = *index + 2;
            // return atoi(gojoa[(*index)+1]);
            // printf("gojoa[(*index)+1] = %c; index = %d\r\n", gojoa[(*index)-1], *index);
            return gojoa[(*index)-1] - '0';
        }
    }
    *index = temp;
    return -1;
}

int scanV(char gojoa[BUF_SIZE], int *index) {
    int temp = *index;
    char imm[IMM_SIZE];
    for(; *index < BUF_SIZE; (*index)++) {
        if(gojoa[*index] == '\0') {
            return -1;
        }
        if(gojoa[*index] == '#') {
            (*index)++;
            int i = 0;
            for(; i < IMM_SIZE; i++) {
                if(gojoa[*index] == ' ' 
                        || gojoa[*index] == ','
                        || gojoa[*index] == '\r' 
                        || gojoa[*index] == '\n') {
                    break;
                }
                imm[i] = gojoa[(*index)++];
            }
            imm[i] = '\0';
            (*index)++;
            // printf("imm = %s", imm);
            return atoi(imm);
        }
    }
    *index = temp;
    return -1;
}

void fprintfReg(FILE *file_in, int reg) {
    // printf("Printing r%d\n", reg);
    switch(reg) {
        case 0:
            fprintf(file_in, r0);
            break;
        case 1:
            fprintf(file_in, r1);
            break;
        case 2:
            fprintf(file_in, r2);
            break;
        case 3:
            fprintf(file_in, r3);
            break;
        case 4:
            fprintf(file_in, r4);
            break;
        case 5:
            fprintf(file_in, r5);
            break;
        case 6:
            fprintf(file_in, r6);
            break;
        case 7:
            fprintf(file_in, r7);
            break;
    }
}

void fprintfOff(FILE *fine_in, int reg0, int reg1) {
    switch(reg1-reg0) {
        case 1:
        case -7:
            fprintf(fine_in, "00");
            break;
        case 2:
        case -6:
            fprintf(fine_in, "01");
            break;
        case 3:
        case -5:
            fprintf(fine_in, "10");
            break;
        case 4:
        case -4:
            fprintf(fine_in, "11");
            break;
    }

}

int regInRange(int reg0, int reg1) {
    // printf("reg1 = %d, reg0 = %d, (reg0+4) = %d\r\n",reg1, reg0, (reg0+4));
    switch(reg1-reg0) {
        case 0:
            return -1;
        case 1:
        case -7:
        case 2:
        case -6:
        case 3:
        case -5:
        case 4:
        case -4:
            return 1;
        default:
            return 0;
    }
}

