#include "assembler.h"
#include "encoders.h"
#include "errors.h"

#define NUM_LINES 1024


/**
 * Reads in a freaky gojo assembly language file
 * Outputs freaky gojo machine language file
 */
int main(int argc, char *argv[]) {
    if(argc < 2) {
       printf("Proper usage: %s <input> -o <output>", argv[0]);
       return 1;
    }

    char file_in_name[MAX_FILE_NAME];
    char file_out_name[MAX_FILE_NAME];

    strcpy(file_in_name, argv[1]);
    // printf("argc == %d\r\n", argc);

    if(argc >= 4) {
        // For loop in case want to add more flags later
        // Flags to check: o
        for (int i = 2; i < argc; i++) {
            // printf("argv[%d] = %s\r\n", i, argv[i]);
            if(strcmp(argv[i], "-o") == 0) {
                strcpy(file_out_name, argv[i+1]);
            }
        }
    }
    else if(argc == 3) {
        // if(strcmp(argv[2], "-o") == 0) {
            printf("Proper usage: %s <input> -o <output>", argv[0]);
            return 1;
        // }
    }
    else {
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
    // printf("file out = %s\r\n", file_out_name);


    FILE *file_in = fopen(file_in_name, "r");
    if(file_in == NULL) {
        printf("Error opening input file");
        return 1;
    }
    FILE *file_out = fopen(file_out_name, "w");
    if(file_out == NULL) {
        printf("Error creating output file");
        return 1;
    }

    char buffer[BUF_SIZE];
    char error[BUF_SIZE];
    int line = 1;
    int lines_written = 1;
    int character = 0;
    while(fgets(buffer, sizeof(buffer), file_in) != NULL) {
        // printf("%s\r\n", buffer);
        character = 0;
        if(processLine(buffer, file_out, &character, error, &lines_written) != 0) {
            printf("Error processing line %d:%d: ",line, character);
            printf(error);
            PRINT_DELIMITER
            return 1;
        }
        line++;
    }

    // Appends rest of output file with "DON" flags
    for(int i = lines_written; i < NUM_LINES; ++i) {
        fprintf(file_out, DON);
        FPRINT_DELIMITER(file_out)
    }

    fclose(file_in);
    fclose(file_out);

    return 0;
}
