library ieee;
use ieee.std_logic_1164.all;

entity eastwood is
	port (
		TXSYNC_A : out std_logic;
		rxsync: in std_logic;
		pin3 : in std_logic;
		pin4 : in std_logic;
		pin5 : in std_logic;
		pin6 : in std_logic;
		pin7 : in std_logic;
		pin8 : in std_logic;
		pin9 : in std_logic;
		pin10 : in std_logic;
		pin11 : in std_logic;  -- mcspi3_cs0
		pin12 : in std_logic;                 -- mcspi4_simo
		pin13 : in std_logic;  -- mcspi3_cs1
		pin14 : out std_logic;
		pin15 : in std_logic;
		pin16 : in std_logic;                 -- mcspi4_cs0
		pin17 : out std_logic; -- mcspi3_somi
		pin18 : out std_logic;                -- mcspi4_somi
		pin19 : in std_logic;  -- mcspi3_simo
		pin20 : in std_logic;                 -- mcspi4_clk
		pin21 : in std_logic;  -- mcspi3_clk
		pin22 : in std_logic;
		pin23 : in std_logic;
		pin24 : in std_logic;
		spi_adc_clk : out std_logic;
		spi_adc_simo : out std_logic;
		spi_adc_somi : in std_logic;
		spi_adc_cs : out std_logic;
		tx_a : out std_logic_vector(13 downto 0);
		rx_a_a : in std_logic_vector(11 downto 0);
		led : out std_logic_vector(7 downto 0)
	);
end eastwood;

architecture passthrough of eastwood is
	signal mcspi3_clk, mcspi3_simo, mcspi3_somi, mcspi3_cs0, mcspi3_cs1: std_logic;
begin
	mcspi3_clk <= pin10;   -- these pins will be wrong for you, 
	mcspi3_simo <= pin12;  -- as I only had a 40-pin cable
	pin14 <= mcspi3_somi;  -- and had to do some contortions
--	mcspi3_cs0 <= pinNN;   -- to make it fit on both boards
--	mcspi3_cs1 <= pinNN;

	spi_adc_clk <= mcspi3_clk;
	spi_adc_simo <= mcspi3_simo;
	mcspi3_somi <= spi_adc_somi;
	spi_adc_cs <= '0'; -- mcspi3_cs0;
	
	
		
	led(7 downto 0) <= rx_a_a(11 downto 4);
	tx_a(13 downto 2) <= rx_a_a(11 downto 0);
	tx_a(1 downto 0) <= (others => '0');
end passthrough;