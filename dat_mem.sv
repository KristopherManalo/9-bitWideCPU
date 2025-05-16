module dat_mem (
    input[7:0]  dat_in,
    input       clk,
                wr_en,
                rd_en,
                reset,
    input[7:0]  address,
    output logic[7:0]   dat_out
);
    logic[7:0] core[256];

    assign dat_out = core[address];

    always_ff @(posedge clk) begin
        if(reset) begin
            for(int i = 0; i < 256; i = i + 1) begin
                core[i] <= 0;
            end
        end
        else if(wr_en) begin
            core[address] <= dat_in;
        end
    end
endmodule