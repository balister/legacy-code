module rx_buffer (
	input spi_clk,
	output spi_output,
	input spi_input,
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

	input rx_clk,
	input rxstrobe,
	input clear_status,

	input [6:0] serial_addr,
	input [31:0] serial_data,
	input serial_strobe,

	output [31:0] debug_bus
);

wire [31:0] fifo_data;
wire [31:0] spi_data;
reg read_fifo;
reg write_fifo;
wire [12:0] rd_fifo_level;
wire [12:0] wr_fifo_level;
wire wr_fifo_full;

// tmp rx_clk divider

reg [12:0] clk_div;
wire rx_clk_div;

always @(posedge rx_clk)
begin
	clk_div <= clk_div + 1;
end

assign rx_clk_div = clk_div[8];

// Instantiate FIFO

rx_fifo	rx_fifo_inst (
	.data (fifo_data),
	.rdclk (!spi_clk),
	.rdreq (read_fifo),
	.wrclk (rx_clk_div),
	.wrreq (fill_fifo),
	.q (spi_data),
	.rdempty (rx_underrun),
	.rdfull ( ),
	.rdusedw (rd_fifo_level ),
	.wrempty (),
	.wrfull (wr_fifo_full),
	.wrusedw (wr_fifo_level)
	);


// Read from FIFO and feed to SPI

reg [31:0] q_to_spi;
reg [5:0] bit_cnt;
wire rx_underrun;

assign spi_output = q_to_spi[31];

always @(negedge spi_clk)
begin
	if (bit_cnt == 6'd31) begin
		bit_cnt <= 6'b0;
		q_to_spi <= spi_data;
	end else begin
		bit_cnt <= bit_cnt + 6'b1; 
		q_to_spi <= q_to_spi << 1;
	end

end

always @(posedge spi_clk)
begin
	if (bit_cnt == 6'd31)
		read_fifo <= 1'b1;
	else
		read_fifo <= 1'b0;
end

assign debug_bus[0] = spi_clk;
assign debug_bus[1] = spi_output;
assign debug_bus[7:2] = bit_cnt;
assign debug_bus[8] = read_fifo;
assign debug_bus[12] = rx_underrun;
assign debug_bus[13] = spi_input;
assign debug_bus[14] = have_pkt_rdy;

// Write data into FIFO

reg fill_fifo;
reg [15:0]cha_data;
reg [15:0]chb_data;

always @(negedge rx_clk_div)
begin
	cha_data <= cha_data + 16'd1;
	chb_data <= chb_data + 16'd1;
//	cha_data <= 16'h1234;
//	chb_data <= 16'h5678;
	fill_fifo <= 1;

	if (wr_fifo_level > 12'd1024)
		have_pkt_rdy <= 1;
	else
		have_pkt_rdy <= 0;
end

assign fifo_data = {chb_data, cha_data};

assign debug_bus[9] = fill_fifo;
assign debug_bus[10] = wr_fifo_full;
assign debug_bus[11] = rx_clk_div;

endmodule