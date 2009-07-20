library ieee;
use ieee.std_logic_1164.all;

entity blondie is
	port (
		-- CLOCK AND RESET PINS
--		master_clk: in std_logic;   -- defective on Rev. 1 boards
		adc_clkout1: in std_logic;
		adc_clkout2: in std_logic;
		adc_reset_codec: out std_logic;
		
		-- BEAGLEBOARD EXPANSION HEADER
		-- using a 40-pin IDE cable: connect it to the BeagleBoard backwards,
		-- and shift it toward the HDMI conector by 1 row of pins. This avoids
		-- shorting pins 1 and 2 (+1.8 V and +5 V) on the BeagleBoard to pins
		-- 27 and 28 (GND) on this board. Clear as mud?
		pin3 :  in  std_logic;					-- GND			-- Beagle pin28
		pin4 :  in  std_logic;					-- GND			-- Beagle pin27
		pin5 :  in  std_logic;					-- nRESET		-- Beagle pin26
		pin6 :  in  std_logic;					-- REGEN		-- Beagle pin25
		pin7 :  in  std_logic;									-- Beagle pin24
		pin8 :  in  std_logic;									-- Beagle pin23
		pin9 :  in  std_logic;									-- Beagle pin22
		pin10 : in  std_logic;	-- McSPI3_CLK					-- Beagle pin21
		pin11 : in  std_logic;					-- McSPI4_CLK	-- Beagle pin20
		pin12 : in  std_logic;	-- McSPI3_SIMO					-- Beagle pin19
		pin13 : out std_logic;					-- McSPI4_SOMI	-- Beagle pin18
		pin14 : out std_logic;	-- McSPI3_SOMI					-- Beagle pin17
		pin15 : in  std_logic;					-- McSPI4_CS0	-- Beagle pin16
		pin16 : in  std_logic;									-- Beagle pin15
		pin17 : in  std_logic;									-- Beagle pin14
		pin18 : in  std_logic;	-- McSPI3_CS1					-- Beagle pin13
		pin19 : in  std_logic;					-- McSPI4_SIMO	-- Beagle pin12
		pin20 : in  std_logic;	-- McSPI3_CS0					-- Beagle pin11
		pin21 : in  std_logic;									-- Beagle pin10
		pin22 : in  std_logic;									-- Beagle pin09
		pin23 : in  std_logic;									-- Beagle pin08
		pin24 : in  std_logic;									-- Beagle pin07
		
		-- SPI BUS TO ADC AND DAUGHTERBOARDS
		spi_adc_clk : out std_logic;
		spi_adc_simo : out std_logic;
		spi_adc_somi : in std_logic;
		spi_adc_cs : out std_logic;
		spi_txdb_cs: out std_logic;
		spi_rxdb_cs: out std_logic;
		
		-- PARALLEL DATA TO/FROM ADC/DAC
		tx_a: out std_logic_vector(13 downto 0);
		txsync_a: out std_logic;
		rx_a_a: in std_logic_vector(11 downto 0);
		rx_a_b: in std_logic_vector(11 downto 0);
		rxsync: in std_logic;
		
		-- SPI DATA FROM AUXILIARY ADCS
		spi_aux_clk: out std_logic;
		spi_aux_cs: out std_logic;
		spi_aux_somi: out std_logic;
		
		-- DAUGHTERBOARD I/O CONNECTIONS
		io_rx_a: out std_logic_vector(15 downto 0);
		io_tx_a: out std_logic_vector(15 downto 0);
		rs232_rxd_1: in std_logic;
		rs232_txd_1: out std_logic;
		i2c_db_scl: out std_logic;
		i2c_db_sda: inout std_logic;
		
		-- DEBUG BLINKENLIGHTS
		led : out std_logic_vector(7 downto 0)
	);
end blondie;

architecture structural of blondie is
	subtype rxword is std_logic_vector (11 downto 0);
	subtype txword is std_logic_vector (13 downto 0);
	
	signal clk: std_logic;
	signal mcspi3_clk, mcspi3_simo, mcspi3_somi, mcspi3_cs0, mcspi3_cs1: std_logic;
	signal mcspi4_clk, mcspi4_simo, mcspi4_somi, mcspi4_cs0: std_logic;
	signal ctrl_somi: std_logic;
	
	signal regfile_wren: std_logic;
	signal regfile_addr: std_logic_vector(5 downto 0);
	signal regfile_rd_data: std_logic_vector(7 downto 0);
	signal regfile_wr_data: std_logic_vector(7 downto 0);
	
	-- zz prefix indicates control signals to/from register file
	signal zz_interpolation_rate: std_logic_vector(15 downto 0);
	signal zz_decimation_rate: std_logic_vector(15 downto 0);
	signal zz_dac_underflow: std_logic;
	signal zz_adc_overflow: std_logic;

	signal spislave_txdata: txword;
	signal spislave_rxdata: rxword;
	
	signal tx_fifo_wr_req, tx_fifo_wr_full, tx_fifo_rd_empty: std_logic;
	signal rx_fifo_rd_req, rx_fifo_rd_empty, rx_fifo_wr_full: std_logic;

	signal interp_input_request: std_logic;
	signal interp_input: txword;
	signal interp_output_request: std_logic;
	
	signal decim_output_request: std_logic;
	signal decim_output: rxword;
	signal decim_input_request: std_logic;
	
	signal sync_rx_from_adc: rxword;
	signal adc_fifo_rd_empty: std_logic;
	
	signal sync_tx_to_dac: txword;
	signal dac_fifo_wr_full: std_logic;
	
	component control_spi_slave
		port (
			clk, simo, cs: in std_logic;
			somi, wren: out std_logic;
			address: out std_logic_vector(5 downto 0);
			rd_data: in  std_logic_vector(7 downto 0);
			wr_data: out std_logic_vector(7 downto 0)
		);
	end component;
	
	component register_file
		port (
			master_clk, spi_clk, wren: in std_logic;
			address: in  std_logic_vector(5 downto 0);
			rd_data: out std_logic_vector(7 downto 0);
			wr_data: in  std_logic_vector(7 downto 0);
			
			leds: out std_logic_vector(7 downto 0);
			interpolation_rate: out std_logic_vector(15 downto 0);
			decimation_rate: out std_logic_vector(15 downto 0);
			dac_underflow, adc_overflow, rx_overflow, tx_underflow: in std_logic
		);
	end component;
	
	component data_spi_slave
		port (
			clk, simo, cs: in std_logic;
			somi, rd_req, wr_req: out std_logic;
			tx_full, rx_empty: in std_logic;
			txdata: out txword;
			rxdata: in  rxword
		);
	end component;
		
	component interp
		port (
			clk: in std_logic;
			input: in txword;
			output: out txword;
			input_request: out std_logic;
			input_enable: in std_logic;
			rate: in std_logic_vector(15 downto 0);
			output_request: out std_logic;
			output_enable: in std_logic
		);
	end component;
	
	component decim
		port (
			clk: in std_logic;
			input: in rxword;
			output: out rxword;
			output_request: out std_logic;
			output_enable: in std_logic;
			rate: in std_logic_vector(15 downto 0);
			input_request: out std_logic;
			input_enable: in std_logic
		);
	end component;
	
	component tx_fifo
		PORT (
			data		: IN STD_LOGIC_VECTOR (13 DOWNTO 0);
			rdclk		: IN STD_LOGIC ;
			rdreq		: IN STD_LOGIC ;
			wrclk		: IN STD_LOGIC ;
			wrreq		: IN STD_LOGIC ;
			q			: OUT STD_LOGIC_VECTOR (13 DOWNTO 0);
			rdempty		: OUT STD_LOGIC ;
			wrfull		: OUT STD_LOGIC 
		);
	end component;
	
	component rx_fifo
		PORT (
			data		: IN STD_LOGIC_VECTOR (11 DOWNTO 0);
			rdclk		: IN STD_LOGIC ;
			rdreq		: IN STD_LOGIC ;
			wrclk		: IN STD_LOGIC ;
			wrreq		: IN STD_LOGIC ;
			q			: OUT STD_LOGIC_VECTOR (11 DOWNTO 0);
			rdempty		: OUT STD_LOGIC ;
			wrfull		: OUT STD_LOGIC 
		);
	end component;
	
	component dac_fifo
		PORT (
			data		: IN STD_LOGIC_VECTOR (13 DOWNTO 0);
			rdclk		: IN STD_LOGIC ;
			rdreq		: IN STD_LOGIC ;
			wrclk		: IN STD_LOGIC ;
			wrreq		: IN STD_LOGIC ;
			q			: OUT STD_LOGIC_VECTOR (13 DOWNTO 0);
			wrempty		: OUT STD_LOGIC ;
			wrfull		: OUT STD_LOGIC 
		);
	end component;
	
	component adc_fifo
		PORT (
			data		: IN STD_LOGIC_VECTOR (11 DOWNTO 0);
			rdclk		: IN STD_LOGIC ;
			rdreq		: IN STD_LOGIC ;
			wrclk		: IN STD_LOGIC ;
			wrreq		: IN STD_LOGIC ;
			q			: OUT STD_LOGIC_VECTOR (11 DOWNTO 0);
			rdempty		: OUT STD_LOGIC ;
			rdfull		: OUT STD_LOGIC 
		);
	end component;
	
begin
	clk <= adc_clkout1; -- FIXME add a PLL here to boost clock rate for filters
	
	mcspi3_clk <= pin10; 
	mcspi3_simo <= pin12;
	pin14 <= mcspi3_somi;
	mcspi3_cs0 <= pin20;
	mcspi3_cs1 <= pin18;
	
	mcspi4_clk <= pin11;
	mcspi4_simo <= pin19;
	pin13 <= mcspi4_somi;
	mcspi4_cs0 <= pin15;

	spi_adc_clk <= mcspi3_clk;
	spi_adc_simo <= mcspi3_simo;
	spi_adc_cs <= mcspi3_cs0;
	
	mcspi3_somi <= spi_adc_somi when mcspi3_cs0 = '0' else
	               ctrl_somi when mcspi3_cs1 = '0' else
	               'Z';
	               
	-- Debug outputs
	io_rx_a <= (15 => mcspi3_clk,
	            13 => mcspi3_simo,
	            11 => mcspi3_somi,
	             9 => mcspi3_cs0,
	             7 => mcspi3_cs1,
	            14 => mcspi4_clk,
	            12 => mcspi4_simo,
	            10 => mcspi4_somi,
	             8 => mcspi4_cs0,
	             6 => zz_dac_underflow,
	             5 => dac_fifo_wr_full,
	             4 => adc_fifo_rd_empty,
	             3 => zz_adc_overflow,
	             2 => rx_fifo_rd_empty,
	             1 => tx_fifo_wr_full,
	             0 => clk);

-- CONTROL PATH
	
	control_spi_slave_0: control_spi_slave port map (
		clk => mcspi3_clk,
		simo => mcspi3_simo,
		somi => ctrl_somi,
		cs => mcspi3_cs1,
		wren => regfile_wren,
		address => regfile_addr,
		rd_data => regfile_rd_data,
		wr_data => regfile_wr_data
	);
	
	register_file_0: register_file port map (
		master_clk => clk,
		spi_clk => mcspi3_clk,
		wren => regfile_wren,
		address => regfile_addr,
		rd_data => regfile_rd_data,
		wr_data => regfile_wr_data,
		interpolation_rate => zz_interpolation_rate,
		decimation_rate => zz_decimation_rate,
		dac_underflow => zz_dac_underflow,
		adc_overflow => zz_adc_overflow,
		rx_overflow => rx_fifo_wr_full,
		tx_underflow => tx_fifo_rd_empty,
		leds => led
	);

	data_spi_slave_0: data_spi_slave port map (
		clk => mcspi4_clk,
		simo => mcspi4_simo,
		somi => mcspi4_somi,
		cs => mcspi4_cs0,
		wr_req => tx_fifo_wr_req,
		rd_req => rx_fifo_rd_req,
		txdata => spislave_txdata,
		rxdata => spislave_rxdata,
		tx_full => tx_fifo_wr_full,
		rx_empty => rx_fifo_rd_empty
	);
	
-- TRANSMIT PATH
	
	tx_fifo_0: tx_fifo port map (
		wrclk => mcspi4_clk,
		wrreq => tx_fifo_wr_req,
		data => spislave_txdata,
		wrfull => tx_fifo_wr_full,
		rdclk => clk,
		rdreq => interp_input_request,
		q => interp_input,
		rdempty => tx_fifo_rd_empty
	);

	interp_0: interp port map (
		clk => clk,
		input => interp_input,
		input_request => interp_input_request,
		input_enable => not tx_fifo_rd_empty,
		rate => zz_interpolation_rate,
		output => sync_tx_to_dac,
		output_request => interp_output_request,
		output_enable => not dac_fifo_wr_full
	);

	dac_fifo_0: dac_fifo port map (
		wrclk => clk,
		wrreq => interp_output_request,
		data => sync_tx_to_dac,
		wrfull => dac_fifo_wr_full,
		rdclk => adc_clkout2,
		rdreq => '1',
		q => tx_a,
		wrempty => zz_dac_underflow
	);
		
-- RECEIVE PATH

	adc_fifo_0: adc_fifo port map (
		rdclk => clk,
		rdreq => decim_input_request,
		q => sync_rx_from_adc,
		rdempty => adc_fifo_rd_empty,
		wrclk => adc_clkout1,
		wrreq => '1',
		data => rx_a_a,
		rdfull => zz_adc_overflow
	);
	
	decim_0: decim port map (
		clk => clk,
		input => sync_rx_from_adc,
		input_request => decim_input_request,
		input_enable => not adc_fifo_rd_empty,
		output => decim_output,
		output_request => decim_output_request,
		output_enable => not rx_fifo_wr_full,
		rate => zz_decimation_rate
	);
	
	rx_fifo_0: rx_fifo port map (
		rdclk => mcspi4_clk,
		rdreq => rx_fifo_rd_req,
		q => spislave_rxdata,
		rdempty => rx_fifo_rd_empty,
		wrclk => clk,
		wrreq => decim_output_request,
		data => decim_output,
		wrfull => rx_fifo_wr_full
	);

end structural;