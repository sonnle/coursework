library ieee;
use ieee.std_logic_1164.all;

entity forwarding is
port (rs, EX_MEM_rd, MEM_WB_rd ,rt : in std_logic_vector(3 downto 0);
		EX_MEM_reg_write, MEM_WB_reg_write : in std_logic;
		forwardA, forwardB : out std_logic_vector(1 downto 0));
end forwarding;

architecture behaviour of forwarding is
begin
	forwardA <= "10" when ((EX_MEM_rd = rs) AND (EX_MEM_reg_write = '1') AND (EX_MEM_rd /= "0000")) else
					"01" when ((MEM_WB_rd = rs) AND (MEM_WB_reg_write = '1') AND (MEM_WB_rd /= "0000")) else
					"00";
	forwardB <= "10" when ((EX_MEM_rd = rt) AND (EX_MEM_reg_write = '1') AND (EX_MEM_rd /= "0000")) else
					"01" when ((MEM_WB_rd = rt) AND (MEM_WB_reg_write = '1') AND (MEM_WB_rd /= "0000")) else
					"00";
end behaviour;
