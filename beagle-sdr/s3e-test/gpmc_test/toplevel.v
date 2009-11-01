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
inout AD0,   // IO1
inout AD1,   // IO2
inout AD2,   // IO3
inout AD3,   // IO4
inout AD4,   // IO5
inout AD5,   // IO6
inout AD6,   // IO7
inout AD7,   // IO8
inout AD8,   // IO9
inout AD9,   // IO10
inout AD10,  // IO11
inout AD11,  // IO12
inout AD12,  // IO13
inout AD13,  // IO14
inout AD14,  // IO15
inout AD15,  // IO16
 
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

endmodule
