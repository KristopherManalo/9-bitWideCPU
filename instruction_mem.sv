`include "globals.svh"
module instruction_mem(
    input           [INSTRUCTION_MEM_WIDTH-1:0] ctr,
    output logic    [INSTRUCTION_WIDTH-1:0] instruction
);
    logic [INSTRUCTION_WIDTH-1:0] core[2**INSTRUCTION_MEM_WIDTH];
    // initial begin
    //     $readmemb("mach_code.txt", core);
    // end 
    assign instruction = core[ctr];

endmodule