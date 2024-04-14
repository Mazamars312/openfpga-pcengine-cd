`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 19.11.2022 14:07:05
// Design Name: 
// Module Name: controller_rom
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module controller_rom #(parameter top_address = 16'h8000, address_size = 5'd14)
(

	input                  	clk,
	input                  	reset_n,
	
	// Instruction
	input [15:0]           	iBus_cmd_payload_pc,
	output [31:0]      		iBus_rsp_payload_inst,
	input							iBus_cmd_valid,
	output reg					iBus_rsp_valid,
	output reg					iBus_cmd_ready,
	
	// Data Bus
	input [31:0]           	data_addr,
	input [31:0]           	data_d,
	output [31:0]      		data_q,
	input                  	data_we,
	input							dBus_cmd_valid,
	input [1:0]            	data_bytesel,
	
	// APF Bus
	input 						little_enden,
	input 	     		 		clk_74a,
	input [31:0]  		 		bridge_addr,
	input 	     		 		bridge_rd,
	output reg [31:0]  	 	bridge_rd_data,
	input 	     		 		bridge_wr,
	input  [31:0]   	 		bridge_wr_data
);

// Imem Controller
	reg [2:0]	mem_status;
	reg [31:0]	bridge_addr_reg;
	reg [31:0] 	bridge_wr_data_reg;
	reg 			bridge_wr_reg;
	reg 			BRAM_CONTROLL;

	always @(posedge clk) begin
		if (~reset_n) begin
			iBus_rsp_valid 		<= 1'b0;
			iBus_cmd_ready 		<= 1'b0;
			mem_status				<= 0;
			BRAM_CONTROLL			<= 1;
			bridge_wr_reg			<= bridge_wr && bridge_addr[31:16] == top_address;
			bridge_addr_reg 		<= bridge_addr;
			bridge_wr_data_reg	<= bridge_wr_data;
		end 
		else begin
			BRAM_CONTROLL	<= 0;
			bridge_wr_reg	<= 0;
			if (bridge_wr && bridge_addr[31:16] == top_address) begin
				iBus_cmd_ready <= 1'b0;
				mem_status <= 1;
				BRAM_CONTROLL <= 1;
				bridge_wr_data_reg <= bridge_wr_data;
				bridge_addr_reg <= bridge_addr;
				bridge_wr_reg	<= 1;
			end
//			else if (bridge_rd && bridge_addr[31:16] == top_address) begin
//				iBus_cmd_ready <= 1'b0;
//				mem_status <= 2;
//				bridge_addr_reg <= bridge_addr;
//			end
			else begin
				if (mem_status == 0)iBus_cmd_ready <= 1'b1;
			end
			
			if(iBus_cmd_valid && iBus_cmd_ready) begin
				// Can read without stall
				iBus_rsp_valid <= 1'b1;
			end 
			else begin
				iBus_rsp_valid <= 1'b0;
			end
			
			case (mem_status)
				1 : begin
					BRAM_CONTROLL	<= 0;
					mem_status		<= 0;
				end
//				2 : begin
//					BRAM_CONTROLL	<= 1;
//				end
//				3 : begin
//					BRAM_CONTROLL	<= 1;
//					q_b_reg			<= iBus_rsp_payload_inst;
//				end
				default : begin
					mem_status		<= 0;				
				end
			endcase
		end
	end



// Data wires
wire [31:0] q_b, data_q_dmem, data_q_imem, bridge_rd_data_dmem;
reg 	[31:0]	q_b_reg;

// APF Bus write selectors
//wire wren_b_instruction = bridge_addr[31:16] == top_address && bridge_wr && ~bridge_addr[15];
//wire wren_b_data 			= bridge_addr[31:16] == top_address && bridge_wr &&  bridge_addr[15];

//wire wren_a_instruction = data_addr[31:16] == 16'h0000 && data_we && ~data_addr[15];
//wire wren_a_data 			= data_addr[31:16] == 16'h0000 && data_we &&  data_addr[15];
wire wren_a_data 			= data_addr[31:16] == 16'h0000 && data_we;

reg [3:0] data_byte_select;

always @* begin
	case ({data_bytesel,data_addr[1:0]})
		{2'b00, 2'b00} : data_byte_select <= 4'b0001;
		{2'b00, 2'b01} : data_byte_select <= 4'b0010;
		{2'b00, 2'b10} : data_byte_select <= 4'b0100;
		{2'b00, 2'b11} : data_byte_select <= 4'b1000;
		
		{2'b01, 2'b00} : data_byte_select <= 4'b0011;
		{2'b01, 2'b01} : data_byte_select <= 4'b0110;
		{2'b01, 2'b10} : data_byte_select <= 4'b1100;
		{2'b01, 2'b11} : data_byte_select <= 4'b1000;
		
		default  		: data_byte_select <= 4'b1111;
	endcase
end

// This is a 32K ram block for the MPU CPU access and is switched to the APF bus when in reset for uploading code.
// It will allow the instruction and data sides of the MPU to read instructions and R/W Data at the same time.


// The A side is the Data side.
// The B Side is the instruction and APF UPLOAD side.

//imem_bram imem_bram (
//	.clock_a		(clk),
//	.address_a	(data_addr[14:2]),
//	.data_a		(data_d),
//	.byteena_a	(data_byte_select),
//	.rden_a		(dBus_cmd_valid),
//	.wren_a		(wren_a_instruction),
//	.q_a			(data_q_imem),
//	
//	.clock_b		(clk),
//	.address_b	(reset_n ? iBus_cmd_payload_pc[14:2] 	: bridge_addr[14:2]),
//	.rden_b		(reset_n ? iBus_cmd_valid 		: bridge_rd),
//	.data_b		({bridge_wr_data[7:0], bridge_wr_data[15:8], bridge_wr_data[23:16], bridge_wr_data[31:24]}),
//	.wren_b		(reset_n ? 0 						: wren_b_instruction),
//	.q_b			(iBus_rsp_payload_inst));
	
// This is a 2K ram block for the MPU CPU access and APF bus for a buffer core.	
dmem_bram dmem_bram (
	.clock_a		(clk),
	.address_a	(data_addr[15:2]),
	.data_a		(data_d),
	.byteena_a	(data_byte_select),
	.rden_a		(dBus_cmd_valid),
	.wren_a		(wren_a_data),
	.q_a			(data_q),
	
	.address_b	(BRAM_CONTROLL ? bridge_addr_reg[15:2] : iBus_cmd_payload_pc[15:2]),
	.rden_b		(BRAM_CONTROLL ? 1'b1 : iBus_cmd_valid),
	.clock_b		(clk),
	.data_b		({bridge_wr_data_reg[7:0], bridge_wr_data_reg[15:8], bridge_wr_data_reg[23:16], bridge_wr_data_reg[31:24]} ),
	.wren_b		(BRAM_CONTROLL ? bridge_wr_reg : 1'b0),
	.q_b			(iBus_rsp_payload_inst));

// MPU Data Side to mux the two ram locations
	
//reg data_addr_mux;
	
//always @(posedge clk) data_addr_mux <= data_addr[15];
	
//assign data_q = data_addr_mux ? data_q_dmem : data_q_imem;

// APF Address

reg bridge_addr_mux;

always @(posedge clk) if (bridge_rd) bridge_addr_mux <= bridge_addr[15];

//assign q_b_wire = bridge_addr_mux ? bridge_rd_data_dmem : iBus_rsp_payload_inst;
	
always @(posedge clk_74a) bridge_rd_data <= ~little_enden ? {iBus_rsp_payload_inst[7:0], iBus_rsp_payload_inst[15:8], iBus_rsp_payload_inst[23:16], iBus_rsp_payload_inst[31:24]} : iBus_rsp_payload_inst;
	
endmodule
