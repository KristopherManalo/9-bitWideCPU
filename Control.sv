`include "globals.svh"

module Control (
    input [OPCODE_WIDTH-1:0] op,
    input [REG_PWIDTH-1:0] reg_source,
    output logic [1:0]  ALUfunct,
    output logic    ALUop,
                    ALU_and,
                    reg_write_en,
                    load_mem_en,
                    store_mem_en,
                    branch_en,
                    branch_eq,
                    branch_lt,
                    imm_en,
                    reg_target_overwrite // If true, writes to R7
);

    always_comb begin
        ALUfunct = '0;
        ALUop = 0;
        ALU_and = 0;
        reg_write_en = 0;
        load_mem_en = 0;
        store_mem_en = 0;
        branch_en = 0;
        branch_eq = 0;
        branch_lt = 0;
        imm_en = 0;
        reg_target_overwrite = 1;
        case(op[3:2])
            'b00: begin
                ALUop = 1;
                ALUfunct = op[1:0];
                // switch(op[1:0]) begin
                //     'b00:
                //         ALUfunct = 'b00;
                //     'b01:
                //     'b10:
                //     'b11:
                // end
            end
            'b01: begin
                if(op[1] == 'b0) begin
                    load_mem_en = 1;
                end
                else if (op[1] == 'b1) begin
                    store_mem_en = 1;
                end
            end
            'b10: begin
                if(op[1] == 'b0) begin
                    branch_eq = 1;
                end
                else if(op[1] == 'b1) begin
                    branch_lt = 1;
                end
            end
            'b11: begin
                if(op[1] == 'b0) begin
                    branch_en = 1;
                end
                else if(op[1] == 'b1) begin
                    if(reg_source != 0) begin
                        reg_target_overwrite = 0;
                    end
                    imm_en = 1;
                end
            end
        endcase
    end

endmodule