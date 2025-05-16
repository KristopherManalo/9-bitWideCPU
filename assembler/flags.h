#ifndef FLAGS
#define FLAGS
    #define FLAGS_NUMBER 5

    #define INCLUDE_ASSEMBLY    0x80000000
    #define REORGANIZE_ASM      0x40000000
    #define OUTPUT_DEFINED      0x20000000
    #define BIN_ONLY            0x10000000
    #define SHOW_FLAGS          0x08000000
    
    /**
     * Prints help documentation when help flag provided
     */
    void printHelp(char*);
    void printFlags(int);
#endif
