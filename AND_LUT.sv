`include "globals.svh"
module AND_LUT(
    input [1:0] address,
    output logic[WORD_WIDTH-1:0] bitmask
);
    always_comb begin
        case(address)
            0:
                bitmask = `MANTISSA;
            1:
                bitmask = `EXPONENT;
            2:
                bitmask = 0;
            3:
                bitmask = -1;
        endcase
    end
endmodule