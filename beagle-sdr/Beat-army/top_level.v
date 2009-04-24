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

wire host_spi_clk;
wire host_spi_simo;
wire host_spi_somi;
wire host_spi_cs0;
wire host_spi_cs1;

assign host_spi_clk = pin21;
assign host_spi_simo = pin19;
assign host_spi_somi = pin17;
assign host_spi_cs0 = pin11;
assign host_spi_cs1 = pin13;



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

//`define FOO
`ifdef FOO
rx_buffer rx_buffer
	  ( .spi_clk(host_spi_clk), .spi_simo(host_spi_clk), .spi_somi(host_spi_somi),
		.spi_cs0(host_spi_cs0), .spi_cs1(host_spi_cs1),
		.bus_reset(rx_bus_reset),.reset(rx_dsp_reset), .reset_regs(rx_dsp_reset),
		.RD(RD),.have_pkt_rdy(have_pkt_rdy),.rx_overrun(rx_overrun),
		.channels(rx_numchan),
		.ch_0(ch0rx),.ch_1(ch1rx),
		.ch_2(ch2rx),.ch_3(ch3rx),
		.rxclk(clk64),.rxstrobe(hb_strobe),
		.clear_status(clear_status),
		.serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
		.debugbus(rx_debugbus) );
`endif

endmodule
