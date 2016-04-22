------ This file describes the instruction decode operation -------------
LIBRARY ieee;
use ieee.std_logic_1164.all;
USE work.components.all;

entity instruction_decode is
	port(instr : in std_logic_vector(31 downto 0);
			m_read, m_write, reg_write, add_sub, alusrc, branch, jump, memtoreg, regdst : out std_logic;
			aluop : out std_logic_vector (1 downto 0);
			read_p1, read_p2, write_p : out std_logic_vector(3 downto 0));
end instruction_decode;

architecture struc_behaviour of instruction_decode is
	signal opcode, funct : std_logic_vector(5 downto 0);
	signal shamt : std_logic_vector(4 downto 0);

begin

	opcode <= instr(31 downto 26);
	shamt <= instr(10 downto 6);
	funct <= instr(5 downto 0);

	read_p1 <= instr(24 downto 21);
	read_p2 <= instr(19 downto 16);
	write_p <= instr(14 downto 11);
	
	reg_write <= '1' when ((opcode = "000000") OR (opcode = "100011"))
							else '0';
	add_sub <= '1' when (((opcode = "000000") AND ((funct="100010") OR (funct = "101010"))) OR (opcode = "000100"))
						else '0';
	branch <= '1' when ((opcode = "000100") OR (opcode = "000101"))
						else '0';
	jump <= '1' when (opcode = "000010")
						else '0';
	alusrc <= '0' when ((opcode = "000000") OR (opcode = "000100") OR (opcode = "000101"))
						else '1';
	m_read <= '1' when (opcode = "100011")
						else '0';
	m_write <= '1' when (opcode = "101011")
						else '0';
	memtoreg <= '1' when (opcode = "100011")
						else '0';
	regdst <= '1' when (opcode = "000000")
						else '0';
	aluop <= "00" when ((opcode = "000010") OR ((opcode = "000000") AND (funct = "101010"))) else
				"01" when (((opcode = "000000") AND ((funct = "100000") OR (funct = "100010"))) OR (opcode = "100011") OR (opcode = "101011") OR (opcode = "000100")) else
				"10" when ((opcode = "000000") AND (funct = "100100")) else
				"11" when ((opcode = "000000") AND (funct = "100101"));
	
	
end struc_behaviour;
