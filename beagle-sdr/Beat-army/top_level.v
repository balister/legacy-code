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
output pin15, // GPIO_133
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

wire host_spi_clk;
wire host_spi_input;
wire host_spi_output;
wire host_spi_cs0;
wire host_spi_cs1;
wire data_ready;

assign host_spi_clk = pin21;
assign host_spi_input = pin19;
assign pin17 = host_spi_output;
assign host_spi_cs0 = pin11;
assign host_spi_cs1 = pin13;
assign pin15 = data_ready;

wire [15:0]rx_debug_bus;

wire clk64;

assign io_rx_a = rx_debug_bus;

assign clk64 = master_clk;

assign led[0] = 1;


`define FOO
`ifdef FOO
rx_buffer rx_buffer
	  ( .spi_clk(host_spi_clk), .spi_input(host_spi_input), .spi_output(host_spi_output),
		.spi_cs0(host_spi_cs0), .spi_cs1(host_spi_cs1),
		.reset(), .reset_regs(),
		.have_pkt_rdy(data_ready),.rx_overrun(),
		.channels(),
		.ch_0(),.ch_1(),
		.ch_2(),.ch_3(),
		.rx_clk(clk64),.rxstrobe(),
		.clear_status(),
		.serial_addr(),.serial_data(),.serial_strobe(),
		.debug_bus(rx_debug_bus) );
`endif

endmodule
