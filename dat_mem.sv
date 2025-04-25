module dat_mem (
    input[7:0]  dat_in,
    input       clk,
                wr_en,
                rd_en,
    input[7:0]  address,
    output logic[7:0]   dat_out
);
    logic[7:0] core[256];

    assign dat_out = core[address];

    always_ff @(posedge clk) begin
        if(wr_en) begin
            core[address] <= dat_in;
        end
    end
endmodule