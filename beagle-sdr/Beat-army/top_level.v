module top_level (

input master_clk,

// Expansion connector from Beagle
input pin3,
input pin4,
input pin5,
input pin6,
input pin7,
input pin8,
input pin9,
input pin10,
input pin11, // MCSPI3_CS0
input pin12,
input pin13, // MCSPI3_CS1
input pin14,
input pin15,
input pin16,
output pin17, // MCSPI3_SOMI
input pin18,
input pin19, // MCSPI3_SIMO
input pin20,
input pin21, // MCSPI3_CLK
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

assign io_rx_a[0] = pin3;
assign io_rx_a[1] = pin4;
assign io_rx_a[2] = pin23;
assign io_rx_a[3] = pin24;
assign io_rx_a[4] = pin14;
assign io_rx_a[5] = pin15;
assign io_rx_a[6] = pin16;
assign io_rx_a[7] = pin17;
assign io_rx_a[8] = pin18;
assign io_rx_a[9] = pin19;
assign io_rx_a[10] = pin20;
assign io_rx_a[11] = pin21;
assign io_rx_a[12] = pin22;
assign io_rx_a[13] = pin23;
assign io_rx_a[14] = pin24;

assign led[0] = 1;

reg rx_data;
always @(negedge pin21)
begin
    rx_data = ! rx_data;
end

assign pin17 = rx_data;

endmodule
