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

logic ADD_en, LSL_en, /*LSR_en,*/ AND_en, XOR_en;

always_comb begin
    result = 'b0;
    shift_carry_out = 'b0;
    // zero = !result;
    zero = (inA == inB);
    // pari = ^result; 
    lt = inA < inB;

    AND_en = (funct == 'b00);
    ADD_en = ((alu_cmd == 'b11 && funct[1] == 'b1) || funct == 'b01);
    LSL_en = (funct == 'b10);
    // LSR_en = (funct == 'b11);
    // Since rolling shifter is simply the inverse of LSL, replace LSR with
    // XOR allows to easily find 2's complement instead of rolling shift and
    // subbing my imm 1, 8 times
    XOR_en = (funct == 'b11);

    if(AND_en) begin
        result = inA & inB;
    end
    else if(ADD_en) begin
        {shift_carry_out, result} = inA + inB + shift_carry_in;
    end
    else if(LSL_en) begin 
        // {shift_carry_out, result} = {inA, shift_carry_in};
        // {shift_carry_out}
        // {shift_carry_out, result} = {inA, inA[7]};
        result = (inA << inB) | (inA >> (8 - inB));
    end
    // else if(LSR_en) begin
    //     // {result,shift_carry_out} = {shift_carry_in, inA};
    //     {result, shift_carry_out} = {inA[0], inA};
    // end
    else if (XOR_en) begin
        result = inA ^ inB;
    end
    else begin
        {shift_carry_out, result} = inA + inB + shift_carry_in;
    end

end

endmodule