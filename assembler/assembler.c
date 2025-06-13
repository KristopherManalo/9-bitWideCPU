#include "assembler.h"
#include "encoders.h"
#include "errors.h"
#include "flags.h"


#define PRINT_LABELS 1

/**
 * Reads in a freaky gojo assembly language file
 * Outputs freaky gojo machine language file
 */
int main(int argc, char *argv[]) {
    if(argc < 2) {
       printf("Proper usage: %s [input] [OPTIONS]", argv[0]);
       return 1;
    }

    if(strcmp(argv[1], "-h") == 0) {
        printHelp(argv[0]);
        return 0;
    }

    char file_in_name[MAX_FILE_NAME];
    char file_out_name[MAX_FILE_NAME];

    // [31] - print assembly        -a
    // [30] - reorganize script     -r
    // [29] - output file defined   -o
    // [28] - output as binary      -b
    // [27] - display flags         -f
    __int32 flags = 0; 

    strcpy(file_in_name, argv[1]);
    // printf("argc == %d\r\n", argc);

    if(argc >= 3) {
        // For loop in case want to add more flags later
        // Flags to check: o, c, r, b, h, f
        for (int i = 2; i < argc; i++) {
            // printf("argv[%d] = %s\r\n", i, argv[i]);
            if(strcmp(argv[i], "-h") == 0) {
                printHelp(argv[0]);
                return 0;
            }
            else if(strcmp(argv[i], "-o") == 0) {
                if(argc <= i + 1) {
                    fprintf(stderr, "Expected file name after -o flag");
                    return 1;
                }
                i++;
                strcpy(file_out_name, argv[i]);
            }
            else if(strcmp(argv[i], "-a") == 0) { // Includes the original assembly at end of instruction
                flags = flags | INCLUDE_ASSEMBLY;
            }
            else if(strcmp(argv[i], "-r") == 0) {
                flags = flags | REORGANIZE_ASM;
            }
            else if(strcmp(argv[i], "-b") == 0) {
                flags = flags | BIN_ONLY;
            }
            else if(strcmp(argv[i], "-f") == 0) {
                flags = flags | SHOW_FLAGS;
            }
            else {
                fprintf(stderr, "Invalid flag: %s", argv[i]);
                return 1;
            }
        }
    }

    if((flags & SHOW_FLAGS) == SHOW_FLAGS) {
        printFlags(flags);
    }

    if((flags & OUTPUT_DEFINED) != OUTPUT_DEFINED) {
        // Handles default output name creation
        // Default file extension is '.fgo'
        for(int i = 0; i < MAX_FILE_NAME; i++) {
            file_out_name[i] = file_in_name[i];
            // printf("file_in_name[i] = %c\r\n", file_in_name[i]);
            if(file_in_name[i] == '.') {
                file_out_name[i] = '.';
                file_out_name[i+1] = 'f';
                file_out_name[i+2] = 'g';
                file_out_name[i+3] = 'o';
                file_out_name[i+4] = '\0';
                break;
            }
        }
    }


    FILE *file_in = fopen(file_in_name, "r");
    if(file_in == NULL) {
        fprintf(stderr, "Error opening input file");
        return 1;
    }

    char buffer[BUF_SIZE];
    char error[BUF_SIZE];
    char current_label[BUF_SIZE];
    current_label[0] = '\0';

    // char outputs[NUM_LINES][BUF_SIZE];
    all_instructions *outputs = (all_instructions*)malloc(sizeof(all_instructions));

    int line = 1;
    int lines_written = 0; // 1?
    int character = 0;

    all_labels *labels = (all_labels*)malloc(sizeof(all_labels));
    labels->num_labels = 0;
    while(fgets(buffer, sizeof(buffer), file_in) != NULL) {
        // printf("%s\r\n", buffer);
        character = 0;
        // current_label[0] = '\0';
        if(processLine(buffer, /*file_out,*/ &character, error, 
                &lines_written, flags, labels, current_label,
                outputs) != 0) {
            fprintf(stderr, "Error processing line %d:%d: ", line, character);
            fprintf(stderr, error);
            PRINT_DELIMITER
            return 1;
        }
        line++;
    }
    // padInstructions(8,18, &line, outputs);

    FILE *file_out = fopen(file_out_name, "w");
    if(file_out == NULL) {
        fprintf(stderr, "Error creating output file");
        return 1;
    }

    for(int i = 0; i < lines_written; i++) {
        fprintf(file_out, outputs->outputs[i]);
        FPRINT_DELIMITER(file_out)
    }

    // Appends rest of output file with "DON" flags
    if((flags & BIN_ONLY) == BIN_ONLY) {
        // char DON_BIN[9];     // TODO?? Remove /0 delimiter from strings to
        // DON_BIN[0] = '0';    // get desired effect????
        // DON_BIN[1] = '0';
        // DON_BIN[2] = '0';
        // DON_BIN[3] = '1';
        // DON_BIN[4] = '1';
        // DON_BIN[5] = '1';
        // DON_BIN[6] = '1';
        // DON_BIN[7] = '0';
        // DON_BIN[8] = '0';
        // for(int i = lines_written; i < NUM_LINES; ++i) {
        //     fprintf(file_out, DON_BIN);
        // }
        for(int i = lines_written; i < NUM_LINES; ++i) {
            fprintf(file_out, DON);
        }
    }
    else if((flags & INCLUDE_ASSEMBLY) == INCLUDE_ASSEMBLY) {
        for(int i = lines_written; i < NUM_LINES; ++i) {
            fprintf(file_out, DON);
            fprintf(file_out, " // DON");
            FPRINT_DELIMITER(file_out)
        }
    }
    else {
        for(int i = lines_written; i < NUM_LINES; ++i) {
            fprintf(file_out, DON);
            FPRINT_DELIMITER(file_out)
        }
    }

    fclose(file_in);
    fclose(file_out);

    #if PRINT_LABELS
        // for(int i = 0; i < MAX_LABELS; i++) {
        for(int i = 0; i < labels->num_labels; i++) {
            printf("Label %d: name = %s, pc = %d, goes to? %d",
                i, labels->names[i].name, labels->names[i].pc, 
                labels->names[i].jumps);
            PRINT_DELIMITER
        }
    #endif
    free(labels);

    return 0;
}
