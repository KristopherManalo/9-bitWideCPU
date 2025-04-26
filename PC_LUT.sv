`include "globals.svh"
module PC_LUT(
    input [2:0] address,
    // input       branch_eq,
    //             branch_lt,
    output logic[INSTRUCTION_MEM_WIDTH-1:0] target_address
);

    // TODO: Decide on permanent LUT values
    always_comb begin
        target_address = 'b0;
        case(address) 
            0:
                target_address = 20;
            1:
                target_address = -20;
            2:
                target_address = 5;
            3:
                target_address = -5;
            4:
                target_address = 50;
            5:
                target_address = -50;
            6:
                target_address = 10;
            7:
                target_address = -10;
        endcase
    end
endmodule