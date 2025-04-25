`ifndef GLOBALS
`define GLOBALS
    parameter OPCODE_WIDTH = 4;
    parameter REG_PWIDTH = 3;
    parameter INSTRUCTION_WIDTH = 9;
    parameter INSTRUCTION_MEM_WIDTH = 10;
    parameter WORD_WIDTH = 8;
    parameter ALU_WIDTH = 4;
`endif

`ifndef BOOL
`define BOOL
    // typedef enum logic {
    //     TRUE = 1'b1,
    //     FALSE = 1'b0;
    // } bool
    `define TRUE  1
    `define FALSE  0
`endif

`ifndef BITMASK
`define BITMASK
    `define MANTISSA 8'b01110000
    `define EXPONENT 8'b00001111
`endif