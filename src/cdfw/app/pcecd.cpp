
/*
	PCECD Controller code

	We have seperated the CUE controller and the PCECD interface files so then the CUE controller can be used on other devices



*/
#include "fileio.h"
#include "cue.h"

static int need_reset=0;
static uint8_t has_command = 0;
int apf_data_already_streamed = 0;

uint32_t poll_timer = 0;
	
void pcecd_poll()
{
	static uint8_t last_req = 0;
	static uint8_t adj = 0;



	if (!poll_timer) poll_timer = RISCGetTimer2(13);

	if (need_reset) {
		mainprintf("reset start");
		need_reset = 0;
		cue.Unload();
		
		mainprintf("reset 1");
		cue.Reset();
		
		mainprintf("reset 2");
		poll_timer = 0;
		pcecd_set_image(DATASLOT_BRAM(0), DATASLOT_BRAM(1));
		
		mainprintf("reset 3");
	}
	// mainprintf("milli seconds %0.8x \r\n", poll_timer);
	if (RISCCheckTimer2(poll_timer))
	{
		// mainprintf("milli seconds done %0.8x \r\n", poll_timer);
	    // ResetTimer2();
		if ((!cue.latency) && (cue.state == CD_STATE_READ)) {
			poll_timer += 16;				// 16.0ms between frames if reading data */
		} else {
			poll_timer += (13 + ((adj == 3) ? 1 : 0));	// 13.33ms otherwise (including latency counts) */
			if (adj > 3) adj = 3;
			if (--adj <= 0) adj = 3;
		}
		if (cue.has_status && !cue.latency) {
			cue.SendStatus(cue.GetStatus());
			cue.has_status = 0;
		}
		else if (cue.data_req) {
			cue.SendDataRequest();
			cue.data_req = false;
		}

		cue.Update();
		
	}

	uint8_t req = HPS_spi_uio_cmd_cont(UIO_CD_GET);

	if (req != last_req)
	{
		last_req = req;
		uint16_t data_in[7];
		data_in[0] = spi_w(0);
		data_in[1] = spi_w(0);
		data_in[2] = spi_w(0);
		data_in[3] = spi_w(0);
		data_in[4] = spi_w(0);
		data_in[5] = spi_w(0);
		data_in[6] = spi_w(0);
		HPS_DisableIO();

		switch (data_in[6] & 0xFF)
		{
		case 0:
			cue.SetCommand((uint8_t*)data_in);
			cue.CommandExec();
			has_command = 1;
			break;

		case 1:
			//TODO: process data
			cue.SendStatus(0);
			break;

		case 2:
			cue.can_read_next = true;
			break;

		default:
			need_reset = 1;
			if (apf_data_already_streamed) file_error_code = 5;
			break;
		}
	}
	else {
		HPS_DisableIO();
	}



}

void pcecd_reset() {
	need_reset = 1;
	poll_timer = 0;
}

static void notify_mount(int load)
{
	// HPS_spi_uio_cmd16(UIO_SET_SDINFO, load);
	HPS_spi_uio_cmd8(UIO_SET_SDSTAT, load);
}

int pcecd_using_cd()
{
	return cue.loaded;
}

void pcecd_set_image(int dataslot, int size)
{

	// cue.Unload();
	// cue.Reset();
	cue.state = CD_STATE_NODISC;
	uint16_t temp = cue.Load(101);
	if (temp == 1)
	{
		cue.state = cue.loaded ? CD_STATE_IDLE : CD_STATE_NODISC;
		cue.latency = 10;
		cue.SendData = pcecd_send_data;
		notify_mount(1);
	}
	else
	{
		cue.Unload();
		notify_mount(0);
		// error_osd_displaying = 1;
		osd_display_error_dataslot(temp);
		cue.state = CD_STATE_NODISC;
	}
	need_reset = 0;
}

int pcecd_send_data(uint8_t* buf, int len, uint8_t index) {
	HPS_io_file_tx_data(buf, len, UIO_CD_DATA);
	return 1;
}
