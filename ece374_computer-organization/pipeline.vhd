library ieee;
use ieee.std_logic_1164.all;
use work.components.all;

entity pipeline is
	port( clock, reset : in std_logic;
	result : out std_logic_vector(3 downto 0));
end pipeline;

architecture behaviour of pipeline is
	--- PC Signals ---
	constant initial_pc : std_logic_vector(3 downto 0) := (others => '0');
	signal update_pc, mout : std_logic_vector(3 downto 0);
	signal pc_write : std_logic;
	
	--- IM Signals ---
	signal instr_from_im : std_logic_vector(31 downto 0);
	signal jump_target : std_logic_vector(3 downto 0);
	
	--- ID Signals ---
	signal m_read, m_write, reg_write, add_sub, alusrc, branch, jump, memtoreg, regdst : std_logic; 
	signal aluop : std_logic_vector (1 downto 0);
	
	--- RF Signals ---
	signal read_port1, read_port2, write_port, value1, value2, rout, rf_mux_out: std_logic_vector(3 downto 0); 
	
	--- Fowarding ---
	signal forwardA, forwardB : std_logic_vector(1 downto 0);
	signal forwardA_out, forwardB_out : std_logic_vector(3 downto 0);
	
	--- ALU Signals ---
	signal offset, alu_mux_out, alu_result : std_logic_vector(3 downto 0);
	signal alu_zero : std_logic;
	
	--- DM Signals ---
	signal branch_zero_result: std_logic;
	signal branch_target, jump_decide, data, dm_mux_out, pc_new: std_logic_vector(3 downto 0);
	
	--- Pipeline Signals ---
		---------------------
		----IF Signals -----
			signal IF_jump : std_logic;
		
		--- IF_ID Signals ---
			signal IF_ID_PC : std_logic_vector(3 downto 0);
			signal IF_ID_INST : std_logic_vector(31 downto 0);
			signal IF_ID_write : std_logic;

		--- ID_EX Signals ---
			------------------
			---      RF Signals     ---
			signal ID_EX_value1, ID_EX_value2, ID_EX_read_port1, ID_EX_read_port2 : std_logic_vector(3 downto 0);
			---   Control Signals   ---
			signal ID_EX_reg_write, ID_EX_memtoreg, ID_EX_m_write, ID_EX_m_read, ID_EX_branch, ID_EX_alusrc, ID_EX_add_sub, ID_EX_bubble : std_logic;
			signal ID_EX_aluop : std_logic_vector(1 downto 0);
			--- INST and PC Signals ---
			signal ID_EX_offset, ID_EX_PC, ID_EX_Rd : std_logic_vector(3 downto 0);
		--- EX_MEM Signals ---
			-------------------
			--- Non-Control ---
			signal EX_MEM_result, EX_MEM_value2, EX_MEM_PC, EX_MEM_offset, EX_MEM_Rd : std_logic_vector(3 downto 0);
			signal EX_MEM_alu_zero : std_logic;
			---   Control   ---
			signal EX_MEM_reg_write, EX_MEM_memtoreg, EX_MEM_m_write, EX_MEM_branch : std_logic;
		--- MEM_WB Signals ---
			signal MEM_WB_data, MEM_WB_result, MEM_WB_Rd : std_logic_vector(3 downto 0);
			signal MEM_WB_memtoreg, MEM_WB_reg_write : std_logic;
		--- Hazard-------
			signal hazard_memtoreg, hazard_m_write, hazard_branch, hazard_alusrc, hazard_add_sub, hazard_m_read, hazard_reg_write: std_logic;
			signal hazard_aluop : std_logic_vector(1 downto 0);
			
			signal notclock : std_logic;
		
begin
	---------- Pre-Pipeline ------------
		m1 : mux2to1 generic map (n=>4) port map (reset, pc_new, initial_pc, mout);
		r1	: regNE generic map (n=>4) port map (clock, pc_write, mout, rout);
		addpc : ripple_carry port map ('0', rout, "0001", update_pc);
		
	----------- IM -------------------------------------------------
		im : instruction_memory port map (clock, reset, rout, instr_from_im);
	   IF_jump <= '1' when (instr_from_im(31 downto 26) = "000010") else '0';
		jump_target <= instr_from_im(3 downto 0);
		
	---------- IF_ID Stage -------------
		PC_IF_ID : regNE generic map (n=>4) port map(clock, IF_ID_write,update_pc, IF_ID_PC);
		INST_IF_ID : regNE generic map (n=>32) port map(clock, IF_ID_write, instr_from_im, IF_ID_INST);

	---------- hazard ----------------
		h1 : hazard port map (ID_EX_m_read, read_port1, read_port2, ID_EX_read_port2, pc_write, IF_ID_write, ID_EX_bubble);
		
	------------- ID ------------------------------------------------
		id : instruction_decode port map(IF_ID_INST, m_read, m_write, reg_write, add_sub, alusrc, branch, jump, memtoreg, regdst, aluop, read_port1, read_port2, write_port);
	
	------------- RF --------------------------------------------------
		rf_mux : mux2to1 generic map (n=>4) port map (regdst, read_port2, write_port, rf_mux_out);
		rf : register_file port map (clock, reset, MEM_WB_reg_write, read_port1, read_port2, MEM_WB_Rd, dm_mux_out, value1, value2);
		offset <= IF_ID_INST(3 downto 0);
	
	---------- ID_EX Stage --------------
		hazard_mux_0 : mux2to1single port map(ID_EX_bubble, reg_write, '0', hazard_reg_write);
		hazard_mux_1 : mux2to1single port map(ID_EX_bubble, memtoreg, '0', hazard_memtoreg);
		hazard_mux_2 : mux2to1single port map(ID_EX_bubble, m_write, '0', hazard_m_write);
		hazard_mux_3 : mux2to1single port map(ID_EX_bubble, branch, '0', hazard_branch);
		hazard_mux_4 : mux2to1single port map(ID_EX_bubble, alusrc, '0', hazard_alusrc);
		hazard_mux_5 : mux2to1single port map(ID_EX_bubble, add_sub, '0', hazard_add_sub);
		hazard_mux_6 : mux2to1single port map(ID_EX_bubble, m_read, '0', hazard_m_read);
		hazard_mux_7 : mux2to1 generic map (n=>2) port map(ID_EX_bubble, aluop, "00", hazard_aluop);
		PC_ID_EX : regN generic map (n=>4) port map(clock, IF_ID_PC, ID_EX_PC);
		Val1_ID_EX : regN generic map (n=>4) port map(clock, value1, ID_EX_value1);
		Val2_ID_EX : regN generic map (n=>4) port map(clock, value2, ID_EX_value2);
		Off_ID_EX : regN generic map (n=>4) port map(clock, offset, ID_EX_offset);
		Rd_ID_EX : regN generic map (n=>4) port map(clock, rf_mux_out, ID_EX_Rd);
		reg_write_ID_EX : regOne port map(clock, hazard_reg_write, ID_EX_reg_write);
		memtoreg_ID_EX : regOne port map(clock, hazard_memtoreg, ID_EX_memtoreg);
		m_write_ID_EX : regOne port map(clock, hazard_m_write, ID_EX_m_write);
		branch_ID_EX : regOne port map(clock, hazard_branch, ID_EX_branch);
		alusrc_ID_EX : regOne port map(clock, hazard_alusrc, ID_EX_alusrc);
		aluop_ID_EX : regN generic map(n=>2) port map (clock, hazard_aluop, ID_EX_aluop);
		add_sub_ID_EX : regOne port map(clock, hazard_add_sub, ID_EX_add_sub);
		m_read_ID_EX : regOne port map(clock, hazard_m_read, ID_EX_m_read);
		r_p1_ID_EX : regN generic map (n=>4) port map(clock, read_port1, ID_EX_read_port1);
		r_p2_ID_EX : regN generic map (n=>4) port map(clock, read_port2, ID_EX_read_port2);
		
	------------- FOWRARDING -----------------------------------------
		f1 : forwarding port map (ID_EX_read_port1, EX_MEM_Rd, MEM_WB_Rd, ID_EX_read_port2, EX_MEM_reg_write, MEM_WB_reg_write, forwardA, forwardB);
		fa : mux4to1 generic map (n=>4) port map (ID_EX_value1, dm_mux_out, EX_MEM_result, "1111", forwardA, forwardA_out);
		fb : mux4to1 generic map (n=>4) port map (ID_EX_value2, dm_mux_out, EX_MEM_result, "0000", forwardB, forwardB_out);
		
	------------- ALU ------------------------------------------------
		alu_mux : mux2to1 generic map (n=>4) port map (ID_EX_alusrc, forwardB_out, ID_EX_offset, alu_mux_out);
		alogic : alu port map (ID_EX_add_sub, ID_EX_aluop, forwardA_out, alu_mux_out, alu_result, alu_zero);

	
	--------   EX_MEM Stage   -----------
		PC_EX_MEM : regN generic map (n=>4) port map(clock, ID_EX_PC, EX_MEM_PC);
		ALU_R_EX_MEM : regN generic map (n=>4) port map(clock, alu_result, EX_MEM_result);
		ZERO_EX_MEM : regOne port map(clock, alu_zero, EX_MEM_alu_zero);
		Val2_EX_MEM : regN generic map (n=>4) port map(clock, ID_EX_value2, EX_MEM_value2);
		Rd_EX_MEM : regN generic map (n=>4) port map(clock, ID_EX_Rd, EX_MEM_Rd);
		Off_EX_MEM : regN generic map (n=>4) port map(clock, ID_EX_offset, EX_MEM_offset);
		reg_write_EX_MEM : regOne port map (clock, ID_EX_reg_write, EX_MEM_reg_write);
		memtoreg_EX_MEM : regOne port map(clock, ID_EX_memtoreg, EX_MEM_memtoreg);
		m_write_EX_MEM : regOne port map(clock, ID_EX_m_write, EX_MEM_m_write);
		branch_EX_MEM : regOne port map(clock, ID_EX_branch, EX_MEM_branch);
	
	---------- DM ---------------------------------------------------
		branch_zero_result <= EX_MEM_branch AND EX_MEM_alu_zero;
		dm : data_memory port map (clock, reset, EX_MEM_m_write, EX_MEM_result, EX_MEM_result, EX_MEM_value2, data);
		branch_adder : ripple_carry port map('0', EX_MEM_PC, EX_MEM_offset, branch_target);
	
	
	--------  MEM_WB Stage  -------------
		Data_MEM_WB : regN generic map (n=>4) port map(clock, data, MEM_WB_data);
		ALU_R_MEM_WB : regN generic map (n=>4) port map(clock, EX_MEM_result, MEM_WB_result);
		Rd_MEM_WB : regN generic map (n=>4) port map(clock, EX_MEM_Rd, MEM_WB_Rd);
		memtoreg_MEM_WB : regOne port map(clock, EX_MEM_memtoreg, MEM_WB_memtoreg);
		reg_write_MEM_WB : regOne port map(clock, EX_MEM_reg_write, MEM_WB_reg_write);
		
		dm_mux : mux2to1 generic map (n=>4) port map (MEM_WB_memtoreg, MEM_WB_result, MEM_WB_data, dm_mux_out);
		
		jump_mux: mux2to1 generic map(n=>4) port map(IF_jump, update_pc, jump_target, jump_decide);
		jump_branch_mux: mux2to1 generic map(n=>4) port map(branch_zero_result, jump_decide, branch_target, pc_new);
		
		choose_mux : mux2to1 generic map (n=>4) port map (EX_MEM_m_write, dm_mux_out, EX_MEM_value2, result);
	
end behaviour;
