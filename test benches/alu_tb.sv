module alu_tb();
    logic clk;
    logic [1:0] alu_cmd, funct;
    logic [7:0] inA, inB;
    wire [7:0] result;
    logic shift_carry_in, shift_carry_out, zero, lt;

    alu alu1(
        .alu_cmd,
        .funct,
        .inA,
        .inB,
        .result,
        .shift_carry_in,
        .shift_carry_out,
        .zero,
        .lt
    );


    initial begin
        shift_carry_in = '0;

        #5ns clk = '1;
        alu_cmd = 'b00;
        funct = 'b00;
        inA = 'b01101001;
        inB = 'b00001111;
        
        #5ns clk = '0;
        $display("AND Test: Expected = %d, recieved = %d", 
        8'b00001001, int'(result));

        #5ns clk = '1;

        alu_cmd = 'b00;
        funct = 'b01;
        inA = 'b00000101; // 5
        inB = 'b11111110; // -2
        
        #5ns clk = '0;
        $display("ADD Test: Expected = %d, recieved = %d", 
        8'b00000011, int'(result));

        #5ns clk = '1;

        alu_cmd = 'b00;
        funct = 'b10;
        inA = 'b11011101;
        inB = 'b00000001;

        #5ns clk = '0;
        $display("LSL Test: Expected = %d, recieved = %d", 
        8'b10111011, int'(result));


        #5ns clk = '1;

        alu_cmd = 'b00;
        funct = 'b11;
        inA = 'b11011101;
        inB = 'b00000001;

        #5ns clk = '0;
        $display("LSR Test: Expected = %d, recieved = %d", 
        8'b11101110, int'(result));


        #5ns clk = '1;

        alu_cmd = 'b11;
        funct = 'b10;
        inA = 'b11111111;
        inB = 'b00000001;

        #5ns clk = '0;
        $display("ADI Test: Expected = %d, recieved = %d", 
        8'b00000000, int'(result));


        #5ns clk = '1;

        alu_cmd = 'b00;
        funct = 'b10;
        inA = 'b10101010;
        inB = 'b10101011;

        #5ns clk = '0;
        $display("BEQ Test1: Expected = %d, recieved = %d",
        0, zero);


        #5ns clk = '1;

        alu_cmd = 'b00;
        funct = 'b10;
        inA = 'b10101011;
        inB = 'b10101011;

        #5ns clk = '0;
        $display("BEQ Test2: Expected = %d, recieved = %d",
        1, zero);

        #5ns clk = '1;

        alu_cmd = 'b00;
        funct = 'b10;
        inA = 'b00000010;
        inB = 'b00000001;

        #5ns clk = '0;
        $display("BLT Test1: Expected = %d, recieved = %d",
        0, lt);


        #5ns clk = '1;

        alu_cmd = 'b00;
        funct = 'b10;
        inB = 'b00000010;
        inA = 'b00000001;

        #5ns clk = '0;
        $display("BLT Test2: Expected = %d, recieved = %d",
        1, lt);
    end
endmodule