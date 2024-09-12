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


module controller_rom #(parameter 
mpu_address 	= 8'h00, 
aft_address 	= 8'h00, 
address_size 	= 14,
aft_master		= 0)
(

	input                  	clk,
	input                  	reset_n,
	
	// Instruction
	input [23:0]           	iBus_cmd_payload_pc,
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
		if (~reset_n || aft_master) begin
			iBus_rsp_valid 		<= 1'b0;
			iBus_cmd_ready 		<= 1'b0;
			mem_status				<= 0;
			BRAM_CONTROLL			<= 1;
			bridge_wr_reg			<= bridge_wr && bridge_addr[31:24] == aft_address;
			bridge_addr_reg 		<= bridge_addr;
			bridge_wr_data_reg	<= bridge_wr_data;
		end 
		else begin
			BRAM_CONTROLL	<= 0;
			bridge_wr_reg	<= 0;
			if (bridge_wr && bridge_addr[31:24] == aft_address) begin
				iBus_cmd_ready <= 1'b0;
				mem_status <= 1;
				BRAM_CONTROLL <= 1;
				bridge_wr_data_reg <= bridge_wr_data;
				bridge_addr_reg <= bridge_addr;
				bridge_wr_reg	<= 1;
			end
//			else if (bridge_rd && bridge_addr[31:16] == aft_address) begin
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

wire wren_a_data 			= data_addr[31:24] == mpu_address && data_we;

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

dbram #(.ADDRWIDTH (address_size) //Address lines for the memory array
)dbram (
	.clock_a		(clk),
	.address_a	(data_addr[23:2]),
	.data_a		(data_d),
	.byteena_a	(data_byte_select),
	.rden_a		(dBus_cmd_valid),
	.wren_a		(wren_a_data && dBus_cmd_valid),
	.q_a			(data_q),
	
	.address_b	(BRAM_CONTROLL  ? bridge_addr_reg[23:2] : iBus_cmd_payload_pc[23:2]),
	.rden_b		(BRAM_CONTROLL ? 1'b1 : iBus_cmd_valid),
	.clock_b		(clk),
	.data_b		({bridge_wr_data_reg[7:0], bridge_wr_data_reg[15:8], bridge_wr_data_reg[23:16], bridge_wr_data_reg[31:24]} ),
	.wren_b		(BRAM_CONTROLL ? bridge_wr_reg : 1'b0),
	.q_b			(iBus_rsp_payload_inst));



reg [31:0] bridge_rd_data_reg;
	
always @(posedge clk_74a) begin
	if(bridge_rd) bridge_rd_data_reg <= ~little_enden ? {iBus_rsp_payload_inst[7:0], iBus_rsp_payload_inst[15:8], iBus_rsp_payload_inst[23:16], iBus_rsp_payload_inst[31:24]} : iBus_rsp_payload_inst;
	bridge_rd_data <= bridge_rd_data_reg;
end
	
endmodule



module dbram #(parameter ADDRWIDTH=14)
(
	input	                 	clock_a,
	input	 [ADDRWIDTH-1:0] 	address_a,
	input  [3:0]				byteena_a,
	input	 [31:0] 				data_a,
	input	                 	wren_a,
	input 						rden_a,
	output [31:0] 				q_a,

	input	                 	clock_b,
	input	 [ADDRWIDTH-1:0] 	address_b,
	input	 [31:0] 				data_b,
	input	                 	wren_b,
	input 						rden_b,
	output [31:0] 				q_b
);

altsyncram altsyncram_component (
			.address_a (address_a),
			.address_b (address_b),
			.clock0 (clock_a),
			.clock1 (clock_b),
			.data_a (data_a),
			.data_b (data_b),
			.wren_a (wren_a),
			.wren_b (wren_b),
			.q_a (q_a),
			.q_b (q_b),
			.aclr0 (1'b0),
			.aclr1 (1'b0),
			.addressstall_a (1'b0),
			.addressstall_b (1'b0),
			.byteena_a (byteena_a),
			.byteena_b (1'b1),
			.clocken0 (1'b1),
			.clocken1 (1'b1),
			.clocken2 (1'b1),
			.clocken3 (1'b1),
			.eccstatus (),
			.rden_a (rden_a),
			.rden_b (rden_b));
defparam
	altsyncram_component.wrcontrol_wraddress_reg_b = "CLOCK1",
	altsyncram_component.address_reg_b = "CLOCK1",
	altsyncram_component.indata_reg_b = "CLOCK1",
	altsyncram_component.numwords_a = 1<<ADDRWIDTH,
	altsyncram_component.numwords_b = 1<<ADDRWIDTH,
	altsyncram_component.widthad_a = ADDRWIDTH,
	altsyncram_component.widthad_b = ADDRWIDTH,
	altsyncram_component.width_a = 32,
	altsyncram_component.width_b = 32,
	altsyncram_component.width_byteena_a = 4,
	altsyncram_component.width_byteena_b = 1,
 
	altsyncram_component.clock_enable_input_a = "NORMAL",
	altsyncram_component.clock_enable_input_b = "NORMAL",
	altsyncram_component.clock_enable_output_a = "BYPASS",
	altsyncram_component.clock_enable_output_b = "BYPASS",
	altsyncram_component.intended_device_family = "Cyclone V",
	altsyncram_component.lpm_type = "altsyncram",
	altsyncram_component.operation_mode = "BIDIR_DUAL_PORT",
	altsyncram_component.outdata_aclr_a = "NONE",
	altsyncram_component.outdata_aclr_b = "NONE",
	altsyncram_component.outdata_reg_a = "UNREGISTERED",
	altsyncram_component.outdata_reg_b = "UNREGISTERED",
	altsyncram_component.power_up_uninitialized = "FALSE",
	altsyncram_component.read_during_write_mode_mixed_ports = "DONT_CARE",
	altsyncram_component.read_during_write_mode_port_a = "NEW_DATA_NO_NBE_READ",
	altsyncram_component.read_during_write_mode_port_b = "NEW_DATA_NO_NBE_READ";

endmodule