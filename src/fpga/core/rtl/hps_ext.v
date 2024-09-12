//
// hps_ext for TurboGrafx-16 CD
//
// Copyright (c) 2020 Alexey Melnikov
//
// This source file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This source file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////

module hps_ext
(
	input             	clk_sys,
	inout      [35:0] 	EXT_BUS,

	// CD interface
	input      [112:0] 	cd_in,
	output reg [112:0] 	cd_out,
	output reg [15:0]		cd_data_out,
	output reg				cd_dat_download,
	output reg				cdctl_wr,
	output reg 				cd_en,
	input 					AUDIO_almost_full,
	input 					CD_almost_full
 );

assign EXT_BUS[15:0] = io_dout;
wire [15:0] io_din = EXT_BUS[31:16];
//assign EXT_BUS[32] = dout_en;
wire io_strobe = EXT_BUS[33];
wire io_enable = EXT_BUS[34];

assign EXT_BUS[32] = |{AUDIO_almost_full, CD_almost_full, dout_en};

localparam EXT_CMD_MIN = CD_STATS;
localparam EXT_CMD_MAX = CD_DATA;

localparam CD_STATS = 'h33;
localparam CD_GET = 'h34;
localparam CD_SET = 'h35;
localparam CD_DATA = 'h36;

reg [15:0] io_dout;
reg        dout_en = 0;
reg [15:0] byte_cnt;

always@(posedge clk_sys) begin
	reg [15:0] cmd;
	reg  [7:0] cd_req = 0;
	reg        old_cd = 0; 
	cdctl_wr 		<= 0;
	old_cd 			<= cd_in[112];
	dout_en			<= 0;
	if(old_cd ^ cd_in[112]) cd_req <= cd_req + 1'd1;

	if(~io_enable) begin
		dout_en <= 0;
		io_dout <= 0;
		byte_cnt <= 0;
		cmd <= 0;
		cd_dat_download <= 0;
		if(cmd == CD_SET) cd_out[112] <= ~cd_out[112]; 
	end
	else if(io_strobe) begin

		io_dout <= 0;
		if(~&byte_cnt) byte_cnt <= byte_cnt + 1'd1;

		if(byte_cnt == 0) begin
			cmd <= io_din;
			dout_en <= (io_din == CD_STATS || io_din == CD_DATA || io_din == CD_SET);
			if(io_din == CD_GET) io_dout <= cd_req; 
			if (io_din == CD_DATA) cd_dat_download <= 1;
		end else begin
			
			dout_en <= (io_din == CD_STATS || io_din == CD_DATA || io_din == CD_SET);
			case(cmd)
				CD_STATS: case(byte_cnt)
							1: cd_en <= io_din[0];
				endcase

				CD_GET: case(byte_cnt)
							1: io_dout <= cd_in[15:0];
							2: io_dout <= cd_in[31:16];
							3: io_dout <= cd_in[47:32];
							4: io_dout <= cd_in[63:48];
							5: io_dout <= cd_in[79:64];
							6: io_dout <= cd_in[95:80];
							7: io_dout <= cd_in[111:96];
						endcase

				CD_SET: case(byte_cnt)
							1: cd_out[15:0]  <= io_din;
							2: cd_out[31:16] <= io_din;
							3: cd_out[47:32] <= io_din;
							4: cd_out[63:48] <= io_din;
							5: cd_out[79:64] <= io_din;
							6: cd_out[95:80] <= io_din;
							7: cd_out[111:96] <= io_din;
						endcase 
				CD_DATA: begin
					cd_data_out  	<= io_din;
					cdctl_wr 		<= 1;
				end
			endcase
		end
	end
end

endmodule
