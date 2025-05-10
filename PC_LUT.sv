`include "globals.svh"
module PC_LUT(
    input [2:0] address,
    output logic[7:0] target_address
);

    // TODO: Decide on permanent LUT values
    // Values decided on as of Milestone 2
    always_comb begin
        target_address = 8'b00000000;
        case(address) 
            3'b000:
                target_address = 8'b11111011; // -5
            3'b001:
                target_address = 8'b00000101; // 5
            3'b010:
                target_address = 8'b00010100; // 20
            3'b011:
                target_address = 8'b11101100; // -20
            3'b100:
                target_address = 8'b00001010; // 10
            3'b101:
                target_address = 8'b11110110; // -10
            3'b110:
                target_address = 8'b00001100; // 15
            3'b111:
                target_address = 8'b00110010; // -50
        endcase
    end
endmodule