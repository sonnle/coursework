library ieee;
use ieee.std_logic_1164.all;
use work.components.all;

entity alu is
	port(add_sub : in std_logic;
			aluop : in std_logic_vector(1 downto 0);
			x, y : in std_logic_vector(3 downto 0);
			s : out std_logic_vector(3 downto 0);
			a_zero : out std_logic);
end alu;

architecture behavior of alu is
	signal adder_out, zero_one_out, x_and_y, x_or_y : std_logic_vector(3 downto 0);
	signal msb_adder_out : std_logic;
begin
	x_and_y <= x AND y;
	x_or_y <= x OR y;
	msb_adder_out <= adder_out(3);
	a_zero <= '1' when  (adder_out = "0000") else '0';
	a1 : ripple_carry port map(add_sub, x, y, adder_out);
	m1 : mux2to1 generic map (n=>4) port map(msb_adder_out, "0000","0001", zero_one_out);
	m2 : mux4to1 generic map (n=>4) port map (zero_one_out, adder_out, x_and_y, x_or_y, aluop, s);
end behavior;
