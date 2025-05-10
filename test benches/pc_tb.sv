module pc_tb();
logic clk;
logic reset;
logic relative_en, absolute_en;
logic[9:0] target_address;
wire [9:0] ctr_next;

PC pc1(
    .reset,
    .clk,
    .relative_en,
    .absolute_en,
    .target_address,
    .ctr_next
);

initial begin
    reset = 1;
    #5ns clk = '1;
    #5ns clk = '0;
    $display("Expected: %d, recieved %d", 0, ctr_next);

    reset = 0;
    #5ns clk = '1;
    #5ns clk = '0;

    #5ns clk = '1;
    #5ns clk = '0;

    #5ns clk = '1;
    #5ns clk = '0;
    $display("Expected: %d, recieved %d", 3, ctr_next);

    relative_en = 1;
    target_address = 15;
    #5ns clk = '1;
    #5ns clk = '0;
    $display("Expected: %d, recieved %d", 18, ctr_next);

    relative_en = 0;
    absolute_en = 1;
    target_address = 10'b1111111110;
    #5ns clk = '1;
    #5ns clk = '0 ;
    $display("Expected: %d, recieved %d", 1022, ctr_next);

    absolute_en = 0;
    #5ns clk = '1;
    #5ns clk = '0 ;

    #5ns clk = '1;
    #5ns clk = '0 ;
    $display("Expected: %d, recieved %d", 0, ctr_next);
end

endmodule