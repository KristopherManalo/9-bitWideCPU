// Program counter
// If I can manage to fit both relative and absolute jumps
// will keep both

`include "globals.svh"
module PC (
    input                       reset,
                                clk,
                                relative_en, 
                                absolute_en,
    input       [INSTRUCTION_MEM_WIDTH-1:0]  target_address,
    output logic[INSTRUCTION_MEM_WIDTH-1:0]  ctr_next
);

always_ff @(posedge clk) begin
    if(reset)
        ctr_next <= '0;
    else if(relative_en)
        ctr_next <= ctr_next + target_address;
    else if(absolute_en)
        ctr_next <= target_address;
    else
        ctr_next <= ctr_next + 'b1;
end
endmodule