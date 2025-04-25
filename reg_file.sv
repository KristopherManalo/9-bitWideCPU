`include "globals.svh"
module reg_file ( // 8 registers
    input   [WORD_WIDTH-1:0]        dat_in,
    input                           clk,
                                    wr_en,
    input   [REG_PWIDTH-1:0]        read_address_A,
                                    read_address_B,
                                    wr_address,
    // input   [OFF_WIDTH-1:0]         offset, // defines offset from address A
    output logic    [7:0]           dat_out_A,
                                    dat_out_B
);

    logic[WORD_WIDTH-1:0] core[2**REG_PWIDTH];

    always_ff @(posedge clk) begin
        if(wr_en) begin
            // core[wr_address] <= dat_in;
            core[wr_address] <= dat_in;
        end
    end

    assign dat_out_A = core[read_address_A];
    assign dat_out_B = core[read_address_B];
endmodule