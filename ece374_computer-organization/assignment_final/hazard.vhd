library ieee;
use ieee.std_logic_1164.all;

entity hazard is
port (ID_EX_m_read : in std_logic;
		IF_ID_rs, IF_ID_rt, ID_EX_rt : in std_logic_vector (3 downto 0);
		pc_write, IF_ID_write, ID_EX_bubble: out std_logic);
end hazard;

architecture behavior of hazard is
	signal stall : std_logic;
begin
	stall <= '1' when ((ID_EX_m_read = '1') AND ((ID_EX_rt = IF_ID_rs) OR (ID_EX_rt = IF_ID_rt))) else
			   '0';
	IF_ID_write <= NOT(stall);
	pc_write <= NOT(stall);
	ID_EX_bubble <= stall;
end behavior;
