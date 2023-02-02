
/*
	PCECD Controller code

	We have seperated the CUE controller and the PCECD interface files so then the CUE controller can be used on other devices



*/
#include "fileio.h"
#include "cue.h"

static int need_reset=0;
static uint8_t has_command = 0;

void pcecd_poll()
{
	static uint32_t poll_timer = 0;
	static uint8_t last_req = 0;
	static uint8_t adj = 0;
	if (!poll_timer) poll_timer = RISCGetTimer(13);

	if (RISCCheckTimer(poll_timer))
	{
	  ResetTimer2();
		if ((!cue.latency) && (cue.state == CD_STATE_READ)) {
			poll_timer = 16;				// 16.0ms between frames if reading data */
		} else {
			poll_timer = 13 + ((adj == 3) ? 1 : 0);	// 13.33ms otherwise (including latency counts) */
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
			// mainprintf("\033[31;1;4mcase 0\033[0m\r\n");
			cue.SetCommand((uint8_t*)data_in);
			cue.CommandExec();
			has_command = 1;
			break;

		case 1:
			//TODO: process data
			// mainprintf("\033[31;1;4mcase 1\033[0m\r\n");
			cue.SendStatus(0);
			break;

		case 2:
			// mainprintf("\033[31;1;4mcase 2\033[0m\r\n");
			cue.can_read_next = true;
			break;

		default:
		// mainprintf("\033[31;1;4mcase reset\033[0m\r\n");
			need_reset = 1;
			break;
		}
	}
	else
		HPS_DisableIO();

	if (need_reset) {
		need_reset = 0;
		// cue.Reset();
		poll_timer = 0;
		pcecd_set_image(DATASLOT_BRAM(8), DATASLOT_BRAM(9));
	}

}

void pcecd_reset() {
	need_reset = 1;
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

	cue.Unload();
	cue.Reset();
	cue.state = CD_STATE_NODISC;

	if (cue.Load(dataslot) > 0)
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
		cue.state = CD_STATE_NODISC;
	}
}

int pcecd_send_data(uint8_t* buf, int len, uint8_t index) {
	HPS_io_file_tx_data(buf, len, UIO_CD_DATA);
	return 1;
}
