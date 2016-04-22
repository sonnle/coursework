library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_signed.all;

package components is
------------ full adder ---------------
component fulladd IS
	PORT (Cin, x, y : IN STD_LOGIC;
			s, Cout : OUT STD_LOGIC );
END component;

------------- n stage ripple carry adder -------------------
component ripple_carry IS
	PORT (Cin: IN STD_LOGIC;
			x,y : IN STD_LOGIC_VECTOR(3 downto 0);
			s : OUT STD_LOGIC_VECTOR(3 downto 0);
			Cout : OUT STD_LOGIC );
END component;

--------- multiplexer -----------------------
component mux2to1 IS
	generic(n : integer := 4);
	PORT (s : IN std_logic;
			a, b : in std_logic_vector((n-1) downto 0);
			f : OUT std_logic_vector((n-1) downto 0));
END component;

component mux4to1 IS
	generic  (n : integer:= 4);
	PORT (w0, w1, w2, w3 : IN STD_LOGIC_VECTOR((n-1) downto 0);
			s : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
			f : OUT STD_LOGIC_VECTOR((n-1) downto 0));
END component;

component mux2to1single IS
	PORT ( s : IN std_logic;
			a, b : in std_logic;
			f : OUT std_logic);
END component;

---------------- decoders -------------------------
component dec2to4 IS
	PORT (w : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
			En : IN STD_LOGIC;
			y : OUT STD_LOGIC_VECTOR(3 downto 0));
END component;

component dec4to16 IS
	PORT (w : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
			En : IN STD_LOGIC;
			y : OUT STD_LOGIC_VECTOR(15 downto 0));
END component;

------------- n stage tri-state buffer ------------------------
component trin IS
	GENERIC (N : INTEGER := 8);
	PORT (X : IN STD_LOGIC_VECTOR((N-1) DOWNTO 0);
			E : IN STD_LOGIC;
			F : OUT STD_LOGIC_VECTOR((N-1) DOWNTO 0));
END component;

------------ n bit register -----------------------
component regN is
	generic(N: integer:= 32);
	port (clock : in std_logic;
			D : in std_logic_vector(N-1 downto 0);
			Q : out std_logic_vector(N-1 downto 0));
end component;

--------- one-bit register -----------
component regOne is
port (clock : in std_logic;
	  D : in std_logic;
	  Q : out std_logic);
end component;

---------- n bit Enable register
component regNE is
	generic(N: integer:= 32);
	port (clock : in std_logic;
			enable : in std_logic;
			D : in std_logic_vector(N-1 downto 0);
			Q : out std_logic_vector(N-1 downto 0));
end component;

-------------- instruction memeory --------------------------
component instruction_memory is
	port (clock, reset : in std_logic;
			input : in std_logic_vector(3 downto 0);
			output : out std_logic_vector(31 downto 0));
end component;

--------------- register file -------------------------------
component register_file is
	port (clock, reset, reg_write : in std_logic;
			read_port1, read_port2, write_port, write_value : in std_logic_vector(3 downto 0);
			value1, value2 : out std_logic_vector(3 downto 0));
end component;

------------- instrcution decode ----------------------------
component instruction_decode is
	port (instr : in std_logic_vector(31 downto 0);
			m_read, m_write, reg_write, add_sub, alusrc, branch, jump, memtoreg, regdst : out std_logic;
			aluop : out std_logic_vector (1 downto 0);
			read_p1, read_p2, write_p : out std_logic_vector(3 downto 0));
end component;

------------ arithmetic logic unit --------------------------
component alu is
	port (add_sub : in std_logic;
			aluop : in std_logic_vector(1 downto 0);
			x, y : in std_logic_vector(3 downto 0);
			s : out std_logic_vector(3 downto 0);
			a_zero : out std_logic);
end component;

----------- data memory -------------------------
component data_memory is
	port (clock, reset, m_write : in std_logic;
			read_port1, write_port, write_value : in std_logic_vector(3 downto 0);
			value1 : out std_logic_vector(3 downto 0));
end component;

---------- forwarding ------------------
component forwarding is
	port (rs, EX_MEM_rd, MEM_WB_rd ,rt : in std_logic_vector(3 downto 0);
			EX_MEM_reg_write, MEM_WB_reg_write : in std_logic;
			forwardA, forwardB : out std_logic_vector(1 downto 0));
end component;

----------  hazard -------------------
component hazard is
port (ID_EX_m_read : in std_logic;
		IF_ID_rs, IF_ID_rt, ID_EX_rt : in std_logic_vector (3 downto 0);
		pc_write, IF_ID_write, ID_EX_bubble: out std_logic);
end component;
	
end components;