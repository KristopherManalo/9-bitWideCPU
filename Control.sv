`include "globals.svh"

module Control (
    input [OPCODE_WIDTH-1:0] op,
    input [REG_PWIDTH-1:0] reg_source,
    output logic [1:0]  ALUfunct,
    output logic    ALUop,
                    ALU_and,
                    ALU_rol,
                    reg_write_en,
                    load_mem_en,
                    store_mem_en,
                    branch_ab,
                    branch_en,
                    branch_eq,
                    branch_lt,
                    branch_co,
                    imm_en,
                    inA_zero,
                    reg_target_overwrite, // If true, writes to R7
                    save_pc,
                    go_link
);

    always_comb begin
        ALUfunct = '0;
        ALUop = 0;
        ALU_and = 0;
        ALU_rol = 0;
        reg_write_en = 0;
        load_mem_en = 0;
        store_mem_en = 0;
        branch_ab = 0;
        branch_en = 0;
        branch_eq = 0;
        branch_lt = 0;
        branch_co = 0;
        imm_en = 0;
        inA_zero = 0;
        reg_target_overwrite = 1;
        save_pc = 0;
        go_link = 0;
        case(op[3:2])
            'b00: begin // ALU
                reg_write_en = 1;
                ALUop = 1;
                ALUfunct = op[1:0];
                case(op[1:0]) 
                    'b00: // AND
                        ALUfunct = 'b00;
                    'b01: // AND
                        ALUfunct = 'b01;
                    'b10:   // left roll
                        ALU_rol = 1;
                    // 'b11:   // right roll
                    //     ALU_rol = 1;
                    'b11: // XOR
                        ALUfunct = 'b11;
                endcase
            end
            'b01: begin
                if(op[1] == 'b0) begin
                    reg_target_overwrite = 0;
                    reg_write_en = 1;
                    load_mem_en = 1;
                end
                else if (op[1] == 'b1) begin
                    store_mem_en = 1;
                end
            end
            'b10: begin
                // relative branch
                if(op[1] == 'b0) begin
                    branch_en = 1;
                    branch_eq = 1;
                    // save_pc = 1;
                end
                else if(op[1] == 'b1) begin
                    branch_en = 1;
                    branch_lt = 1;
                    // save_pc = 1;
                end
            end
            'b11: begin
                // if(op[1] == 'b0) begin
                //     branch_en = 1;
                //     branch_ab = 1;
                // end
                if(op[1:0] == 'b00) begin
                    branch_en = 1;
                    branch_ab = 1;
                    save_pc = 1;
                end
                else if(op[1:0] == 'b01) begin
                    // go to link register
                    // go_link = 1;
                    branch_en = 1;
                    branch_co = 1;
                end
                else if(op[1] == 'b1) begin
                    reg_write_en = 1;
                    if(reg_source != 0) begin
                        reg_target_overwrite = 0;
                    end
                    else begin
                        inA_zero = 1;
                    end
                    imm_en = 1;
                end
            end
        endcase
    end

endmodule