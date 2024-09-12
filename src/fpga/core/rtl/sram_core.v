module sram_core(
	input wire 				clk,
	
	// Access Request
	
	input wire 				ce_rom, // This is a pulse to say do something now
	output reg 				RAM_RDY, // HIGH core is ready for a command
	// RAM Access
	
	input wire [14:0] 	RAM_addr,	// Ram 0 port 
	input wire				RAM_wr,		// Ram 0 write high
	input wire [7:0]		RAM_data,	// Ram 0 data in
	output reg [7:0]		RAM_q,		// Ram 0 data out on ready goes high
	input wire 				RAM_SEL,		// Ram 0 Select. this must be high and the ce-rom has to go high at the same time.
	
	// PRAM Access
	
	input wire [14:0] 	PRAM_addr,	// Ram 1 port 
	input wire				PRAM_wr,    // Ram 1 write high
	input wire [7:0]		PRAM_data,  // Ram 1 data in
	output reg [7:0]		PRAM_q,     // Ram 1 data out on ready goes high
	input wire 				PRAM_SEL,   // Ram 1 Select. this must be high and the ce-rom has to go high at the same time.
	
	// SRAM
	 
	output reg  [16:0] 	sram_a,		// SRAM Interface
	inout  wire [15:0] 	sram_dq,
	output reg        	sram_oe_n,
	output reg        	sram_we_n,
	output reg        	sram_ub_n,
	output reg        	sram_lb_n

);


  reg [3:0] 	ram_cnt;
  reg [1:0] 	ram_state;
  reg 			ce_rom_reg;
  reg [15:0] 	sram_d;
  
	always @(posedge clk) begin
		ce_rom_reg <= ce_rom;
		case(ram_state) 
			'd0 : begin
				sram_oe_n 		<= 1;	// all the signal outputs need to be high always unless used.
				sram_we_n 		<= 1;
				sram_ub_n 		<= 1;
				sram_lb_n 		<= 1;
				RAM_RDY			<= 1;
				if (&{ce_rom, ~ce_rom_reg} && |{PRAM_SEL, RAM_SEL}) begin
					sram_a 		<= PRAM_SEL ? {2'b01, PRAM_addr} : {2'b00, RAM_addr};	// This is only a 32K block at the moment for each area
					sram_oe_n 	<= PRAM_SEL ?  PRAM_wr :  RAM_wr;	// to output or not too. When write is high then OE is high too
					sram_we_n 	<= PRAM_SEL ? ~PRAM_wr : ~RAM_wr;	// to write or not too. When write is low then WE is low too low == write
					sram_ub_n 	<= 1'b0;	// For both reads these have to be low. but masking happens when this is high
					sram_lb_n 	<= 1'b0;	// For both reads these have to be low. but masking happens when this is high
					ram_state 	<= 'd1;	// I didnt make a state name..... why Agg?
					ram_cnt		<= 'd3; // we are running at 48mhz
					RAM_RDY		<= |{PRAM_wr, RAM_wr};	// Writes keep the Ready signal high. Only due to how the PCE runs
					sram_d		<= PRAM_SEL ? {2{PRAM_data}} : {2{RAM_data}}; // We just double the data output to the core. Should look at using the mask and reads for more memory
				end
			end
			'd1 : begin
				if (|ram_cnt) begin // wait for the counter to finish
					ram_cnt 		<= ram_cnt -1;
				end
				else begin	
					if (~sram_oe_n) begin // read done
						if (RAM_SEL)  RAM_q		<= sram_dq[7:0];
						if (PRAM_SEL) PRAM_q	<= sram_dq[7:0];
					end
					ram_state 	<= 'd0;
					sram_a 		<= 'd0;
					sram_oe_n 	<= 1; // reset everything
					sram_we_n 	<= 1;
					sram_ub_n 	<= 1;
					sram_lb_n 	<= 1;
					RAM_RDY	 	<= 1;
				end
			end
			default : begin
				ram_state 		<= 'd0;
				sram_a 			<= 'd0;
				sram_oe_n 		<= 1;
				sram_we_n 		<= 1;
				sram_ub_n 		<= 1;
				sram_lb_n 		<= 1;
			end
		endcase
	end
  assign sram_dq = sram_oe_n ? sram_d : {16{1'bz}}; // output tristate (I do it like this as in simulations you need too.)

endmodule