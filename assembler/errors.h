#ifndef ERRORS
#define ERRORS
    #define INSTRUCTION_ERR         "Illegal instrucion"
    #define REGISTER_ERR            "Illegal register address"
    #define REGISTER_OFFSET_ERR     "Offset Register out of range of Base register"
    #define IMMEDIATE_ERR           "Illegal immediate"
    #define BRANCH_ERR              "Illegal branch offset"
    #define JUMP_ERR                "Illegal jump address: [0, 63]"
    #define IMMBRANCH_ERR           "Immediate or label not found"
    #define ILLEGAL_LABEL           "Labels are illegal without \"-r\" flag"
#endif
