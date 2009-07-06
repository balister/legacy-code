library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity harry is
	port (
		adc_clkout1: in std_logic;
		led : out std_logic_vector(7 downto 0)
	);
end harry;

architecture passthrough of harry is
	signal divide: integer range 0 to 12500000;
	signal sig, clk: std_logic;
begin
	clk <= adc_clkout1;
	
	process (clk)
	begin
		if (rising_edge(clk)) then
			divide <= divide + 1;
			if (divide = 0) then sig <= not sig; end if;
		end if;
	end process;
	
	led <= (others => sig);
end passthrough;