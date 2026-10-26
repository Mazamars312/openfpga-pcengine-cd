// Host tests for the CUE loader. cue.cpp is compiled into this file, so its
// static helpers can be called, and runs against a fake APF: a 16K scratch
// RAM and a list of files standing in for the SD card.

#include <map>
#include <new>
#include <string>
#include <vector>

#define private public // to read the TOC
#include "../../drivers/cue/cue.cpp"
#undef private

/* ---------------------------------------------------------------- fake APF */

unsigned char fake_scratch[16384];
uint32_t fake_dataslot_bram[2];
dataslot_type aft;
bool file_error_enabled;
int file_error_code;
int apf_data_already_streamed;

static std::string cue_path, cue_text;
static std::map<std::string, uint32_t> sd_card;	// path -> size
static std::vector<std::string> opens;			// every OpenFileNameDataSlot path
static int cue_reads;
static bool overflow;	// something the real hardware could not take

// The split dataslot_pathname_file_decode() in apf.cpp does.
static void split_path(dataslot_type *a, const char *p)
{
	const char *name = strrchr(p, '/') + 1;
	memset(a->aft_filepath, 0, sizeof(a->aft_filepath));
	memset(a->aft_filename, 0, sizeof(a->aft_filename));
	memcpy(a->aft_filepath, p, name - p);
	strcpy(a->aft_filename, name);
}

int GetFileNameDataSlot(dataslot_type *a)
{
	split_path(a, cue_path.c_str());
	return 0;
}

int OpenFileNameDataSlot(dataslot_type *a, const char *s)
{
	opens.push_back(s);
	if (strlen(s) > 254) overflow = true; // apf.cpp copies paths into 255 byte buffers
	split_path(a, s);
	if (!sd_card.count(s)) {
		file_error_enabled = 1;
		file_error_code = 2;
		return 1;
	}
	a->aft_op_filesize = sd_card[s];
	return 0;
}

uint32_t dataslot_read(uint16_t slot, uint32_t address, uint32_t offset, uint32_t length)
{
	uint32_t at = address - APF_SCRACH_ADDRESS_OFFSET;
	cue_reads++;
	if ((slot != 100) || (at + length > sizeof(fake_scratch)) || (offset + length > cue_text.size())) {
		overflow = true;
		return 1;
	}
	memcpy(fake_scratch + at, cue_text.data() + offset, length);
	return 0;
}

void clearscrachram(uint32_t offset, uint32_t size) { memset(fake_scratch + offset, 0, size); }
uint32_t RISCFileSeek(const char *, uint32_t, uint32_t, uint32_t, dataslot_type *) { return 1; }
uint32_t RISCFileReadAdv(int, uint8_t, uint8_t, uint8_t, uint32_t) { return 1; }
void HPS_DisableIO() {}
uint16_t spi_w(uint16_t) { return 0; }
uint16_t HPS_spi_uio_cmd_cont(uint16_t) { return 0; }
void osd_display_timing(uint8_t, uint8_t, uint8_t) {}
int mainprintf(const char *, ...) { return 0; }
int get_cd_seek_ms(int, int) { return 0; }

/* ----------------------------------------------------------------- helpers */

static int checks, failures;
#define CHECK(c) check((c), #c, __LINE__)
static void check(bool ok, const char *what, int line)
{
	checks++;
	if (!ok) {
		failures++;
		fprintf(stderr, "test_cue.cpp:%d: failed: %s\n", line, what);
	}
}

static const std::string common = "/Assets/pcecd/common/";
static const std::string game = "De-Ja (Japan)";

static std::string track_file(int t)
{
	char s[32];
	snprintf(s, sizeof(s), " (Track %02d).bin", t);
	return game + s;
}

static uint32_t track_size(int t)
{
	return 2352 * (300 + 37 * t);
}

// Redump-style CUE with one bin per track: track 2 and the last are data.
static std::string redump_cue(const std::string &prefix, int n)
{
	std::string cue;
	for (int t = 1; t <= n; t++) {
		bool data = (t == 2) || ((t == n) && (n > 2));
		char s[64];
		snprintf(s, sizeof(s), "  TRACK %02d %s\r\n", t, data ? "MODE1/2352" : "AUDIO");
		cue += "FILE \"" + prefix + track_file(t) + "\" BINARY\r\n" + s;
		if (t > 1) cue += "    INDEX 00 00:00:00\r\n";
		cue += (t == 1) ? "    INDEX 01 00:00:00\r\n" : data ? "    INDEX 01 00:03:00\r\n" : "    INDEX 01 00:02:00\r\n";
	}
	return cue;
}

static std::vector<std::string> track_paths(const std::string &dir, int n)
{
	std::vector<std::string> paths;
	for (int t = 1; t <= n; t++) paths.push_back(dir + track_file(t));
	return paths;
}

static void put_bins(const std::string &dir, int n)
{
	for (int t = 1; t <= n; t++) sd_card[dir + track_file(t)] = track_size(t);
}

struct result
{
	uint16_t ret;
	toc_t toc;
	int error;
	std::string shown; // the path the file error OSD shows
};

// Loads a CUE the way pcecd_set_image() does, on a freshly booted MPU.
static result load(const std::string &path, const std::string &text, int error = 0)
{
	cue_path = path;
	cue_text = text;
	fake_dataslot_bram[1] = text.size();
	opens.clear();
	cue_reads = 0;
	overflow = false;
	file_error_code = error;
	file_error_enabled = error != 0;
	memset((void *)&cue, 0, sizeof(cue));
	new (&cue) cue_t();

	result r;
	r.ret = cue.Load(101);
	r.toc = cue.toc;
	r.error = file_error_code;
	r.shown = std::string(aft.aft_filepath) + aft.aft_filename;
	CHECK(!overflow);
	return r;
}

static bool same_toc(const toc_t &a, const toc_t &b)
{
	if ((a.last != b.last) || (a.end != b.end)) return false;
	for (int i = 0; i < a.last; i++) {
		const cd_track_t &x = a.tracks[i], &y = b.tracks[i];
		if ((x.start != y.start) || (x.end != y.end) || (x.offset != y.offset) || (x.size != y.size) || (x.opened != y.opened) ||
			(x.type != y.type) || (x.sector_size != y.sector_size) || (x.dataslot != y.dataslot) || strcmp(x.name, y.name))
			return false;
	}
	return true;
}

/* ------------------------------------------------------------------- tests */

static std::string resolved(const char *dir, const char *file)
{
	strcpy(cue_dir, dir);
	return resolve(file) ? path : "<error>";
}

static void test_resolve()
{
	const char *cues = "/Assets/pcecd/common/_Cues/Japan/";
	CHECK(resolved(cues, "../../_Games/052 De-Ja/x.bin") == "/Assets/pcecd/common/_Games/052 De-Ja/x.bin");
	CHECK(resolved(cues, "..\\..\\_Games\\052 De-Ja\\x.bin") == "/Assets/pcecd/common/_Games/052 De-Ja/x.bin");
	CHECK(resolved("/a/b/", "x.bin") == "/a/b/x.bin");
	CHECK(resolved("/a/b/", "./sub//./x.bin") == "/a/b/sub/x.bin");
	CHECK(resolved("/a/b/", "sub/../x.bin") == "/a/b/x.bin");
	CHECK(resolved("/a/b/", "../../x.bin") == "/x.bin");
	CHECK(resolved("/a/b/", "../../../x.bin") == "<error>");	// above the SD card root
	CHECK(resolved("/a/b/", "/x/y.bin") == "/a/b/x/y.bin");		// always relative to the CUE

	std::string dir = "/" + std::string(100, 'd') + "/";
	CHECK(resolved(dir.c_str(), std::string(152, 'f').c_str()).size() == 254);
	CHECK(resolved(dir.c_str(), std::string(153, 'f').c_str()) == "<error>");

	CHECK(!strcmp(file_name("../a\\b/x (Track 01).bin"), "x (Track 01).bin"));
	CHECK(!strcmp(file_name("x.bin"), "x.bin"));
}

// The TOC firmware 0.2.3 builds for a Redump CUE with its bins next to it.
static void test_plain_cue()
{
	const std::string dir = common + "De-Ja (Japan)/";
	sd_card.clear();
	put_bins(dir, 5);
	result r = load(dir + game + ".cue", redump_cue("", 5));
	CHECK(r.ret == 1);
	CHECK(r.error == 0);
	CHECK(opens == track_paths(dir, 5));
	CHECK(r.toc.last == 5);
	CHECK(r.toc.end == 2055);
	const int starts[] = {0, 562, 861, 1272, 1795}, ends[] = {337, 711, 1122, 1570, 2055};
	for (int i = 0; i < 5; i++) {
		CHECK(r.toc.tracks[i].start == starts[i]);
		CHECK(r.toc.tracks[i].end == ends[i]);
		CHECK(r.toc.tracks[i].size == (int)track_size(i + 1));
		CHECK(r.toc.tracks[i].name == track_file(i + 1));
	}
}

// One unquoted bin holding every track, with a PREGAP: 0.2.3's TOC.
static void test_single_bin()
{
	const std::string dir = common + "Gate/";
	sd_card.clear();
	sd_card[dir + "gate.bin"] = 2352 * 75 * 60 * 20;
	result r = load(dir + "gate.cue",
		"FILE gate.bin BINARY\n"
		"  TRACK 01 MODE1/2352\n    INDEX 01 00:00:00\n"
		"  TRACK 02 AUDIO\n    PREGAP 00:02:00\n    INDEX 01 10:00:00\n"
		"  TRACK 03 AUDIO\n    INDEX 00 12:00:00\n    INDEX 01 12:02:00\n"
		"  TRACK 04 MODE1/2352\n    INDEX 00 15:00:00\n    INDEX 01 15:03:00\n");
	CHECK(opens == std::vector<std::string>({dir + "gate.bin"}));
	CHECK(r.toc.last == 4);
	CHECK(r.toc.end == 90000);
	const int starts[] = {0, 45150, 54300, 67875}, ends[] = {45150, 54150, 67650, 90000};
	const int offsets[] = {0, 106192800, 127713600, 159642000};
	for (int i = 0; i < 4; i++) {
		CHECK(r.toc.tracks[i].start == starts[i]);
		CHECK(r.toc.tracks[i].end == ends[i]);
		CHECK(r.toc.tracks[i].offset == offsets[i]);
		CHECK(!strcmp(r.toc.tracks[i].name, "gate.bin"));
	}
}

// CUEs in one folder, bins in another: same TOC as with the bins next to the CUE.
static void test_relative_paths()
{
	const std::string beside = common + "De-Ja (Japan)/", cues = common + "_Cues/Japan/", games = common + "_Games/052 De-Ja/";
	sd_card.clear();
	put_bins(beside, 5);
	toc_t plain = load(beside + game + ".cue", redump_cue("", 5)).toc;

	sd_card.clear();
	put_bins(games, 5);
	for (const char *prefix : {"../../_Games/052 De-Ja/", "..\\..\\_Games\\052 De-Ja\\", "./../.././/_Games/052 De-Ja/"}) {
		result r = load(cues + game + ".cue", redump_cue(prefix, 5));
		CHECK(r.ret == 1);
		CHECK(r.error == 0);
		CHECK(opens == track_paths(games, 5));
		CHECK(same_toc(r.toc, plain));
	}
}

// Folders that don't exist: the bin next to the CUE is used, as in 0.2.3,
// and the failed first try leaves no error behind.
static void test_fallback()
{
	const std::string dir = common + "De-Ja (Japan)/";
	sd_card.clear();
	put_bins(dir, 5);
	for (const char *prefix : {"Old Folder/", "C:\\Rips\\", "../../../../../../"}) {
		result r = load(dir + game + ".cue", redump_cue(prefix, 5));
		CHECK(r.ret == 1);
		CHECK(r.error == 0);
		CHECK(r.toc.last == 5);
		CHECK(opens.back() == dir + track_file(5));
	}
	CHECK(load(dir + game + ".cue", redump_cue("Old Folder/", 5), 6).error == 6); // keeps an earlier error
}

static void test_errors()
{
	const std::string cues = common + "_Cues/Japan/", games = common + "_Games/052 De-Ja/";

	// missing bin: both places are tried, then the file error OSD
	sd_card.clear();
	result r = load(cues + game + ".cue", redump_cue("../../_Games/052 De-Ja/", 5));
	CHECK(r.ret == 1);
	CHECK(r.error == 2);
	CHECK(opens == std::vector<std::string>({games + track_file(1), cues + track_file(1)}));

	// bins in two folders: tracks would be reopened in the wrong one
	put_bins(games, 5);
	sd_card[common + "Other/" + track_file(2)] = track_size(2);
	std::string cue = redump_cue("../../_Games/052 De-Ja/", 5);
	cue.replace(cue.find("../../_Games/052 De-Ja/" + track_file(2)), 23, "../../Other/");
	r = load(cues + game + ".cue", cue);
	CHECK(r.error == 2);
	CHECK(opens.size() == 2);

	// FILE line with no closing quote
	r = load(cues + game + ".cue", "FILE \"" + track_file(1) + " BINARY\r\n  TRACK 01 AUDIO\r\n");
	CHECK(r.ret == 0xFFFF);
	CHECK(opens.empty());

	// a file name too long for the TOC's 128 byte track names
	const std::string long_name = std::string(130, 'n') + ".bin";
	sd_card[cues + long_name] = track_size(1);
	r = load(cues + game + ".cue", "FILE \"" + long_name + "\" BINARY\r\n  TRACK 01 MODE1/2352\r\n    INDEX 01 00:00:00\r\n");
	CHECK(r.error == 2);
	CHECK(r.toc.last == 0);

	// a path over 254 characters is never sent to the APF
	const std::string deep = common + std::string(180, 'c') + "/";
	sd_card[deep + track_file(1)] = track_size(1);
	r = load(deep + game + ".cue", redump_cue("s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/s/", 1));
	CHECK(r.ret == 1);
	CHECK(opens == std::vector<std::string>({deep + track_file(1)}));
}

// 99 tracks plus REM lines make a 40K CUE: three chunk reads, and the same TOC
// as the same tracks in a small CUE. Tracks past 99 are ignored.
static void test_big_cue()
{
	const std::string cues = common + "_Cues/Japan/", games = common + "_Games/052 De-Ja/";
	sd_card.clear();
	put_bins(games, 120);
	std::string small = redump_cue("../../_Games/052 De-Ja/", 99), big;
	for (size_t at = 0; at < small.size();) {
		size_t end = small.find('\n', at) + 1;
		big += small.substr(at, end - at);
		if (!small.compare(at, 4, "FILE")) big += "REM " + std::string(300, 'x') + "\r\n";
		at = end;
	}
	CHECK(big.size() > 40000);

	toc_t want = load(cues + game + ".cue", small).toc;
	CHECK(cue_reads == 1);
	result r = load(cues + game + ".cue", big);
	CHECK(cue_reads == (int)(big.size() + 15871) / 15872);
	CHECK(cue_reads >= 3);
	CHECK(r.toc.last == 99);
	CHECK(same_toc(r.toc, want));

	r = load(cues + game + ".cue", redump_cue("../../_Games/052 De-Ja/", 120));
	CHECK(r.toc.last == 99);
	CHECK(opens.size() == 99);
}

// Slide a CUE across the end of the first 15,872 byte chunk one byte at a
// time, so every line of it gets split between two reads.
static void test_chunk_boundary()
{
	const std::string cues = common + "_Cues/Japan/", games = common + "_Games/052 De-Ja/";
	sd_card.clear();
	put_bins(games, 5);
	std::string cue = redump_cue("../../_Games/052 De-Ja/", 5);
	toc_t want = load(cues + game + ".cue", cue).toc;
	const size_t chunk = 16384 - 0x200;
	int bad = 0;
	for (size_t pad = chunk - cue.size() - 2; pad <= chunk + 2; pad++) {
		std::string text;
		while (pad - text.size() > 105) text += "REM " + std::string(94, 'p') + "\r\n";
		text += "REM " + std::string(pad - text.size() - 6, 'q') + "\r\n" + cue;
		result r = load(cues + game + ".cue", text);
		if ((r.ret != 1) || !same_toc(r.toc, want)) bad++;
	}
	CHECK(bad == 0);
}

int main()
{
	test_resolve();
	test_plain_cue();
	test_single_bin();
	test_relative_paths();
	test_fallback();
	test_errors();
	test_big_cue();
	test_chunk_boundary();
	printf("%s: %d of %d checks failed\n", failures ? "FAILED" : "OK", failures, checks);
	return failures != 0;
}
