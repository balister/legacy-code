module top_level (

input master_clk,

// Expansion connector from Beagle
input pin3,
input pin4,
input pin5,
input pin6,
input pin7,
input pin8, // MCSPI3_CS0
input pin9,
input pin10, // MCSPI3_CS1
input pin11,
input pin12,
input pin13,
input pin14, // MCSPI3_SOMI
input pin15,
input pin16, // MCSPI3_SIMO
input pin17,
input pin18, // MCSPI3_CLK
input pin19,
input pin20,
input pin21,
input pin22,
input pin23,
input pin24,

output [7:0] led,

input SCLK,
input SDI,
input SDO,

input wire [11:0] rx_a_a,
input wire [11:0] rx_a_b,

output wire [13:0] tx_a,

output wire TXSYNC_A,

inout wire [15:0] io_tx_a,
inout wire [15:0] io_rx_a
);

assign io_rx_a[0] = pin8;
assign io_rx_a[1] = pin10;
assign io_rx_a[2] = pin16;
assign io_rx_a[3] = pin18;

endmodule
