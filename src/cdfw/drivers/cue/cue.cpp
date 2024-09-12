
#include "cue.h"

int get_cd_seek_ms(int start_sector, int target_sector);

#define CD_DATA_IO_INDEX 2
uint32_t foo1;
cue_t cue;

cue_t::cue_t() {
	latency = 0;
	audiodelay = 0;
	loaded = 0;
	index = 0;
	lba = 0;
	scanOffset = 0;
	isData = 1;
	state = CD_STATE_NODISC;
	audioLength = 0;
	audioOffset = 0;
	SendData = NULL;
	has_status = 0;
	data_req = false;
	can_read_next = false;
	CDDAStart = 0;
	CDDAEnd = 0;
	CDDAMode = CD_CDDAMODE_SILENT;
	region = 0;

	stat = 0x0000;

}

const uint32_t memory_offset_cue = 0x00000200;

static int sgets(char *out, int sz, int cnt)
{
	*out = 0;
	do
	{
		char *instr = (char*)foo1;
		int cnt = 0;
		while (*instr && *instr != 10)
		{
			if (*instr == 13)
			{
				instr++;
				foo1++;
				continue;
			}
			if (cnt < sz - 1)
			{
				out[cnt++] = *instr;
				out[cnt] = 0;
			}
			instr++;
			foo1++;
		}
		if(*instr == 10) {
			instr++;
			foo1++;
		}
		if(*instr == 00) {
			return *out;
		}
	}
	while ((foo1 - (MAIN_SCRACH_ADDRESS_OFFSET + memory_offset_cue)) >= cnt);
	return *out;
}

	// dataslot_type aft;
uint16_t cue_t::LoadCUE(int dataslot) {
	static char line[128];
	char fline[255];
	char s[255];
	// static char fname[80];
	char *ptr, *lptr;
	int hdr = 0;
	int toc_size;
	// First we get the Cue File
	aft.aft_dataslot = 100;
	GetFileNameDataSlot(&aft);
	bool File_processesd = 0;
	bool File_processesd_done = true;
	toc_size = DATASLOT_BRAM(1); // This is where the Cue file will be The BINs are in the 101 dataslot
	if (toc_size == 0) return 100;
	clearscrachram(0,16384);
	foo1 = MAIN_SCRACH_ADDRESS_OFFSET + memory_offset_cue;
	dataslot_read(100, APF_SCRACH_ADDRESS_OFFSET + memory_offset_cue, 0, toc_size);
	aft.aft_dataslot = dataslot;
	int mm, ss, bb, pregap = 0;
	while ((sgets(line, sizeof(line), toc_size)))
	{
		lptr = line;
		// mainprintf("%s\r\n", line);
		while (*lptr == 0x20) lptr++;

		/* decode FILE commands */
		if (!(memcmp(lptr, "FILE", 4)))
		{
			int i = 0;
         	int j = 0;
         	int q = 0; // track open and closed quotes
			do { 
				this->toc.tracks[this->toc.last].name[j] = lptr[5+i]; 
				i++;
				j++;
				if ((this->toc.tracks[this->toc.last].name[j-1] == '\\') || (this->toc.tracks[this->toc.last].name[j-1] == '/')) { j = 0; } //strip out path info
				if (this->toc.tracks[this->toc.last].name[j-1] == '"') { j--; q++;} // strip out quotes
        	} while ((lptr[5+i-1] != ' ') || (q == 1));
			sprintf(s,"%s%s", aft.aft_filepath, this->toc.tracks[this->toc.last].name);
			if (OpenFileNameDataSlot(&aft, s) > 0) {
				return 1;
				}
			// OpenFileNameDataSlot (&aft, s);
			File_processesd = 1;
			if (strstr(lptr, ".wav")){
				hdr = 44;
			};
			pregap = 0;

			// this->toc.tracks[this->toc.last].offset = 0;
			
			

			if (!strstr(lptr, "BINARY") && !strstr(lptr, "MOTOROLA") && !strstr(lptr, "WAVE"))
			{
				return -1;
			}
			
		}

		/* decode TRACK commands */
		else if ((sscanf(lptr, "TRACK %d %*s", &bb)) || (sscanf(lptr, "TRACK %d %*s", &bb)))
		{
			if (bb != (this->toc.last + 1))
			{
				break;
			}
			else
			{
				
  				mainprintf("New Directory %s ||| File name %s ||| more information %0.8x %0.8x\r\n",aft.aft_filepath, aft.aft_filename, aft.aft_op_flags, aft.aft_op_filesize);
				this->toc.tracks[this->toc.last].dataslot = aft.aft_dataslot;
				this->toc.tracks[this->toc.last].size  = aft.aft_op_filesize;
				if (strstr(lptr, "MODE1/2048"))
				{
					this->toc.tracks[this->toc.last].sector_size = 2048;
					this->toc.tracks[this->toc.last].type = 1;
				}
				else if (strstr(lptr, "MODE1/2352"))
				{
					this->toc.tracks[this->toc.last].sector_size = 2352;
					this->toc.tracks[this->toc.last].type = 1;
				}
				else if (strstr(lptr, "AUDIO"))
				{
					this->toc.tracks[this->toc.last].sector_size = 2352;
					this->toc.tracks[this->toc.last].type = 0;
				} else if (strstr(lptr, "CDG"))
				{
					this->toc.tracks[this->toc.last].sector_size = 2448;
					this->toc.tracks[this->toc.last].type = 2;
				}
			}
			
			if (File_processesd) {
				if (this->toc.last >= 1) {
					this->toc.tracks[this->toc.last].opened = true;
					if (this->toc.last == 1) {
						this->toc.tracks[this->toc.last - 1].end = this->toc.tracks[this->toc.last - 1].start + ((this->toc.tracks[this->toc.last - 1].size - this->toc.tracks[this->toc.last - 1].offset) / this->toc.tracks[this->toc.last - 1].sector_size );
						this->toc.end = this->toc.tracks[this->toc.last - 1].end;
						this->toc.tracks[this->toc.last - 1].opened = true;
  						// mainprintf("\033[92;1;1m Done the change 1 %d\033[0m\r\n", this->lba);
					}
				} else {
					this->toc.tracks[this->toc.last].opened = false;
					// mainprintf("\033[92;1;1m Done the change 2 %d\033[0m\r\n", this->lba);
				}
				File_processesd = 0;
			} else {
				
  				// mainprintf("\033[92;1;1m Done the change 3 %d\033[0m\r\n", this->lba);
				strcpy(this->toc.tracks[this->toc.last].name,aft.aft_filename);
				this->toc.tracks[this->toc.last].opened = false;
			}
			
		}

		/* decode PREGAP commands */
		else if (sscanf(lptr, "PREGAP %02d:%02d:%02d", &mm, &ss, &bb) == 3)
		{
			pregap += bb + (ss * 75) + (mm * 60 * 75);
		}
		/* decode INDEX commands */
		else if ((sscanf(lptr, "INDEX 00 %02d:%02d:%02d", &mm, &ss, &bb) == 3) ||
			(sscanf(lptr, "INDEX 0 %02d:%02d:%02d", &mm, &ss, &bb) == 3))
		{
			if (this->toc.last && !this->toc.tracks[this->toc.last - 1].end)
			{
				this->toc.tracks[this->toc.last - 1].end = bb + (ss * 75) + (mm * 60 * 75) + pregap;
			}
		}
		else if ((sscanf(lptr, "INDEX 01 %02d:%02d:%02d", &mm, &ss, &bb) == 3) || (sscanf(lptr, "INDEX 1 %02d:%02d:%02d", &mm, &ss, &bb) == 3))
		{
			if (!this->toc.tracks[this->toc.last].opened)
			{
				this->toc.tracks[this->toc.last].start = bb + (ss * 75) + (mm * 60 * 75) + pregap;
				this->toc.tracks[this->toc.last].offset = (this->toc.tracks[this->toc.last].start * this->toc.tracks[this->toc.last].sector_size) - hdr;
				if (this->toc.last && !this->toc.tracks[this->toc.last - 1].end)
				{
					this->toc.tracks[this->toc.last - 1].end = this->toc.tracks[this->toc.last].start;
				}

				mainprintf("opened %u %d %u %u %u\r\n", pregap, hdr, this->toc.tracks[this->toc.last].start, this->toc.tracks[this->toc.last].end, this->toc.tracks[this->toc.last].offset);
			}
			else
			{
				this->toc.tracks[this->toc.last].start = this->toc.end + pregap;
				this->toc.tracks[this->toc.last].offset = (this->toc.tracks[this->toc.last].start * this->toc.tracks[this->toc.last].sector_size) - hdr;
				this->toc.tracks[this->toc.last].end = (this->toc.tracks[this->toc.last].start + ((this->toc.tracks[this->toc.last].size - hdr + this->toc.tracks[this->toc.last].sector_size - 1)) / this->toc.tracks[this->toc.last].sector_size);

				this->toc.tracks[this->toc.last].start += (bb + (ss * 75) + (mm * 60 * 75));
				this->toc.end = this->toc.tracks[this->toc.last].end;
				mainprintf(" not opened %u %u %u\r\n", this->toc.tracks[this->toc.last].start, this->toc.tracks[this->toc.last].end, this->toc.tracks[this->toc.last].offset);
			}
			this->toc.last++;
			if (this->toc.last == 99) break;
		}
	}

	if (!this->toc.tracks[this->toc.last - 1].opened) {
		this->toc.tracks[this->toc.last - 1].end = this->toc.tracks[this->toc.last - 1].start + ((this->toc.tracks[this->toc.last - 1].size - this->toc.tracks[this->toc.last - 1].offset) / this->toc.tracks[this->toc.last - 1].sector_size );		
		this->toc.end = this->toc.tracks[this->toc.last - 1].end;
		// mainprintf("unopened %u %d \r\n", pregap, hdr);
	} 

	if (this->toc.last && !this->toc.tracks[this->toc.last - 1].end)
	{
		this->toc.end += pregap;
		this->toc.tracks[this->toc.last - 1].end = this->toc.end;
	}
	
	for (int i = 0; i < this->toc.last; i++)
	{
		mainprintf("\x1b[32mPCECD: Track = %u, start = %u, end = %u, offset = %u, sector_size=%d, type = %u, dataslot = %u opend = %u \r\n name = %s \r\n\x1b[0m", i, this->toc.tracks[i].start, this->toc.tracks[i].end, this->toc.tracks[i].offset, this->toc.tracks[i].sector_size, this->toc.tracks[i].type, this->toc.tracks[i].dataslot, this->toc.tracks[i].opened, this->toc.tracks[i].name);
	}
	
	return 1;
}

uint16_t cue_t::Load(int dataslot)
{
	// Unload();
	uint16_t temp = LoadCUE(dataslot);
	if (temp){
		this->loaded = 1;
		return temp;
	} else {
		this->loaded = 0;
		return temp;
	}
return 0;
}

void cue_t::Unload()
{
	if (this->loaded)
	{


		for (int i = 0; i < this->toc.last; i++)
			{
				this->toc.tracks[i].dataslot = 0;
				this->toc.tracks[i].opened = 0;
				this->toc.tracks[i].size = 0;
				this->toc.tracks[i].offset = 0;
				this->toc.tracks[i].pregap = 0;
				this->toc.tracks[i].start = 0;
				this->toc.tracks[i].end = 0;
				this->toc.tracks[i].type = 0;
				this->toc.tracks[i].sector_size = 0;
				// sprintf(this->toc.tracks[i].name,"");
				// int j = 0;
				// do{
					// this->toc.tracks[i].name = NULL;
					// j++;
				// }while(j < 128);
			}
			this->loaded = 0;
			this->toc.end=0;
			this->toc.last=0;
	}
	 mainprintf ("Im Here Father %d \r\n", this->toc.last);
}

void cue_t::Reset() {
	latency = 0;
	audiodelay = 0;
	index = 0;
	lba = 0;
	scanOffset = 0;
	isData = 1;
	state = loaded ? CD_STATE_IDLE : CD_STATE_NODISC;
	audioLength = 0;
	audioOffset = 0;
	has_status = 0;
	data_req = false;
	can_read_next = false;
	CDDAStart = 0;
	CDDAEnd = 0;
	CDDAMode = CD_CDDAMODE_SILENT;
	this->toc.last = 0;
	stat = 0x0000;

}

void cue_t::Update() {
	// mainprintf ("LBA master READING %d \r\n", this->lba);
	if (this->state == CD_STATE_READ)
	{
		if (this->latency > 0)
		{
			this->latency--;
			return;
		}

		if (this->index >= this->toc.last)
		{
			this->state = CD_STATE_IDLE;
			return;
		}

		if (!this->can_read_next)
			return;

		this->can_read_next = false;

		if (this->toc.tracks[this->index].type == 1)
		{
			mainprintf("check they type data \r\n");
			// CD-ROM (Mode 1)
			ReadData();
			apf_data_already_streamed = 1;

		}
		else
		{
			if (this->lba >= this->toc.tracks[this->index].start)
			{
				this->isData = 0x00;
			}

		}

		this->cnt--;

		if (!this->cnt) {
			PendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
			this->state = CD_STATE_IDLE;
		}

		this->lba++;
		if (this->lba >= this->toc.tracks[this->index].end)
		{
			this->index++;

			this->isData = 0x01;
			mainprintf("check they type data 2\r\n");

			if (!this->toc.tracks[this->index].opened) {
				RISCFileSeek(this->toc.tracks[this->index].name, this->lba * 2352 + 16, this->toc.tracks[this->index].sector_size, this->index, &aft);
			}else{
				RISCFileSeek(this->toc.tracks[this->index].name, (this->lba * 2352 - this->toc.tracks[this->index].offset), this->toc.tracks[this->index].sector_size, this->index, &aft);
			}

		}
	}
	else if (this->state == CD_STATE_PLAY)
	{
		if (this->latency > 0)
		{
			this->latency--;
			return;
		}

		if (this->audiodelay > 0)
		{
			this->audiodelay--;
			return;
		}
		
		this->index = GetTrackByLBA(this->lba, &this->toc);

		for (int i = 0; i <= this->CDDAFirst; i++)
		{
			uint32_t temp_LBA;
			temp_LBA = this->lba * this->toc.tracks[this->index].sector_size;
			if (this->toc.tracks[this->index].opened){
				temp_LBA = temp_LBA - this->toc.tracks[this->index].offset;
			}
			
			if (!(this->toc.tracks[this->index].type == 1)) {
				RISCFileSeek(this->toc.tracks[this->index].name, temp_LBA, this->toc.tracks[this->index].sector_size, this->index, &aft);
			}
			
			if (this->toc.tracks[this->index].type == 2)
			{
				ReadCDG();
			} else if (this->toc.tracks[this->index].type == 0)
			{
				ReadCDDA();
			}
			this->lba++;
		}

		this->CDDAFirst = 0;

		if ((this->lba > this->CDDAEnd) || this->toc.tracks[this->index].type || this->index > this->toc.last)
		{
			if (this->CDDAMode == CD_CDDAMODE_LOOP) {
				this->lba = this->CDDAStart;
			}
			else {
				this->state = CD_STATE_IDLE;
			}
			if (this->CDDAMode == CD_CDDAMODE_INTERRUPT) {
				SendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
			}
			//riscprintf("\x1b[32mPCECD: playback reached the end %d\r\n\x1b[0m", this->lba);
		}
	}
	else if (this->state == CD_STATE_PAUSE)
	{
		if (this->latency > 0)
		{
			this->latency--;
			return;
		}
	}

	int lba_test;
	lba_test = this->lba;
	int index_test = GetTrackByLBA(lba_test, &this->toc);
	lba_test = lba_test - this->toc.tracks[index_test].start;

	msf_t msf;
	LBAToMSF(lba_test, &msf);
	osd_display_timing(this->index + 1,  msf.m , msf.s);
}

void cue_t::CommandExec() {
	msf_t msf;
	int new_lba = 0;
	static uint8_t buf[32];
	uint32_t temp_latency;

	memset(buf, 0, 32);
	switch (comm[0]) {
	case CD_COMM_TESTUNIT:
		if (state == CD_STATE_NODISC) {
			CommandError(SENSEKEY_NOT_READY, NSE_NO_DISC, 0, 0);
			SendStatus(MAKE_STATUS(CD_STATUS_CHECK_COND, 0));
		}
		else {
			SendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
		}

		break;

	case CD_COMM_REQUESTSENSE:
		buf[0] = 18;
		buf[1] = 0 | 0x80;

		buf[2] = 0x70;
		buf[4] = sense.key;
		buf[9] = 0x0A;
		buf[14] = sense.asc;
		buf[15] = sense.ascq;
		buf[16] = sense.fru;

		sense.key = sense.asc = sense.ascq = sense.fru = 0;

		if (SendData)
			SendData(buf, 18 + 2, CD_DATA_IO_INDEX);

		SendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));

		break;

	case CD_COMM_GETDIRINFO: {
		int len = 0;
		switch (comm[1]) {
		case 0:
		default:
			buf[0] = 4;
			buf[1] = 0 | 0x80;
			buf[2] = 1;
			buf[3] = BCD(this->toc.last);
			buf[4] = 0;
			buf[5] = 0;
			len = 4 + 2;
			// mainprintf("Command 0 T %u \r\n", (this->toc.last));
			break;
		case 1:
			new_lba = this->toc.end + 150;
			LBAToMSF(new_lba, &msf);

			buf[0] = 4;
			buf[1] = 0 | 0x80;
			buf[2] = BCD(msf.m);
			buf[3] = BCD(msf.s);
			buf[4] = BCD(msf.f);
			// mainprintf("Command 1 M %0.4x S %0.4x F %0.4x  M %0.4x S %0.4x F %0.4x\r\n", buf[2], buf[3], buf[4], msf.m, msf.s, msf.f);
			buf[5] = 0;
			len = 4 + 2;
			break;

		case 2:
			int track = U8(comm[2]);
			new_lba = this->toc.tracks[track - 1].start + 150;
			LBAToMSF(new_lba, &msf);

			buf[0] = 4;
			buf[1] = 0 | 0x80;
			buf[2] = BCD(msf.m);
			buf[3] = BCD(msf.s);
			buf[4] = BCD(msf.f);
			buf[5] = this->toc.tracks[track - 1].type << 2;
			// mainprintf("Command 2 M %0.4x S %0.4x F %0.4x  M %0.4x S %0.4x F %0.4x start %d \r\n", buf[2], buf[3], buf[4], msf.m, msf.s, msf.f, this->toc.tracks[track - 1].start);
			len = 4 + 2;
			break;
		}

		if (SendData && len)
			SendData(buf, len, CD_DATA_IO_INDEX);

		SendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
	}
		break;

	case CD_COMM_READ6: {
		new_lba = ((comm[1] << 16) | (comm[2] << 8) | comm[3]) & 0x1FFFFF;
		int cnt_ = comm[4] ? comm[4] : 256;

		int index = GetTrackByLBA(new_lba, &this->toc);

		this->index = index;

		/* HuVideo streams by fetching 120 sectors at a time, taking advantage of the geometry
		 * of the disc to reduce/eliminate seek time */
		if ((this->lba == new_lba) && (cnt_ == 120))
		{
			this->latency = 0;
		}
		/* Sherlock Holmes streams by fetching 252 sectors at a time, and suffers
		 * from slight pauses at each seek */
		else if ((this->lba == new_lba) && (cnt_ == 252))
		{
			this->latency = 5;
		}
		else if (comm[13] & 0x80) // fast seek (OSD setting)
		{
			this->latency = 0;
		}
		else
		{
			this->latency = (int)(get_cd_seek_ms(this->lba, new_lba)/13);
			this->audiodelay = 0;
		}

		this->lba = new_lba;
		this->cnt = cnt_;


		this->audioOffset = 0;
		this->can_read_next = true;
		this->state = CD_STATE_READ;
	}
		break;

	case CD_COMM_MODESELECT6:
		if (comm[4]) {
			data_req = true;
		}
		else {
			SendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
		}

		break;

	case CD_COMM_SAPSP: {
		switch (comm[9] & 0xc0)
		{
		default:
		case 0x00:
			new_lba = (comm[3] << 16) | (comm[4] << 8) | comm[5];
			break;

		case 0x40:
			MSFToLBA(&new_lba, U8(comm[2]), U8(comm[3]), U8(comm[4]));
			break;

		case 0x80:
		{
			int track = U8(comm[2]);

			if (!track)
				track = 1;
			else if (track > toc.last)
				track = toc.last;
			new_lba = this->toc.tracks[track - 1].start;
		}
		break;
		}

		if (comm[13] & 0x80) // fast seek (OSD setting)
		{
			this->latency = 0;
			this->audiodelay = 0;
		}
		else
		{
			temp_latency = (get_cd_seek_ms(this->lba, new_lba) / 13);
			this->audiodelay = (int)(220 / 13);

			if (temp_latency > this->audiodelay)
				this->latency = temp_latency - this->audiodelay;
			else {
				this->latency = temp_latency;
				this->audiodelay = 0;
			}
		}

		this->lba = new_lba;
		int index = GetTrackByLBA(new_lba, &this->toc);

		this->index = index;

		this->CDDAStart = new_lba;
		this->CDDAEnd = this->toc.end;
		this->CDDAMode = comm[1];
		this->CDDAFirst = 1;

		if (this->CDDAMode == CD_CDDAMODE_SILENT) {
			this->state = CD_STATE_PAUSE;
		}
		else {
			this->state = CD_STATE_PLAY;
		}

		PendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
	}
		break;

	case CD_COMM_SAPEP: {
		switch (comm[9] & 0xc0)
		{
		default:
		case 0x00:
			new_lba = (comm[3] << 16) | (comm[4] << 8) | comm[5];
			break;

		case 0x40:
			MSFToLBA(&new_lba, U8(comm[2]), U8(comm[3]), U8(comm[4]));
			break;

		case 0x80:
		{
			int track = U8(comm[2]);

			// Note that track (imput from PCE) starts numbering at 1
			// but toc.tracks starts numbering at 0
			//
			if (!track)	track = 1;
			new_lba = ((track-1) >= toc.last) ? this->toc.end : (this->toc.tracks[track - 1].start);
		}
		break;
		}

		this->CDDAMode = comm[1];
		this->CDDAEnd = new_lba;

		if (this->CDDAMode == CD_CDDAMODE_SILENT) {
			this->state = CD_STATE_IDLE;
		}
		else {
			this->state = CD_STATE_PLAY;
		}

		if (this->CDDAMode != CD_CDDAMODE_INTERRUPT) {
			SendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
		}

	}
		break;

	case CD_COMM_PAUSE: {
		this->state = CD_STATE_PAUSE;

		SendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
	}
		break;

	case CD_COMM_READSUBQ: {
		int lba_rel = this->lba - this->toc.tracks[this->index].start;

		buf[0] = 0x0A;
		buf[1] = 0 | 0x80;
		buf[2] = this->state == CD_STATE_PAUSE ? 2 : (this->state == CD_STATE_PLAY ? 0 : 3);
		buf[3] = 0;
		buf[4] = BCD(this->index + 1);
		buf[5] = BCD(this->index);

		LBAToMSF(lba_rel, &msf);
		buf[6] = BCD(msf.m);
		buf[7] = BCD(msf.s);
		buf[8] = BCD(msf.f);

		LBAToMSF(this->lba+150, &msf);
		buf[9] = BCD(msf.m);
		buf[10] = BCD(msf.s);
		buf[11] = BCD(msf.f);

		if (SendData)
			SendData(buf, 10 + 2, CD_DATA_IO_INDEX);
		SendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
	}
		break;

	default:
		CommandError(SENSEKEY_ILLEGAL_REQUEST, NSE_INVALID_COMMAND, 0, 0);
		has_status = 0;
		SendStatus(MAKE_STATUS(CD_STATUS_CHECK_COND, 0));
		break;
	}
}

uint16_t cue_t::GetStatus() {
	return stat;
}

int cue_t::SetCommand(uint8_t* buf) {
	memcpy(comm, buf, 14);
	return 0;
}

void cue_t::PendStatus(uint16_t status) {
	stat = status;
	has_status = 1;
}

void cue_t::SendStatus(uint16_t status) {
	HPS_spi_uio_cmd_cont(UIO_CD_SET);
	spi_w(status);
	spi_w(region ? 2 : 0);
	HPS_DisableIO();
}

void cue_t::SendDataRequest() {
	HPS_spi_uio_cmd_cont(UIO_CD_SET);
	spi_w(0);
	spi_w((region ? 2 : 0) | 1);
	HPS_DisableIO();
}

void cue_t::SetRegion(uint8_t rgn) {
	region = rgn;
}

void cue_t::LBAToMSF(int lba, msf_t* msf) {
	msf->m = (lba / 75) / 60;
	msf->s = (lba / 75) % 60;
	msf->f = (lba % 75);
}

void cue_t::MSFToLBA(int* lba, uint8_t m, uint8_t s, uint8_t f) {
	*lba = f + s * 75 + m * 60 * 75 - 150;
}

void cue_t::MSFToLBA(int* lba, msf_t* msf) {
	*lba = msf->f + msf->s * 75 + msf->m * 60 * 75 - 150;
}

int cue_t::GetTrackByLBA(int lba, toc_t* toc) {
	int index = 0;
	while ((toc->tracks[index].end <= lba) && (index < toc->last)) index++;
	return index;
}

void cue_t::ReadData()
{
	if (this->toc.tracks[this->index].type && (this->lba >= 0))
	{	
			int32_t temp_LBA;
			
    		mainprintf ("LBA %d \r\n", this->lba);
			if (this->toc.tracks[this->index].sector_size == 2048)
			{
				if (!this->toc.tracks[this->index].opened) {
					RISCFileSeek(this->toc.tracks[this->index].name, this->lba * 2048, 2048, this->index, &aft);
				}else{
					temp_LBA = (this->lba * 2048) - this->toc.tracks[this->index].offset;
					RISCFileSeek(this->toc.tracks[this->index].name, temp_LBA, 2048, this->index, &aft);
				}
				RISCFileReadAdv(this->toc.tracks[this->index].dataslot,0x00,(0x08 | 0x80),UIO_CD_DATA, this->toc.tracks[this->index].sector_size);
			} else {
				if (!this->toc.tracks[this->index].opened) {
					RISCFileSeek(this->toc.tracks[this->index].name, this->lba * 2352 + 16 , 2352, this->index, &aft);
				}else{
					temp_LBA = (this->lba * 2352) - this->toc.tracks[this->index].offset;
					RISCFileSeek(this->toc.tracks[this->index].name, temp_LBA + 16, 2352, this->index, &aft);
				}
				RISCFileReadAdv(this->toc.tracks[this->index].dataslot,0x00,(0x08 | 0x80),UIO_CD_DATA, this->toc.tracks[this->index].sector_size);
			}
			
	}
}

void cue_t::ReadCDDA()
{
	this->audioLength = 2352;
	this->audioOffset = 0;// 2352;
	RISCFileReadAdv(this->toc.tracks[this->index].dataslot,0x30,0x09,UIO_CD_DATA,2352);
}

void cue_t::ReadCDG()
{
	this->audioLength = 2448;
	this->audioOffset = 0;// 2352;
	RISCFileReadAdv(this->toc.tracks[this->index].dataslot,0x90,(0x09 | 0x80),UIO_CD_DATA,2448);
}

void cue_t::CommandError(uint8_t key, uint8_t asc, uint8_t ascq, uint8_t fru) {
	sense.key = key;
	sense.asc = asc;
	sense.ascq = ascq;
	sense.fru = fru;
}
