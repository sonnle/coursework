--------- This is one-bit register circuit -----------
library ieee;
use ieee.std_logic_1164.all;

entity regOne is
port (clock : in std_logic;
	  D : in std_logic;
	  Q : out std_logic);
end regOne;

architecture behavior of regOne is
begin
	process (clock)
	begin
		if clock'event and clock = '1' then
				Q <= D;
		end if;
	end process;
end behavior;
