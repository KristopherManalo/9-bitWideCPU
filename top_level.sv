`include "globals.svh"

module top_level(
    input clk, reset, req,
    output logic done
);


wire[INSTRUCTION_WIDTH-1:0] instruction;

// PC wires
wire[INSTRUCTION_MEM_WIDTH-1:0] program_counter, target_counter;
wire relative_en, absolute_en;

// Instruction mem wires
// wire [INSTRUCTION_MEM_WIDTH-1:0]

// Control wires
wire[1:0] ALUfunct;
wire ALUop, ALU_and, reg_write_en, load_mem_en, store_mem_en,
    branch_en, branch_eq, branch_lt, imm_en, reg_target_overwrite;

// Reg file wires
wire[REG_PWIDTH-1:0] reg_pA, reg_pB, reg_pWr;
wire[WORD_WIDTH-1:0] reg_dat_in, reg_dat_outA, reg_dat_outB;

// ALU wires
wire[1:0] alu_cmd, funct;
wire[WORD_WIDTH-1:0] alu_inA, alu_inB, alu_result;
wire shift_carry_in, shift_carry_out, alu_zero, alu_lt;

// Data mem wires
wire[WORD_WIDTH-1:0] dat_in, dat_address, dat_out;


PC pc1(
    .reset,
    .clk,
    .relative_en,
    .absolute_en,
    .target_address(target_counter),

    .ctr_next(program_counter)
);

instruction_mem im1(
    .ctr(program_counter),
    .instruction(instruction)
);

Control ctrl(
    .op(instruction[8:5]),
    .reg_source(instruction[5:3]),
    .*
);

reg_file reg1(
    .clk,
    .wr_en(reg_write_en),
    .dat_in(reg_dat_in),
    .read_address_A(reg_pA),
    .read_address_B(reg_pB),
    .wr_address(reg_pWr),

    .dat_out_A(reg_dat_outA),
    .dat_out_B(reg_dat_outB)
);

alu alu1(
    .alu_cmd,
    .funct,
    .inA(alu_inA),
    .inB(alu_inB),
    .shift_carry_in,

    .result(alu_result),
    .shift_carry_out,
    .zero(alu_zero),
    .lt(alu_lt)
);

dat_mem dm1(
    .dat_in,
    .clk,
    .wr_en(store_mem_en),
    .rd_en(load_mem_en),
    .address(dat_address),

    .dat_out
);


// Determines data_in for alu
always_comb begin
    if(reg_target_overwrite) begin
        alu_inA = reg_dat_outA
    end
    else begin
        alu_inA = '0;
    end
    if(imm_en)begin
        // sign extended immediate 
    end
    else begin
        alu_inB = reg_dat_outB;
    end
end

always_comb begin
    if(reg_target_overwrite)
        reg_pWr == 3'b111;
    else 
        reg_pWr == instruction[5:3];
end

assign done = (program_counter == 2**INSTRUCTION_MEM_WIDTH-1);

endmodule
