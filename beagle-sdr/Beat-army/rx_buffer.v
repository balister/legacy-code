

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
wire [11:0] rd_fifo_level;
wire [11:0] wr_fifo_level;
wire wr_fifo_full;

// Instantiate FIFO

rx_fifo	rx_fifo_inst (
	.data (fifo_data),
	.rdclk (spi_clk),
	.rdreq (!read_fifo),
	.wrclk (rx_clk),
	.wrreq (fill_fifo),
	.q (spi_data),
	.rdempty ( ),
	.rdfull ( ),
	.rdusedw (rd_fifo_level ),
	.wrempty (),
	.wrfull (wr_fifo_full),
	.wrusedw (wr_fifo_level)
	);


// Read from FIFO and feed to SPI

reg [31:0] q_to_spi;
reg [5:0] bit_cnt;

assign spi_output = q_to_spi[0];

always @(negedge spi_clk)
begin
	if (bit_cnt == 6'd31) begin
		bit_cnt <= 6'b0;
		q_to_spi <= spi_data;
	end else begin
		bit_cnt <= bit_cnt + 6'b1; 
		q_to_spi <= {1'b0, q_to_spi[31:1]};
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

// Write data into FIFO

reg fill_fifo;
reg [31:0]rx_data;

always @(negedge rx_clk)
begin
	rx_data <= 32'hDEADBEEF;

	if (wr_fifo_full)
		fill_fifo <= 0;
	else if (wr_fifo_level < 1024)
		fill_fifo <= 1;
	else
		fill_fifo <= fill_fifo;

end

assign debug_bus[9] = fill_fifo;
assign debug_bus[10] = wr_fifo_full;
assign debug_bus[11] = rx_clk;

endmodule