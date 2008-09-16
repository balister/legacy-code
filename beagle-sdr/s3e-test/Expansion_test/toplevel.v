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


// Connections to beagle via FX2 connector
input pin3,   // IO39
input pin4,   // IO40
input pin5,   // IO37
input pin6,   // IO38
input pin7,   // IO35
input pin8,   // IO36
input pin9,   // IO33
input pin10,  // IO34
input pin11,  // IO31
input pin12,  // IO32
input pin13,  // IO29
input pin14,  // IO30
input pin15,  // IO27
input pin16,  // IO28
input pin17,  // IO25
input pin18,  // IO26
input pin19,  // IO23
input pin20,  // IO24
input pin21,  // IO21
input pin22,  // IO22
input pin23,  // IO11
input pin24,  // IO12

input nREGEN, // IO09
input nRESET, // IO10

output [7:0] led

    );

assign led[0] = pin3;

assign led[1] = 0;
assign led[2] = 1;
assign led[3] = 0;
assign led[4] = 0;
assign led[5] = 0;
assign led[6] = 0;
assign led[7] = 0;
endmodule
