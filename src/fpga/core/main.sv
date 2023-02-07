module pce (
    input wire clk_sys_42_95,
    input wire clk_mem_85_91,

    input wire core_reset,
    input wire pll_core_locked,

    // Input
    input wire p1_button_1,
    input wire p1_button_2,
    input wire p1_button_3,
    input wire p1_button_4,
    input wire p1_button_5,
    input wire p1_button_6,
    input wire p1_button_select,
    input wire p1_button_start,
    input wire p1_dpad_up,
    input wire p1_dpad_down,
    input wire p1_dpad_left,
    input wire p1_dpad_right,

    input wire p2_button_1,
    input wire p2_button_2,
    input wire p2_button_3,
    input wire p2_button_4,
    input wire p2_button_5,
    input wire p2_button_6,
    input wire p2_button_select,
    input wire p2_button_start,
    input wire p2_dpad_up,
    input wire p2_dpad_down,
    input wire p2_dpad_left,
    input wire p2_dpad_right,

    input wire p3_button_1,
    input wire p3_button_2,
    input wire p3_button_3,
    input wire p3_button_4,
    input wire p3_button_5,
    input wire p3_button_6,
    input wire p3_button_select,
    input wire p3_button_start,
    input wire p3_dpad_up,
    input wire p3_dpad_down,
    input wire p3_dpad_left,
    input wire p3_dpad_right,

    input wire p4_button_1,
    input wire p4_button_2,
    input wire p4_button_3,
    input wire p4_button_4,
    input wire p4_button_5,
    input wire p4_button_6,
    input wire p4_button_select,
    input wire p4_button_start,
    input wire p4_dpad_up,
    input wire p4_dpad_down,
    input wire p4_dpad_left,
    input wire p4_dpad_right,

    input wire sgx,
	 input wire AC_EN,

    // Settings
    input wire turbo_tap_enable,
    input wire button6_enable,
    input wire [1:0] button1_turbo_speed,
    input wire [1:0] button2_turbo_speed,

    input wire overscan_enable,
    input wire extra_sprites_enable,
    input wire raw_rgb_enable,

    input wire mb128_enable,

    input wire cd_audio_boost,
    input wire adpcm_audio_boost,
    input wire [1:0] master_audio_boost,

    // Data in
    input wire        ioctl_wr,
    input wire [23:0] ioctl_addr,
    input wire [15:0] ioctl_dout,
    input wire        cart_download,

    // Data out
    input wire sd_wr,
    input wire [7:0] sd_buff_addr,
    input wire [16:0] sd_lba,
    input wire [15:0] sd_buff_dout,
    output wire [15:0] sd_buff_din,
    input wire save_download,

    // SDRAM
    output wire [12:0] dram_a,
    output wire [ 1:0] dram_ba,
    inout  wire [15:0] dram_dq,
    output wire [ 1:0] dram_dqm,
    output wire        dram_clk,
    output wire        dram_cke,
    output wire        dram_ras_n,
    output wire        dram_cas_n,
    output wire        dram_we_n,
	 
	 // SRAM
	 
    output wire [16:0] 	sram_a,
    inout  wire [15:0] 	sram_dq,
    output wire        	sram_oe_n,
    output wire        	sram_we_n,
    output wire        	sram_ub_n,
    output wire        	sram_lb_n,
	 	 
	 // CRAM
	 output wire [21:16] cram0_a,
    inout  wire [ 15:0] cram0_dq,
    input  wire         cram0_wait,
    output wire         cram0_clk,
    output wire         cram0_adv_n,
    output wire         cram0_cre,
    output wire         cram0_ce0_n,
    output wire         cram0_ce1_n,
    output wire         cram0_oe_n,
    output wire         cram0_we_n,
    output wire         cram0_ub_n,
    output wire         cram0_lb_n,

    output wire [21:16] cram1_a,
    inout  wire [ 15:0] cram1_dq,
    input  wire         cram1_wait,
    output wire         cram1_clk,
    output wire         cram1_adv_n,
    output wire         cram1_cre,
    output wire         cram1_ce0_n,
    output wire         cram1_ce1_n,
    output wire         cram1_oe_n,
    output wire         cram1_we_n,
    output wire         cram1_ub_n,
    output wire         cram1_lb_n,

    // Video
    output reg ce_pix,
    output wire hsync,
    output wire vsync,
    output wire hblank,
    output wire vblank,
    output wire [7:0] video_r,
    output wire [7:0] video_g,
    output wire [7:0] video_b,

    output wire [1:0] dotclock_divider,
    output wire border,

    // Audio
    output wire [15:0] audio_l,
    output wire [15:0] audio_r,
	 
	 // CDROM Access
	 inout  			[35:0] EXT_BUS
);

  wire                                       [63:0] status = 0;

  wire code_download = 0;

  // wire img_mounted = 0;
  // wire img_readonly = 0;
  // wire sd_ack = 0;
  // reg                                                              sd_rd = 0;
  // reg                                                              sd_wr = 0;
  // reg                                   [31:0]                     sd_lba = 0;
  wire        cd_en;

  wire VDC_BG_EN = 1;
  wire VDC_SPR_EN = 1;
  wire                                       [ 1:0] VDC_GRID_EN = 2'd0;
  wire CPU_PAUSE_EN = 0;

  wire reset = (core_reset | save_download);
  
  
	wire [14:0] PRAM_addr, 	RAM_addr;
	wire [7:0] 	PRAM_q, 		RAM_q;
	wire [7:0] 	PRAM_data, 	RAM_data;
	wire 			PRAM_wr,		RAM_wr;
	wire 			PRAM_SEL,	RAM_SEL;

	wire [15:0] VRAM1_A;
	wire [15:0] VRAM1_DI;
	wire [15:0] VRAM1_DO;
	wire 			VRAM1_WE;
	wire 			VIDEO_CE;

  wire overscan = ~status[17];

  wire [95:0]  cd_comm;
  wire         cd_comm_send;
  reg  [15:0]  cd_stat;
  reg          cd_stat_rec;
  reg          cd_dataout_req;
  wire [79:0]  cd_dataout;
  wire         cd_dataout_send;
  wire         cd_reset_req;
  reg          cd_region;

  wire [21:0]  cd_ram_a;
  wire 			cd_ram_rd, cd_ram_wr;
  wire [7:0] 	cd_ram_do;

  wire       	ce_rom;
  reg				RAM_RDY;

  wire [15:0] 	cdda_sl, cdda_sr, adpcm_s, psg_sl, psg_sr;

  pce_top #(LITE) pce_top (
      .RESET(reset | cart_download),
      .COLD_RESET(cart_download),

      .CLK(clk_sys_42_95),

      .ROM_RD(rom_rd),
      .ROM_RDY(rom_sdrdy),
      // .ROM_RDY(~rom_sdbusy),
      .ROM_A(rom_rdaddr),
      .ROM_DO(rom_sdata),
      .ROM_SZ(romwr_a[23:12]),
      .ROM_POP(populous[romwr_a[9]]),
      .ROM_CLKEN(ce_rom),

      .BRM_A (bram_addr),
      .BRM_DO(bram_q),
      .BRM_DI(bram_data),
      .BRM_WE(bram_wr),
		
		.RAM_A (RAM_addr),
      .RAM_DO(RAM_q),
      .RAM_DI(RAM_data),
      .RAM_WE(RAM_wr),
		.RAM_SEL(RAM_SEL),
		
		.VRAM1_A		(VRAM1_A),
		.VRAM1_DI	(VRAM1_DI),
		.VRAM1_DO	(VRAM1_DO),
		.VRAM1_WE	(VRAM1_WE),
		
		.PRAM_A (PRAM_addr),
      .PRAM_DO(PRAM_q),
      .PRAM_DI(PRAM_data),
      .PRAM_WE(PRAM_wr),
		.PRAM_SEL(PRAM_SEL),
		
		.RAM_RDY(RAM_RDY),

      .GG_EN(status[5]),
      .GG_CODE(gg_code),
      .GG_RESET((cart_download | code_download) & ioctl_wr & !ioctl_addr),
      .GG_AVAIL(gg_avail),

      .SP64(extra_sprites_enable),
      .SGX (sgx && !LITE),

      .JOY_OUT(joy_out),
      .JOY_IN (joy_in),

      .CD_EN(cd_en),
      // Arcade card
      .AC_EN(AC_EN),

       .CD_RAM_A (cd_ram_a),
       .CD_RAM_DO(cd_ram_do),
       .CD_RAM_DI(rom_sdata),
       .CD_RAM_RD(cd_ram_rd),
       .CD_RAM_WR(cd_ram_wr),

       .CD_STAT(cd_stat[7:0]),
       .CD_MSG(cd_stat[15:8]),
       .CD_STAT_GET(cd_stat_rec),

       .CD_COMM(cd_comm),
       .CD_COMM_SEND(cd_comm_send),

       .CD_DOUT_REQ(cd_dataout_req),
       .CD_DOUT(cd_dataout),
       .CD_DOUT_SEND(cd_dataout_send),

       .CD_REGION(cd_region),
       .CD_RESET (cd_reset_req),

       .CD_DATA(!cd_dat_byte ? cd_dat[7:0] : cd_dat[15:8]),
       .CD_WR(cd_wr),
       .CD_DATA_END(cd_dat_req),
       .CD_DM(cd_dm),

      .CDDA_SL(cdda_sl),
      .CDDA_SR(cdda_sr),
      .ADPCM_S(adpcm_s),
      .PSG_SL (psg_sl),
      .PSG_SR (psg_sr),

      .BG_EN(VDC_BG_EN),
      .SPR_EN(VDC_SPR_EN),
      .GRID_EN(VDC_GRID_EN),
      .CPU_PAUSE_EN(CPU_PAUSE_EN),

      .ReducedVBL(~overscan_enable),
      .BORDER_EN(0),
      .DOTCLOCK_DIVIDER(dotclock_divider),
      .VIDEO_R(r),
      .VIDEO_G(g),
      .VIDEO_B(b),
      .VIDEO_BW(bw),
      //.VIDEO_CE(ce_vid),
      .VIDEO_CE_FS(ce_vid),
      .VIDEO_VS(vs),
      .VIDEO_HS(hs),
      .VIDEO_HBL(hbl),
      .VIDEO_VBL(vbl),

      .BORDER_OUT(border)
  );
  


  ////////////////////////////  CD communication   ///////////////////////////////////

   reg  [112:0] 	cd_in = 0;
   wire [112:0] 	cd_out;
	wire 				cd_dat_download;
	wire 				cdctl_wr;
	wire [15:0]		cd_data_out;
   hps_ext hps_ext
   (
   	.clk_sys(clk_sys_42_95),
   	.EXT_BUS(EXT_BUS),
   	.cd_in(cd_in),
   	.cd_out(cd_out),
		.cd_dat_download(cd_dat_download),
		.cdctl_wr(cdctl_wr),
		.cd_data_out(cd_data_out),
		.cd_en(cd_en)
   );

   reg        cd_dat_req;
   always @(posedge clk_sys_42_95) begin
   	reg cd_out112_last = 1;
   	reg cd_comm_send_old = 0, cd_dataout_send_old = 0, cd_dat_req_old = 0, cd_reset_req_old = 0;

   	cd_stat_rec <= 0;
   	cd_dataout_req <= 0;
   	if (reset) begin
   		cd_region <= 0;
   	end
   	else begin
   		if (cd_out[112] != cd_out112_last) begin
   			cd_out112_last <= cd_out[112];

   			cd_stat <= cd_out[15:0];
   			cd_stat_rec <= ~cd_out[16];
   			cd_dataout_req <= cd_out[16];
   			cd_region <= cd_out[17];
   		end

   		cd_comm_send_old <= cd_comm_send;
   		cd_dataout_send_old <= cd_dataout_send;
   		cd_dat_req_old <= cd_dat_req;
   		cd_reset_req_old <= cd_reset_req;
   		if (cd_comm_send && !cd_comm_send_old) begin
   			cd_in[95:0] <= cd_comm;
   			cd_in[111:96] <= {cd_en,15'd0};
   			cd_in[112] <= ~cd_in[112];
   		end
   		else if (cd_dataout_send && !cd_dataout_send_old) begin
   			cd_in[79:0] <= cd_dataout;
   			cd_in[111:96] <= 16'h0001;
   			cd_in[112] <= ~cd_in[112];
   		end
   		else if (cd_dat_req && !cd_dat_req_old) begin
   			cd_in[111:96] <= 16'h0002;
   			cd_in[112] <= ~cd_in[112];
   		end
   		else if (cd_reset_req && !cd_reset_req_old) begin
   			cd_in[111:96] <= 16'h00FF;
   			cd_in[112] <= ~cd_in[112];
   		end
   	end
   end

  reg [15:0] cd_dat = 0;
  reg        cd_wr = 0;
  reg        cd_dat_byte = 0;
  reg        cd_dm = 0;
   always @(posedge clk_sys_42_95) begin
   	reg old_download;
   	reg head_pos, cd_dat_write;
   	reg [14:0] cd_dat_len, cd_dat_cnt;

   	old_download <= cd_dat_download;
   	if ((~old_download && cd_dat_download) || reset) begin
   		head_pos <= 0;
   		cd_dat_len <= 0;
   		cd_dat_cnt <= 0;
   	end
   	else if (cdctl_wr && cd_dat_download) begin
   		if (!head_pos) begin
   			{cd_dm,cd_dat_len} <= cd_data_out;
   			cd_dat_cnt <= 0;
   			head_pos <= 1;
   		end
   		else if (cd_dat_cnt < cd_dat_len) begin
   			cd_dat_write <= 1;
   			cd_dat_byte <= 0;
   			cd_dat <= cd_data_out;
   		end
   	end

   	if (cd_dat_write) begin
   		if (!cd_wr) begin
   			cd_wr <= 1;
   		end
   		else begin
   			cd_wr <= 0;
   			cd_dat_byte <= ~cd_dat_byte;
   			cd_dat_cnt <= cd_dat_cnt + 15'd1;
   			if (cd_dat_byte || cd_dat_cnt >= cd_dat_len-1) begin
   				cd_dat_write <= 0;
   			end
   		end
   	end
   end

  ////////////////////////////  VIDEO  ///////////////////////////////////

  wire [2:0] r, g, b;
  wire hs, vs;
  wire hbl, vbl;
  wire bw;

  wire ce_vid;

  always @(posedge clk_mem_85_91) begin
    reg old_ce;

    old_ce <= ce_vid;
    ce_pix <= ~old_ce & ce_vid;
  end

  logic [23:0] pal_color;

  dpram #(
      .addr_width(9),
      .data_width(24),
      .mem_init_file("palette.mif")
  ) palette_ram (
      .clock(clk_mem_85_91),

      .address_a({g, r, b}),
      .q_a(pal_color)
  );

  logic [7:0] r1, b1, g1;

  assign {r1, g1, b1} = raw_rgb_enable ? {{r, r, r[2:1]}, {g, g, g[2:1]}, {b, b, b[2:1]}} : pal_color;

  color_mix color_mix (
      .clk_vid(clk_mem_85_91),
      .ce_pix(ce_pix),
      .mix(bw ? 3'd5 : 0),

      .R_in(r1),
      .G_in(g1),
      .B_in(b1),
      .HSync_in(hs),
      .VSync_in(vs),
      .HBlank_in(hbl),
      .VBlank_in(vbl),

      .R_out(video_r),
      .G_out(video_g),
      .B_out(video_b),
      .HSync_out(hsync),
      .VSync_out(vsync),
      .HBlank_out(hblank),
      .VBlank_out(vblank)
  );

  ////////////////////////////  AUDIO  ///////////////////////////////////

  pce_audio pce_audio (
      .clk_sys_42_95(clk_sys_42_95),

      .cd_audio_boost(cd_audio_boost),
      .adpcm_audio_boost(adpcm_audio_boost),
      .master_audio_boost(master_audio_boost),

      .cdda_sl(cdda_sl),
      .cdda_sr(cdda_sr),
      .adpcm_s(adpcm_s),
      .psg_sl (psg_sl),
      .psg_sr (psg_sr),

      .audio_l(audio_l),
      .audio_r(audio_r)
  );

  ////////////////////////////  MEMORY  //////////////////////////////////

  localparam LITE = 1;

  wire [21:0] rom_rdaddr;
  wire [ 7:0] rom_sdata;
  wire rom_rd, rom_sdrdy;

  sdram sdram (
      .init(~pll_core_locked),
      .clk(clk_mem_85_91),
      .clkref(ce_rom),

      .waddr(cart_download ? romwr_a : {3'b001, cd_ram_a}),
      .din(cart_download ? romwr_d : {cd_ram_do, cd_ram_do}),
      .we(~cart_download & cd_ram_wr & ce_rom),
      .we_req(rom_wr),
      // .we_ack(sd_wrack),

      .raddr(rom_rd ? {3'b000, (rom_rdaddr + (romwr_a[9] ? 22'h200 : 22'h0))} : {3'b001, cd_ram_a}),
      .rd((rom_rd | cd_ram_rd) & ce_rom),
      .rd_rdy(rom_sdrdy),
      .dout(rom_sdata),

      // Actual SDRAM interface
      .SDRAM_DQ(dram_dq),
      .SDRAM_A(dram_a),
      .SDRAM_DQML(dram_dqm[0]),
      .SDRAM_DQMH(dram_dqm[1]),
      .SDRAM_BA(dram_ba),
      //   .SDRAM_nCS(),
      .SDRAM_nWE(dram_we_n),
      .SDRAM_nRAS(dram_ras_n),
      .SDRAM_nCAS(dram_cas_n),
      .SDRAM_CLK(dram_clk),
      .SDRAM_CKE(dram_cke)
  );
  
  sram_core sram_core(
	.clk			(clk_sys_42_95),
	.ce_rom		(ce_rom),
	.RAM_RDY		(RAM_RDY),
	.RAM_addr	(RAM_addr),
	.RAM_wr		(RAM_wr),
	.RAM_SEL		(RAM_SEL),
	.RAM_data	(RAM_data),
	.RAM_q		(RAM_q),
	.PRAM_addr	(PRAM_addr),
	.PRAM_wr		(PRAM_wr),
	.PRAM_SEL	(PRAM_SEL),
	.PRAM_data	(PRAM_data),
	.PRAM_q		(PRAM_q),
	.sram_a		(sram_a),
	.sram_dq		(sram_dq),
	.sram_oe_n	(sram_oe_n),
	.sram_we_n	(sram_we_n),
	.sram_ub_n	(sram_ub_n),
	.sram_lb_n	(sram_lb_n)

);

  // Video Ram
//  reg VIDEO_CE_delay;
//  
//  always @(posedge clk_mem_85_91) VIDEO_CE_delay <= VIDEO_CE;
//  
//  opb_psram_v VDP0_RAM(
//		.OPB_ABus		({8'd0, VRAM0_A[14:0], 1'b0}),
//      .OPB_BE			(2'b00),
//      .OPB_Clk			(clk_mem_85_91),
//      .OPB_DBus		(VRAM0_DO),
//		.OPB_32Bit		(1'b0),
//      .OPB_RNW			(~VRAM0_WE),
//      .OPB_Rst			(~reset),
//      .OPB_select		(~VIDEO_CE_delay && ~VRAM0_A[15]),
//      .Sln_DBus		(VRAM0_DI),
//      .Sln_xferAck	(),
//      .cram_a			(cram0_a),
//		.cram_dq			(cram0_dq),
//		.cram_wait		(cram0_wait),
//		.cram_clk		(cram0_clk),
//		.cram_adv_n		(cram0_adv_n),
//		.cram_cre		(cram0_cre),
//		.cram_ce0_n		(cram0_ce0_n),
//		.cram_ce1_n		(cram0_ce1_n),
//		.cram_oe_n		(cram0_oe_n),
//		.cram_we_n		(cram0_we_n),
//		.cram_ub_n		(cram0_ub_n),
//		.cram_lb_n		(cram0_lb_n)
//);
//
//  // Video Ram
//  
//  
//  opb_psram_v VDP1_RAM(
//		.OPB_ABus		({8'd0, VRAM1_A[14:0], 1'b0}),
//      .OPB_BE			(4'b0000),
//      .OPB_Clk			(clk_mem_85_91),
//      .OPB_DBus		({2{VRAM1_DO}}),
//		.OPB_32Bit		(1'b0),
//      .OPB_RNW			(~VRAM1_WE),
//      .OPB_Rst			(~reset),
//      .OPB_select		(~VIDEO_CE_delay && ~VRAM1_A[15]),
//      .Sln_DBus		(VRAM1_DI),
//      .Sln_xferAck	(),
//      .cram_a			(cram1_a),
//		.cram_dq			(cram1_dq),
//		.cram_wait		(cram1_wait),
//		.cram_clk		(cram1_clk),
//		.cram_adv_n		(cram1_adv_n),
//		.cram_cre		(cram1_cre),
//		.cram_ce0_n		(cram1_ce0_n),
//		.cram_ce1_n		(cram1_ce1_n),
//		.cram_oe_n		(cram1_oe_n),
//		.cram_we_n		(cram1_we_n),
//		.cram_ub_n		(cram1_ub_n),
//		.cram_lb_n		(cram1_lb_n)
//);


  wire romwr_ack;
  reg [23:0] romwr_a;
  wire [15:0] romwr_d = status[3] ?
		{ ioctl_dout[8], ioctl_dout[9], ioctl_dout[10],ioctl_dout[11],ioctl_dout[12],ioctl_dout[13],ioctl_dout[14],ioctl_dout[15],
		  ioctl_dout[0], ioctl_dout[1], ioctl_dout[2], ioctl_dout[3], ioctl_dout[4], ioctl_dout[5], ioctl_dout[6], ioctl_dout[7] }
		: ioctl_dout;

  reg prev_ioctl_wr = 0;
  reg rom_wr = 0;
  wire sd_wrack;

  // Special support for the Populous ROM
  reg [1:0] populous;
  // reg sgx;
  always @(posedge clk_sys_42_95) begin
    reg old_download;

    old_download  <= cart_download;
    // old_reset <= reset;
    prev_ioctl_wr <= ioctl_wr;

    // if (~old_reset && reset) ioctl_wait <= 0;
    if (~old_download && cart_download) begin
      romwr_a  <= 0;
      populous <= 2'b11;
      // sgx <= ioctl_index[0];
    end else begin
      if (ioctl_wr && ~prev_ioctl_wr) begin
        // ioctl_wait <= 1;
        rom_wr <= ~rom_wr;
        // Hacks for Populous game
        if ((romwr_a[23:4] == 'h212) || (romwr_a[23:4] == 'h1f2)) begin
          case (romwr_a[3:0])
            6:  if (romwr_d != 'h4F50) populous[romwr_a[13]] <= 0;
            8:  if (romwr_d != 'h5550) populous[romwr_a[13]] <= 0;
            10: if (romwr_d != 'h4F4C) populous[romwr_a[13]] <= 0;
            12: if (romwr_d != 'h5355) populous[romwr_a[13]] <= 0;
          endcase
        end
      end  // else if (rom_wr == sd_wrack) begin
      else if (~ioctl_wr && prev_ioctl_wr) begin
        // Falling edge of ioctl_wr
        // ioctl_wait <= 0;
        romwr_a <= romwr_a + 24'd2;
      end
    end
  end

  ////////////////////////////  CODES  ///////////////////////////////////

  reg [128:0] gg_code;
  wire gg_avail;

  ////////////////////////////  INPUT  ///////////////////////////////////

  wire button1_turbo_enable = button1_turbo_speed == 1 ? turbo_counter[2] :
                              button1_turbo_speed == 2 ? turbo_counter[1] :
                              1;

  wire button2_turbo_enable = button2_turbo_speed == 1 ? turbo_counter[2] :
                              button2_turbo_speed == 2 ? turbo_counter[1] :
                              1;

  wire [11:0] joy_0 = {
    p1_button_6,
    p1_button_5,
    p1_button_4,
    p1_button_3,
    p1_button_start,
    p1_button_select,
    button6_enable ? p1_button_2 : p1_button_2 | (button2_turbo_enable && p1_button_4),
    button6_enable ? p1_button_1 : p1_button_1 | (button1_turbo_enable && p1_button_3),
    p1_dpad_up,
    p1_dpad_down,
    p1_dpad_left,
    p1_dpad_right
  };

  wire [11:0] joy_1 = {
    p2_button_6,
    p2_button_5,
    p2_button_4,
    p2_button_3,
    p2_button_start,
    p2_button_select,
    button6_enable ? p2_button_2 : p2_button_2 | (button2_turbo_enable && p2_button_4),
    button6_enable ? p2_button_1 : p2_button_1 | (button1_turbo_enable && p2_button_3),
    p2_dpad_up,
    p2_dpad_down,
    p2_dpad_left,
    p2_dpad_right
  };

  wire [11:0] joy_2 = {
    p3_button_6,
    p3_button_5,
    p3_button_4,
    p3_button_3,
    p3_button_start,
    p3_button_select,
    button6_enable ? p3_button_2 : p3_button_2 | (button2_turbo_enable && p3_button_4),
    button6_enable ? p3_button_1 : p3_button_1 | (button1_turbo_enable && p3_button_3),
    p3_dpad_up,
    p3_dpad_down,
    p3_dpad_left,
    p3_dpad_right
  };

  wire [11:0] joy_3 = {
    p4_button_6,
    p4_button_5,
    p4_button_4,
    p4_button_3,
    p4_button_start,
    p4_button_select,
    button6_enable ? p4_button_2 : p4_button_2 | (button2_turbo_enable && p4_button_4),
    button6_enable ? p4_button_1 : p4_button_1 | (button1_turbo_enable && p4_button_3),
    p4_dpad_up,
    p4_dpad_down,
    p4_dpad_left,
    p4_dpad_right
  };

  wire [11:0] joy_4 = 0;

  wire [15:0] joy_data  /* synthesis keep */;
  always_comb begin
    case (joy_port)
      0:
      joy_data = (status[27:26] == 2'b01) ? {mouse_data, mouse_data} :
						                            ~{4'hF, joy_0[11:8], joy_0[1], joy_0[2], joy_0[0], joy_0[3], joy_0[7:4]};

      1:
      joy_data = (status[27:26] == 2'b10) ? pachinko                 : ~{4'hF, joy_1[11:8], joy_1[1], joy_1[2], joy_1[0], joy_1[3], joy_1[7:4]};
      2: joy_data = ~{4'hF, joy_2[11:8], joy_2[1], joy_2[2], joy_2[0], joy_2[3], joy_2[7:4]};
      3: joy_data = ~{4'hF, joy_3[11:8], joy_3[1], joy_3[2], joy_3[0], joy_3[3], joy_3[7:4]};
      4: joy_data = ~{4'hF, joy_4[11:8], joy_4[1], joy_4[2], joy_4[0], joy_4[3], joy_4[7:4]};
      default: joy_data = 16'h0FFF;
    endcase
  end

  reg [6:0] pachinko = 0;


  wire [7:0] mouse_data = 0;

  reg [3:0] joy_latch;
  reg [2:0] joy_port;
  reg [1:0] mouse_cnt;
  reg [7:0] ms_x, ms_y;
  reg [3:0] turbo_counter = 0;

  always @(posedge clk_sys_42_95) begin : input_block
    reg [ 1:0] last_gp;
    reg        high_buttons;
    reg [14:0] mouse_to;
    reg        ms_stb;
    reg [7:0] msr_x, msr_y;

    joy_latch <= joy_data[{high_buttons, joy_out[0], 2'b00}+:4];

    last_gp   <= joy_out;

    if (joy_out[1]) mouse_to <= 0;
    else if (~&mouse_to) mouse_to <= mouse_to + 1'd1;

    if (&mouse_to) mouse_cnt <= 3;
    if (~last_gp[1] & joy_out[1]) begin
      mouse_cnt <= mouse_cnt + 1'd1;
      if (&mouse_cnt) begin
        ms_x  <= msr_x;
        ms_y  <= msr_y;
        msr_x <= 0;
        msr_y <= 0;
      end
    end

    // ms_stb <= ps2_mouse[24];
    // if (ms_stb ^ ps2_mouse[24]) begin
    //   msr_x <= 8'd0 - ps2_mouse[15:8];
    //   msr_y <= ps2_mouse[23:16];
    // end

    if (joy_out[1]) begin
      if (~last_gp[1]) begin
        // Rising edge of joy_out
        turbo_counter <= turbo_counter + 1;
      end

      joy_port <= 0;
      if (status[27:26] != 2'b11) begin
        joy_latch <= 0;
        if (~last_gp[1]) high_buttons <= ~high_buttons && button6_enable;
      end
    end
	else if (joy_out[0] && ~last_gp[0] && (turbo_tap_enable | status[27]) && (status[27:26] != 2'b11)) begin	// suppress if XE-1AP
      joy_port <= joy_port + 3'd1;
    end
  end

  wire [1:0] joy_out;
  wire [3:0] joy_in = (mb128_ena & mb128_Active) ? mb128_Data : joy_latch;

  /////////////////////////  BACKUP RAM SAVE/LOAD  /////////////////////////////

  wire [15:0] mb128_dout;
  // wire mb128_dirty;
  wire mb128_ena = mb128_enable;
  wire mb128_Active;
  wire [3:0] mb128_Data;


  wire [10:0] bram_addr;
  wire [7:0] bram_data;
  wire [7:0] bram_q;
  wire bram_wr;

  // wire format = status[12];
  reg [3:0] bram_init_counter = 0;
  reg [15:0] defval[4] = '{16'h5548, 16'h4D42, 16'h8800, 16'h8010};  //{ HUBM,0x00881080 };

  wire bk_int = !sd_lba[15:2];
  wire [15:0] bk_int_dout;

  assign sd_buff_din = bk_int ? bk_int_dout : mb128_dout;

  dpram_difclk #(11, 8, 10, 16) backram (
      .clock0(clk_sys_42_95),
      .address_a(bram_addr),
      .data_a(bram_data),
      .wren_a(bram_wr),
      .q_a(bram_q),

      .clock1(clk_sys_42_95),
      .address_b(bram_init_counter[3] ? {sd_lba[1:0], sd_buff_addr} : bram_init_counter[2:1]),
      .data_b(bram_init_counter[3] ? sd_buff_dout : defval[bram_init_counter[2:1]]),
      .wren_b(bram_init_counter[3] ? bk_int & sd_wr : 1'b1),

      .q_b(bk_int_dout)
  );

  always @(posedge clk_sys_42_95) begin
    if (pll_core_locked && bram_init_counter != 4'hF) begin
      // Initialize memory card
      bram_init_counter <= bram_init_counter + 4'd1;
    end
  end

  wire downloading = cart_download;
  reg old_downloading = 0;

   /////////////////////////  P/RAM SAVE/LOAD  /////////////////////////////
  



endmodule


