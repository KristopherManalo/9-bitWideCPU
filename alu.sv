`include "globals.svh"

module alu(
    input [1:0]             alu_cmd,
    input [1:0]             funct,
    input [WORD_WIDTH-1:0]  inA, inB,
    input                   shift_carry_in,
    output logic[7:0]       result,
    output logic            shift_carry_out,
                            // pari,
                            zero,
                            lt
);

always_comb begin
    result = 'b0;
    shift_carry_out = 'b0;
    zero = !result;
    // pari = ^result; 
    lt = inA < inB;

    case(alu_cmd)
        2'b00: begin
            case(funct)
                2'b00: begin // AND
                    result = inA & inB;
                    // case(bottom3[1:0])
                    //     2'b00:
                    //         result = inA & `MANTISSA;
                    //     2'b10:
                    //         result = inA & `EXPONENT;
                    //     // 2'b11:
                    //     //     result = inA & `EXPONENT;
                    // endcase
                end
                2'b01: begin // ADD
                    {shift_carry_out, result} = inA + inB + shift_carry_in;
                end
                2'b10: begin // LSL
                    {shift_carry_out, result} = {inA, shift_carry_in};
                end
                2'b11: begin // LSR
                    {result,shift_carry_out} = {shift_carry_in, inA};
                end
            endcase
        end

        // 2'b01: begin // Memory management
        //     case(funct[1])
        //         2'b0: begin // LWD
        //         end
        //         2'b1: begin // SWD
        //         end
        //     endcase
        // end
        // 2'b10: begin
        //     case(funct[1])
        //         2'b0: begin // BEQ
        //         end
        //         2'b1: begin // BLT
        //         end
        //     endcase
        // end
        2'b11: begin
            if(funct[1] == 1) begin // ADI
                // TODO: Implement signed behavior
                result = inA + inB + shift_carry_in;
            end
        //     case(funct[1])
        //         2'b0: begin
        //         end
        //         2'b1: begin
        //         end
        //     endcase
        end
    endcase
end

endmodule