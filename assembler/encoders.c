#include "encoders.h"
#include "assembler.h"

#define CHECK_REG(reg) { \
    if((reg = scanR(gojoa, index, current_inst, &instP)) == -1) { \
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

#define CHECK_IMM(imm) { \
    if((imm = scanV(gojoa, index, current_inst, &instP)) == -1) { \
        return -1; \
    } \
}

#define CHECK_IMM_BRANCH(imm) { \
    if((imm = scanV(gojoa, index, current_inst, &instP)) == -1) { \
        if((flags & REORGANIZE_ASM) != REORGANIZE_ASM) { \
            strcpy(error, ILLEGAL_LABEL); \
            return 1; \
        } \
        if(scanL(gojoa, index, current_inst, &instP, out) == 0) { \
            strcpy(error, IMMBRANCH_ERR); \
            return 1; \
        } \
        hasLabel = 1; \
        goto_label = 1; \
        labels->names[labels->num_labels].jumps = 1; \
        labels->names[labels->num_labels].pc = *lineOut; \
        strcpy(labels->names[labels->num_labels].name, out); \
        (labels->num_labels)++; \
    } \
}

#define GET_BRANCH(imm) {\
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
        case 15: \
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
    int reg1; \
    CHECK_REG(reg1); \
    int valid = regInRange(reg0, reg1); \
    if(valid == 1) { \
        outputs->outputs[*lineOut][0] = '0'; \
        outputs->outputs[*lineOut][1] = '0'; \
        outputs->outputs[*lineOut][2] = '0'; \
        outputs->outputs[*lineOut][3] = '0'; \
        int pt = 4; \
        encodeReg(&pt, outputs->outputs[*lineOut], reg0); \
        encodeOff(reg0, reg1, outputs->outputs[*lineOut], &pt); \
        outputs->outputs[*lineOut][pt] = '\0'; \
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
    if(valid == 1) { \
        outputs->outputs[*lineOut][0] = '0'; \
        outputs->outputs[*lineOut][1] = '0'; \
        outputs->outputs[*lineOut][2] = '0'; \
        outputs->outputs[*lineOut][3] = '1'; \
        int pt = 4; \
        encodeReg(&pt, outputs->outputs[*lineOut], reg0); \
        encodeOff(reg0, reg1, outputs->outputs[*lineOut], &pt); \
        outputs->outputs[*lineOut][pt] = '\0'; \
    } \
    else { \
        strcpy(error, REGISTER_OFFSET_ERR); \
        return 1; \
    } \
}

#define encLSL() { \
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
    outputs->outputs[*lineOut][0] = '0'; \
    outputs->outputs[*lineOut][1] = '0'; \
    outputs->outputs[*lineOut][2] = '1'; \
    outputs->outputs[*lineOut][3] = '0'; \
    int pt = 4; \
    encodeReg(&pt, outputs->outputs[*lineOut], reg0); \
    outputs->outputs[*lineOut][7] = bit_shift[0]; \
    outputs->outputs[*lineOut][8] = bit_shift[1]; \
    outputs->outputs[*lineOut][9] = '\0'; \
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
    outputs->outputs[*lineOut][0] = '0'; \
    outputs->outputs[*lineOut][1] = '0'; \
    outputs->outputs[*lineOut][2] = '1'; \
    outputs->outputs[*lineOut][3] = '1'; \
    int pt = 4; \
    encodeReg(&pt, outputs->outputs[*lineOut], reg0); \
    outputs->outputs[*lineOut][7] = bit_shift[0]; \
    outputs->outputs[*lineOut][8] = bit_shift[1]; \
    outputs->outputs[*lineOut][9] = '\0'; \
}

#define encLWD() { \
    int reg0; \
    CHECK_REG(reg0) \
    int reg1; \
    CHECK_REG(reg1) \
    outputs->outputs[*lineOut][0] = '0'; \
    outputs->outputs[*lineOut][1] = '1'; \
    outputs->outputs[*lineOut][2] = '0'; \
    int pt = 3; \
    encodeReg(&pt, outputs->outputs[*lineOut], reg0); \
    encodeReg(&pt, outputs->outputs[*lineOut], reg1); \
    outputs->outputs[*lineOut][pt] = '\0'; \
}

#define encSWD() { \
    int reg0; \
    CHECK_REG(reg0) \
    int reg1; \
    CHECK_REG(reg1) \
    outputs->outputs[*lineOut][0] = '0'; \
    outputs->outputs[*lineOut][1] = '1'; \
    outputs->outputs[*lineOut][2] = '1'; \
    int pt = 3; \
    encodeReg(&pt, outputs->outputs[*lineOut], reg0); \
    encodeReg(&pt, outputs->outputs[*lineOut], reg1); \
    outputs->outputs[*lineOut][pt] = '\0'; \
}

#define encBEQ() { \
    int reg0; \
    CHECK_REG(reg0) \
    char out[BUF_SIZE]; \
    int imm; \
    CHECK_IMM_BRANCH(imm) \
    if(imm != -1) { \
        GET_BRANCH(imm); \
    } \
    outputs->outputs[*lineOut][0] = '1'; \
    outputs->outputs[*lineOut][1] = '0'; \
    outputs->outputs[*lineOut][2] = '0'; \
    int pt = 3; \
    encodeReg(&pt, outputs->outputs[*lineOut], reg0); \
    outputs->outputs[*lineOut][6] = out[0]; \
    outputs->outputs[*lineOut][7] = out[1]; \
    outputs->outputs[*lineOut][8] = out[2]; \
    outputs->outputs[*lineOut][9] = '\0'; \
}

#define encBLT() { \
    int reg0; \
    CHECK_REG(reg0) \
    char out[BUF_SIZE]; \
    int imm; \
    CHECK_IMM_BRANCH(imm) \
    if(imm != -1) { \
        GET_BRANCH(imm); \
    } \
    outputs->outputs[*lineOut][0] = '1'; \
    outputs->outputs[*lineOut][1] = '0'; \
    outputs->outputs[*lineOut][2] = '1'; \
    int pt = 3; \
    encodeReg(&pt, outputs->outputs[*lineOut], reg0); \
    outputs->outputs[*lineOut][6] = out[0]; \
    outputs->outputs[*lineOut][7] = out[1]; \
    outputs->outputs[*lineOut][8] = out[2]; \
    outputs->outputs[*lineOut][9] = '\0'; \
}

#define encJMP() { \
    int imm; \
    CHECK_IMM(imm); \
    char out[7]; \
    GET_JUMP() \
    out[6] = '\0'; \
    outputs->outputs[*lineOut][0] = '1'; \
    outputs->outputs[*lineOut][1] = '1'; \
    outputs->outputs[*lineOut][2] = '0'; \
    for(int i = 0; i < 8; i++) { \
        outputs->outputs[*lineOut][3+i] = out[i]; \
    } \
    outputs->outputs[*lineOut][9] = '\0'; \
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
    outputs->outputs[*lineOut][0] = '1'; \
    outputs->outputs[*lineOut][1] = '1'; \
    outputs->outputs[*lineOut][2] = '1'; \
    int pt = 3; \
    encodeReg(&pt, outputs->outputs[*lineOut], reg0); \
    outputs->outputs[*lineOut][6] = imm_out[0]; \
    outputs->outputs[*lineOut][7] = imm_out[1]; \
    outputs->outputs[*lineOut][8] = imm_out[2]; \
    outputs->outputs[*lineOut][9] = '\0'; \
}

int hasLabel = 0;
int goto_label = 0;
int processLine(char gojoa[BUF_SIZE], /*FILE *file_out,*/ 
            int *index, char error[BUF_SIZE], int *lineOut,
            int flags, all_labels *labels, 
            char current_label[BUF_SIZE],
            all_instructions *outputs) {
    // printf("Processing line\r\n");
    // Check for label: defined by leading '.'
    if(gojoa[0] == '.') {
        hasLabel = 1;
        // printf("Detected label on line %d: ", *lineOut);
        // char label_name[BUF_SIZE];
        labels->names[labels->num_labels].name[0] = '.';
        labels->names[labels->num_labels].pc = *lineOut; // Saves current pc to label name
        current_label[0] = '.';

        // Get label name
        // printf(".");
        int i;
        for(i = 1; i < BUF_SIZE; i++) {
            if(gojoa[i] == '\n' || gojoa[i] == '\r') {
                // return 0;
                break;
            }
            else if(gojoa[i] == ' ') {
                i++;
                break;
            }
            // printf("%c", gojoa[i]);
            labels->names[labels->num_labels].name[i] = gojoa[i];
            current_label[i] = gojoa[i];
            // printf("%c", labels.names[labels.num_labels].name[i]);
        }
        // printf("heya :3 ");
        labels->names[labels->num_labels].name[i] = '\0';
        // printf("%s", labels->names[labels->num_labels].name);
        // PRINT_DELIMITER
        labels->num_labels++;
        *index = *index + i;
    }

    char OPWORD[4];
    // Get instruction opword
    OPWORD[0] = gojoa[*index];
    OPWORD[1] = gojoa[*index + 1];
    OPWORD[2] = gojoa[*index + 2];
    OPWORD[3] = '\0';
    // printf("OPWORD = %s\r\n", OPWORD);

    *index = *index + 3;
    // int hasLabel = 0;
    char current_inst[BUF_SIZE];
    current_inst[0] = '\0';
    int instP = 0;
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
        strcpy(outputs->outputs[*lineOut], NOP);
    }
    else if(strcmp(OPWORD, "DON") == 0) {
        strcpy(outputs->outputs[*lineOut], DON);
    }
    else {
        *index = 0;
        // printf("index = %d\n", *index);
        strcpy(error, INSTRUCTION_ERR);
        return 1;
    }
    if(hasLabel == 1) {
        printf("LINE %d HAS A LABEL\n", *lineOut);
        printf("current label = %s", current_label);
        for(int i = 0; i < labels->num_labels; i++) {
            if(strcmp(labels->names[i].name, labels->names[labels->num_labels-1].name) == 0) {
                if(goto_label) {
                    printf("going to label");
                    printf("evalauted true!");
                    printf("init = %d, goal = %d", labels->names[i].pc, *lineOut);
                    int val;
                    if((val = padInstructions(labels->names[i].pc, 
                        *lineOut, lineOut, outputs, labels)) == 1) {
                        break;
                    }
                    else if(val == -1) {
                        strcpy(error, BRANCH_ERR);
                        return 1;
                    }
                }
                else {
                    printf("coming from label");
                    int val;
                    if((val = padInstructions(*lineOut, 
                        labels->names[i].pc,
                        lineOut, outputs, labels)) == 1) {
                        break;
                    }
                    else if(val == -1) {
                        strcpy(error, BRANCH_ERR);
                        return 1;
                    }
                }
            } 
        }
    }
    // FPRINT_DELIMITER(file_out)
    if((flags & BIN_ONLY) == BIN_ONLY) {
        
    }
    else if((flags & INCLUDE_ASSEMBLY) == INCLUDE_ASSEMBLY) {
        // printf("Current line: %d\n", *lineOut);
        // printf("label = %s\n", current_label);
        // printf("OPWORD = %s ", OPWORD);
        // printf("current instruction = %s", current_inst);
        // printf("instP = %d", instP);
        outputs->outputs[*lineOut][9] = ' ';
        outputs->outputs[*lineOut][10] = '/';
        outputs->outputs[*lineOut][11] = '/';
        int pt = 12;
        for(int i = 0; pt < BUF_SIZE; pt++, i++) {
            // printf("i = %d", i);
            if(current_label[i] == '\0') {
                break;
            }
            outputs->outputs[*lineOut][pt] = current_label[i];
            // printf("char = %c", outputs->outputs[*lineOut][pt + i]);
        }
        current_label[0] = '\0';
        outputs->outputs[*lineOut][pt] = ' ';
        // pt++;
        outputs->outputs[*lineOut][pt + 1] = OPWORD[0];
        outputs->outputs[*lineOut][pt + 2] = OPWORD[1];
        outputs->outputs[*lineOut][pt + 3] = OPWORD[2];
        outputs->outputs[*lineOut][pt + 4] = ' ';
        printf("\n%s",outputs->outputs[*lineOut]);
        pt = pt + 5;
        if(hasLabel == 0) {
            // printf("Removing words");
            current_inst[instP-2] = '\0';
        }
        for(int i = 0; pt < BUF_SIZE; pt++, i++) {
            // printf("i = %d", i);
            if(current_inst[i] == '\0') {
                break;
            }
            // printf("current_inst[i] =%c",current_inst[i]);
            outputs->outputs[*lineOut][pt] = current_inst[i];
            // printf("outputs's insertion =%c", outputs->outputs[*lineOut][pt + i]);
        }
        outputs->outputs[*lineOut][pt] = '\0';
        
        // printf("%s",outputs->outputs[*lineOut]);
        // fprintf(file_out, " //");
        // fprintf(file_out, current_label);
        // strcpy(current_label, "");
        // fprintf(file_out, " ");
        // // fprintf(file_out, gojoa);
        // fprintf(file_out, OPWORD);
        // fprintf(file_out, " ");
        // // printf("hasLabel = %d\n", hasLabel);
        // if(hasLabel == 0) {
        //     current_inst[instP-2] = '\0';
        // }
        // fprintf(file_out, current_inst);
        // FPRINT_DELIMITER(file_out)
    }
    else {
    }
    (*lineOut)++;
    hasLabel = 0;
    goto_label = 0;
    return 0;
}


int scanR(char gojoa[BUF_SIZE], int *index, char current_inst[BUF_SIZE], int *instP) {
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
            current_inst[*instP] = 'r';
            current_inst[(*instP)+1] = gojoa[(*index)-1];
            current_inst[(*instP)+2] = ',';
            current_inst[(*instP)+3] = ' ';
            (*instP) = (*instP) + 4;
            return gojoa[(*index)-1] - '0';
        }
    }
    *index = temp;
    return -1;
}

int scanV(char gojoa[BUF_SIZE], int *index, char current_inst[BUF_SIZE], int *instP) {
    int temp = *index;
    char imm[IMM_SIZE];
    // printf("Scanning for immediate");
    for(; *index < BUF_SIZE; (*index)++) {
        if(gojoa[*index] == '\0') {
            *index = temp;
            return -1;
        }
        if(gojoa[*index] == '#') {
            (*index)++;
            current_inst[*instP] = '#';
            (*instP)++;
            int i = 0;
            for(; i < IMM_SIZE; i++) {
                if(gojoa[*index] == ' ' 
                        || gojoa[*index] == ','
                        || gojoa[*index] == '\r' 
                        || gojoa[*index] == '\n') {
                    break;
                }
                imm[i] = gojoa[(*index)++];
                current_inst[*instP] = imm[i];
                (*instP)++;
            }
            imm[i] = '\0';
            (*index)++;
            // printf("imm = %s", imm);
            // current_inst[*instP] = 
            // (*instP) = (*instP);
            current_inst[(*instP)+1] = ',';
            current_inst[(*instP)+2] = ' ';
            (*instP) = (*instP) + 3;

            return atoi(imm);
        }
    }
    *index = temp;
    // printf("Returning from here");
    return -1;
}

int scanL(char gojoa[BUF_SIZE], int *index, char current_inst[BUF_SIZE], int *instP, char label[BUF_SIZE]) {
    int temp = *index;
    // printf("Scanning for label");
    // printf("%s", gojoa);
    // printf("%d", *index);
    for(; *index < BUF_SIZE; (*index)++) {
        // printf("%c\n", gojoa[*index]);
        // if(gojoa[*index] == '\0') {
        //     return 0;
        // }
        if(gojoa[*index] == '.') {
            (*index)++;
            current_inst[*instP] = '.';
            label[0] = '.';
            int i = 1;
            (*instP)++;
            for(; *index < BUF_SIZE; (*index)++, (*instP)++, i++) {
                if(gojoa[*index] == '\n' || gojoa[*index] == '\r') {
                    // return 0;
                    break;
                }
                else if(gojoa[*index] == ' ') {
                    break;
                }
                // printf("gojoa[*index] = %c", gojoa[*index]);
                current_inst[*instP] = gojoa[*index];
                label[i] = gojoa[*index];
            }
            label[i] = '\0';
            current_inst[*instP]= '\0';
            // printf("current_inst = %s\n", current_inst);
            // printf("label = %s", label);
            return 1;
        }
    }
    *index = temp;
    return 0;
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

void encodeOff(int reg0, int reg1, char out[BUF_SIZE], int *pt) {
    char in[3];
    switch(reg1 - reg0) {
        case 1:
        case -7:
            strcpy(in, "00");
            break;
        case 2:
        case -6:
            strcpy(in, "01");
            break;
        case 3:
        case -5:
            strcpy(in, "10");
            break;
        case 4:
        case -4:
            strcpy(in, "11");
            break;
    }
    out[*pt] = in[0];
    out[*pt+1] = in[1];
    (*pt) = (*pt) + 2;
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

int padInstructions(int init, int goal, int *currPC, 
        all_instructions *outputs, all_labels *labels) {
    // int oldPC = *currPC;
    int cmp = init - goal;
    printf("cmp = %d\n", cmp);
    switch(cmp) {
        case -5: 
            outputs->outputs[goal][6] = '0';
            outputs->outputs[goal][7] = '0';
            outputs->outputs[goal][8] = '0';
            break;
        case 5: 
            outputs->outputs[goal][6] = '0';
            outputs->outputs[goal][7] = '0';
            outputs->outputs[goal][8] = '1';
            break;
        case 20: 
            outputs->outputs[goal][6] = '0';
            outputs->outputs[goal][7] = '1';
            outputs->outputs[goal][8] = '0';
            break;
        case -20: 
            outputs->outputs[goal][6] = '0';
            outputs->outputs[goal][7] = '1';
            outputs->outputs[goal][8] = '1';
            break;
        case 10: 
            outputs->outputs[goal][6] = '1';
            outputs->outputs[goal][7] = '0';
            outputs->outputs[goal][8] = '0';
            break;
        case -10: 
            printf("%s", outputs->outputs[goal]);
            outputs->outputs[goal][6] = '1';
            outputs->outputs[goal][7] = '0';
            outputs->outputs[goal][8] = '1';
            break;
        case 15: 
            outputs->outputs[goal][6] = '1';
            outputs->outputs[goal][7] = '1';
            outputs->outputs[goal][8] = '0';
            break;
        case -50: 
            outputs->outputs[goal][6] = '1';
            outputs->outputs[goal][7] = '1';
            outputs->outputs[goal][8] = '1';
            break;
        case 0: 
            return 0;
        default: // Non trivial case
            int insert_amount = 20;
            int final_diff;
            if(cmp > 20) {
                return -1;
            }
            else if(cmp < -50) {
                return -1;
            }
            if(cmp > 0) {
                const int vals[] = {5, 10, 15, 20};
                for(int i = 0; i < 4; i++) {
                    if(vals[i] - cmp > 0) {
                        if(insert_amount > vals[i] - cmp) {
                            insert_amount = vals[i] - cmp;
                            final_diff = vals[i];
                        } 
                    }
                }
            }
            else { // cmp < 0
                insert_amount = 50;
                const int vals[] = {-50, -20, -10, -5};
                for(int i = 0; i < 4; i++) {
                    printf("val[%d] = %d", i, vals[i]);
                    printf("cmp - vals[%d] = %d", i, vals[i] - cmp);
                    printf("insert_amount = %d", insert_amount);
                    if(cmp - vals[i] > 0) {
                        if(insert_amount > cmp - vals[i]) {
                            insert_amount = cmp - vals[i];
                            final_diff = vals[i];
                        }
                    }
                }
            }
            printf("\n");
            printf("insert amount= %d", insert_amount);
            printf("final_diff = %d", final_diff);
            char temp[BUF_SIZE];
            strcpy(temp, outputs->outputs[*currPC]);
            for(int i = 0; i < insert_amount; i++) {
                strcpy(outputs->outputs[(*currPC) + i], NOP);
            }
            (*currPC) = (*currPC) + insert_amount;
            strcpy(outputs->outputs[*currPC], temp);
            switch(final_diff) {
                case -5: 
                    outputs->outputs[*currPC][6] = '0';
                    outputs->outputs[*currPC][7] = '0';
                    outputs->outputs[*currPC][8] = '0';
                    break;
                case 5: 
                    outputs->outputs[goal][6] = '0';
                    outputs->outputs[goal][7] = '0';
                    outputs->outputs[goal][8] = '1';
                    break;
                case 20: 
                    outputs->outputs[goal][6] = '0';
                    outputs->outputs[goal][7] = '1';
                    outputs->outputs[goal][8] = '0';
                    break;
                case -20: 
                    outputs->outputs[*currPC][6] = '0';
                    outputs->outputs[*currPC][7] = '1';
                    outputs->outputs[*currPC][8] = '1';
                    break;
                case 10: 
                    outputs->outputs[goal][6] = '1';
                    outputs->outputs[goal][7] = '0';
                    outputs->outputs[goal][8] = '0';
                    break;
                case -10: 
                    printf("%s", outputs->outputs[goal]);
                    printf("goal = %d", goal);
                    outputs->outputs[*currPC][6] = '1';
                    outputs->outputs[*currPC][7] = '0';
                    outputs->outputs[*currPC][8] = '1';
                    break;
                case 15: 
                    outputs->outputs[goal][6] = '1';
                    outputs->outputs[goal][7] = '1';
                    outputs->outputs[goal][8] = '0';
                    break;
                case -50: 
                    outputs->outputs[*currPC][6] = '1';
                    outputs->outputs[*currPC][7] = '1';
                    outputs->outputs[*currPC][8] = '1';
                    break;
                case 0: 
                    return 0;
            }
        }
        // Update oldpc
        labels->names[labels->num_labels-1].pc = *currPC;
    return 1;
}

void encodeReg(int* pt, char out[BUF_SIZE], int reg) {
    char in[4];
    switch(reg) {
        case 0:
            strcpy(in, r0); 
            break;
        case 1:
            strcpy(in, r1); 
            break;
        case 2:
            strcpy(in, r2); 
            break;
        case 3:
            strcpy(in, r3); 
            break;
        case 4:
            strcpy(in, r4); 
            break;
        case 5:
            strcpy(in, r5); 
            break;
        case 6:
            strcpy(in, r6); 
            break;
        case 7:
            strcpy(in, r7); 
            break;
    }
    // printf("in = %s, pt = %d", in, *pt);
    out[*pt] = in[0];
    out[*pt+1] = in[1];
    out[*pt+2] = in[2];
    (*pt) = (*pt) + 3;
    // printf("out = %s", out);
}

void encodeImm(int* pt, char[BUF_SIZE], const char[IMM_SIZE]) {

}
