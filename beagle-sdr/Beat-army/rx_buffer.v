

module rx_buffer (
	input spi_clk,
	output spi_simo,
	input spi_somi,
	input spi_cs0,
	input spi_cs1,

	output reg have_pkt_rdy,
	output reg rx_overrun,

	input reset,
	input reset_regs,

	input wire [3:0] channels,
	input wire [15:0] ch_0,
	input wire [15:0] ch_1,
	input wire [15:0] ch_2,
	input wire [15:0] ch_3,

	input rxclk,
	input rxstrobe,
	input clear_status,

	input [6:0] serial_addr,
	input [31:0] serial_data,
	input serial_strobe,

	output [31:0] debugbus
);


endmodule