`include "globals.svh"

module top_level(
    input clk, reset, req,
    output logic done
);


logic[INSTRUCTION_WIDTH-1:0] instruction;

// PC wires
logic[INSTRUCTION_MEM_WIDTH-1:0] program_counter, target_counter;
logic relative_en, absolute_en;
logic[INSTRUCTION_MEM_WIDTH-1:0] pc_in;

// AND_LUT wires
// logic[WORD_WIDTH-1:0] and_lut_out;

// Control wires
logic[1:0] ALUfunct;
logic ALUop, ALU_and, ALU_rol, reg_write_en, load_mem_en, store_mem_en,
    branch_ab, branch_en, branch_eq, branch_lt, imm_en, inA_zero, reg_target_overwrite;

// Reg file wires
logic[REG_PWIDTH-1:0] reg_pA, reg_pB, reg_pWr;
logic[WORD_WIDTH-1:0] reg_dat_in, reg_dat_outA, reg_dat_outB;

// ALU wires
// logic alu_zeroQ;
// logic[1:0] alu_cmd, funct;
logic[WORD_WIDTH-1:0] alu_inA, alu_inB, alu_result;
logic shift_carry_in = 0, shift_carry_out;
logic alu_zero, alu_lt;

// Data mem wires
logic[WORD_WIDTH-1:0] dat_in, dat_address, dat_out;

// logic shift_clear = 0, shift_carry_en = 0;

PC_LUT pclut1(
    .address(instruction[2:0]),
    .target_address(target_counter)
);
// AND_LUT andlut1(
//     .address(instruction[1:0]),
//     .bitmask(and_lut_out)
// );

PC pc1(
    .reset,
    .clk,
    .relative_en,
    .absolute_en,
    .target_address(pc_in),

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
    .reset,
    .wr_en(reg_write_en),
    .dat_in(reg_dat_in),
    .read_address_A(reg_pA),
    .read_address_B(reg_pB),
    .wr_address(reg_pWr),

    .dat_out_A(reg_dat_outA),
    .dat_out_B(reg_dat_outB)
);

alu alu1(
    .alu_cmd(instruction[8:7]),
    .funct(instruction[6:5]),
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
    .reset,
    .wr_en(store_mem_en),
    .rd_en(load_mem_en),
    .address(dat_address),

    .dat_out
);

// Determines target_adress => PC
always_comb begin
    absolute_en = 0;
    relative_en = 0;
    pc_in = 0;
    if(req == 1) begin
        relative_en = 1;
        pc_in = 0;
    end
    else if(branch_ab) begin
        absolute_en = 1;
        pc_in = (instruction[5:0] << 4);
    end
    else if(branch_eq && alu_zero) begin
        relative_en = 1;
        pc_in = target_counter;
    end
    else if(branch_lt && alu_lt) begin
    // else begin
        relative_en = 1;
        pc_in = target_counter;
    end
end

// Determines dat_mem
always_comb begin
    dat_in = reg_dat_outA;
    dat_address = reg_dat_outB;
end

// Determines pointers that go into reg_file
always_comb begin
    if(load_mem_en) begin
        reg_dat_in = dat_out;
    end
    else begin
        reg_dat_in = alu_result;
    end

    if(imm_en) begin
        if(instruction[5:3] == 3'b000) begin
            reg_pA = 3'b000;
        end
        reg_pA = 3'b111;
    end
    else if(reg_write_en) begin // If writing to register
        reg_pA = instruction[4:2];
        case(instruction[1:0]) 
            2'b00:
                reg_pB = instruction[4:2] + 3'b001;
            2'b01:
                reg_pB = instruction[4:2] + 3'b010;
            2'b10:
                reg_pB = instruction[4:2] + 3'b011;
            2'b11:
                reg_pB = instruction[4:2] + 3'b100;
        endcase
    end
    else if(branch_en) begin // If branching
        reg_pA = instruction[4:2];
        // Changed to comparing register and register + 1
        reg_pB = instruction[4:2] + 1'b1; 
    end
    else begin // If loading/storing to memory
        reg_pA = instruction[5:3];
        reg_pB = instruction[2:0];
    end
end
always_comb begin
    if(reg_target_overwrite)
        reg_pWr = 3'b111;
    else 
        reg_pWr = instruction[5:3];
end

// Determines data_in for alu
always_comb begin
    // 
    if(!inA_zero) begin
        alu_inA = reg_dat_outA;
    end
    else begin
        alu_inA = 8'b0;
    end
    if(imm_en)begin
        // sign extended immediate 
        if(instruction[2] == 1'b1) begin
            alu_inB = {{5{1'b1}}, instruction[2:0]};
        end
        else begin
            alu_inB = {{5{1'b0}}, instruction[2:0]};
        end
    end
    else if(ALU_rol) begin
        case(instruction[1:0])
            2'b00:
                alu_inB = 8'b00000001;
            2'b01:
                alu_inB = 8'b00000010;
            2'b10:
                alu_inB = 8'b00000011;
            2'b11:
                alu_inB = 8'b00000111;
        endcase 
    end
    else begin
        alu_inB = reg_dat_outB;
    end
end

// always_ff @(posedge clk) begin
//     alu_zeroQ <= alu_zero;
//     if(shift_clear)
//         shift_carry_in <= 'b0;
//     else if(shift_carry_en) 
//         shift_carry_in <= shift_carry_out;
// end

assign done = (instruction == 9'b000111100);

endmodule
