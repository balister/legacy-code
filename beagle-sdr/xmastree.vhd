-- Copyright (C) 1991-2009 Altera Corporation
-- Your use of Altera Corporation's design tools, logic functions 
-- and other software and tools, and its AMPP partner logic 
-- functions, and any output files from any of the foregoing 
-- (including device programming or simulation files), and any 
-- associated documentation or information are expressly subject 
-- to the terms and conditions of the Altera Program License 
-- Subscription Agreement, Altera MegaCore Function License 
-- Agreement, or other applicable license agreement, including, 
-- without limitation, that your use is for the sole purpose of 
-- programming logic devices manufactured by Altera and sold by 
-- Altera or its authorized distributors.  Please refer to the 
-- applicable agreement for further details.

library ieee;
use ieee.std_logic_1164.all;
library altera;
use altera.altera_syn_attributes.all;

entity xmastree is
	port
	(
		led : out std_logic_vector(7 downto 0);
		master_clk : in std_logic;
		io_rx_a : out std_logic_vector(0 to 15);
		io_tx_a : out std_logic_vector(0 to 15);
		tx_a : in std_logic_vector(0 to 13);
		rx_a_a : in std_logic_vector(0 to 11);
		rx_a_b : in std_logic_vector(0 to 11);
		TXSYNC_A :in std_logic;
		pin3 : out std_logic;
		pin4 : out std_logic;
		pin5 : out std_logic;
		pin6 : out std_logic;
		pin7 : out std_logic;
		pin8 : out std_logic;
		pin9 : out std_logic;
		pin10 : out std_logic;
		pin11 : out std_logic;
		pin12 : out std_logic;
		pin13 : out std_logic;
		pin14 : out std_logic;
		pin15 : out std_logic;
		pin16 : out std_logic;
		pin17 : out std_logic;
		pin18 : out std_logic;
		pin19 : out std_logic;
		pin20 : out std_logic;
		pin21 : out std_logic;
		pin22 : out std_logic;
		pin23 : out std_logic;
		pin24 : out std_logic;
		adc_clkout1 : in std_logic;
		adc_clkout2 : in std_logic;
		rs232_txd_1 : out std_logic;
		rs232_rxd_1 : out std_logic;
		spi_adc_clk : in std_logic;
		spi_adc_simo : in std_logic;
		spi_aux_clk : in std_logic;
		spi_aux_cs : in std_logic;
		spi_aux_somi : in std_logic;
		spi_adc_somi : in std_logic;
		spi_adc_cs : in std_logic;
		spi_txdb_cs : out std_logic;
		spi_rxdb_cs : out std_logic;
		spi_db_clk : out std_logic;
		spi_db_data : out std_logic;
		adc_reset_codec : in std_logic;
		rxsync : in std_logic
	);

end xmastree;

architecture ppl_type of xmastree is
	signal x: std_logic;
begin
	x <= '1';

		led <= (others => x);
		
		io_rx_a <= (others => x);
		io_tx_a <= (others => x);
		pin3 <= x;
		pin4 <= x;
		pin5 <= x;
		pin6 <= x;
		pin7 <= x;
		pin8 <= x;
		pin9 <= x;
		pin10 <= x;
		pin11 <= x;
		pin12<= x;
		pin13<= x;
		pin14 <= x;
		pin15 <= x;
		pin16 <= x;
		pin17 <= x;
		pin18 <= x;
		pin19 <= x;
		pin20 <= x;
		pin21 <= x;
		pin22 <= x;
		pin23 <= x;
		pin24 <= x;
		rs232_txd_1 <= x;
		rs232_rxd_1 <= x;
		spi_txdb_cs <= x;
		spi_rxdb_cs <= x;
		spi_db_clk<= x;
		spi_db_data <= x;

end;
