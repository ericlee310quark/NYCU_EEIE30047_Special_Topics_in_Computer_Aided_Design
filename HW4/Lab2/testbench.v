//Verilog HDL for "PLL", "testbench" "functional"

`timescale 1ns/10ps
module testbench (rst );
output reg rst;
initial begin
	rst = 1'b0;
	#1 rst = 1'b1;
end
endmodule

