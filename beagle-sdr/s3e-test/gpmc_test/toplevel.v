`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    14:49:41 09/12/2008 
// Design Name: 
// Module Name:    toplevel 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module toplevel(

input clk,    // 50 MHz clock

// Connections to beagle via FX2 connector
inout [15:0] AD,   // IO1-IO16
 
input nWE,   // IO18
input nOE,   // IO19
input nADV,  // IO20
input CS4,   // IO21
input G_CLK, // IO22

output tx_data_ok, // IO23
output rx_data_ok, // IO24

output lcd_rs,
output lcd_rw,
output lcd_e,
output [3:0] lcd_d
);

reg [15:0] data_out;
reg [15:0] count;

assign AD = nOE ? 16'bz : count;

always @(negedge nOE)
begin
	if (count == 16'd65535) begin
		count <= 16'b0;
		data_out <= count;
	end else begin
		count <= count + 16'b1;
		data_out <= count;
	end
end

endmodule
