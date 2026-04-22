module dat_mem (
    input[7:0]  dat_in,
    input       clk,
                wr_en,
                rd_en,
                reset,
    input[7:0]  address,
    output logic[7:0]   dat_out
);
    logic[7:0] mem_core[256];

    assign dat_out = mem_core[address];

    always_ff @(posedge clk) begin
        if(reset) begin
            for(int i = 0; i < 256; i = i + 1) begin
                mem_core[i] <= 0;
            end
        end
        else if(wr_en) begin
            mem_core[address] <= dat_in;
        end
    end
endmodule