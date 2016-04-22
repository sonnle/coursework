
-------- This is a n-bit 2_to_1 MUX one bit input and output ------------------
LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY mux2to1single IS
	PORT ( s : IN std_logic;
			a, b : in std_logic;
			f : OUT std_logic);
END mux2to1single;

ARCHITECTURE LogicFunc OF mux2to1single IS
	signal s_vec : std_logic;
BEGIN
	process(s)
		begin
			if (s='0') then
				s_vec <= '0';
			else
				s_vec <= '1';
			end if;
	end process;
		
	f <= ((not s_vec) and a) OR (b and s_vec);	
END LogicFunc;
