PC Engine CD 0.2.0 ALPHA for Analogue Pocket
============================================

This is a pre-release of the 0.2.0 version, which will have some more upgrades to the
MPU core code. Nothing in the PCE core has been changed that might be causing bugs in the 
runtime. So please keep that in mind.

Ported from the core originally developed by [Gregory
Estrade](https://github.com/Torlus/FPGAPCE) and heavily modified by
[@srg320](https://github.com/srg320) and
[@greyrogue](https://github.com/greyrogue). Then this was ported over to the
Pocket by [Agg233](https://github.com/agg23/openfpga-pcengine) . Core icon based
on TG-16 icon by [spiritualized1997](https://github.com/spiritualized1997) and
many fixes by [@dshadoff](https://github.com/dshadoff). The port is based from
the Mister system and the latest upstream available at
https://github.com/MiSTer-devel/TurboGrafx16_MiSTer"

Please report any issues encountered to this repo. Most likely any problems are
a result of my port, not the original core. Issues will be upstreamed as
necessary.

There is still a lot to be done in this core as there is still a lot to do. It 
was always more of a display core on what the pocket can do.

PLEASE NOTE THAT CHD FILES WILL NOT BE SUPPORTED. Many times I have ben asked. But I have always said that 
the MPU is only a small processer with about 64K of ram, which Im already using 
90% of that for this core. CHD uses a lot of compression that would slow down the 
processor and will cause a lot of overhead. 

What will make this PCECD 0.2.0 ALPHA completed Release?
--------------------------------------------------------

-	Get 99 tracks with Multi file BIN/CUE Files  with the new APF framework

- 	Remove the need for the requirement of JSON Files. (Yes it is now doable I just 
	need to do all the checks in the core)

- 	Get Video based games like Sherlock Homes corrected due to the data and CDROM timing.

What is left after this build?
------------------------------

-	Find those pesky bugs that cause the core to crash ingame!!!

-	More sync issues with the CD Audio with some games.

-	Please note that this was a show of concept that the Pocket can do CDROM based cores. So 
	I hope others might take over on this core to inprove on it.


Change log from 0.1.7:
----------------------

-	Massive amount of work on getting the audio and data to the CORE from the MPU 
	cpu in a reliable way.
	
-	Made the MPU get data from the APF bus without having two lots of BRAM. This currently 
	Pasues the Instruction side of the MPU from 1 to 3 Clock cycles with duel ported BRAM 
	modules. Can be used with the Amiga core???

-   Correction of the timing in the CD core so the correct Minutes, seconds and 
	Frames are sent.

-   Fifo checking when sending data to the core which was causing audio skipping.

-   Removed the Processing delay as an interupt is now used on the MPU.

-   Started on the process of getting seperate BIN files to be loaded (Next release 
	will have this done). Thus removing the 26 Track limit on Seperate BIN files.

-   Autoupdaters: Have removed the "bios_1_0_usa.pce" bios requirement due to there 
	not being such a bios.

Known Bugs:
-----------

-	At this moment PopfulMail Still has a intro bug in it.

-	Have not tested games that crash halfway in. Maybe some testers can confirm this 
	for me later on

Change log from 0.1.6:
----------------------

-   OSD menu for information of the SD activity and if an error on json loading
    happens

-   Audio Timer (Audio Delay) timer in the interaction for changing the timing
    of the CD access - mostly used for audio syncing.

-   Tested on both OS1.1 Beta 7 (Jan.11 2023) and OS1.1 Beta 8 (To be released)
    for delay in APF file access

-   Two timers for software interrupts. One for CD access delays and the other
    used for OSD updates.

-   Autoupdaters: Have added the extra function for adding more BIOS’s for users
    to be downloaded

Notices For running this core
-----------------------------

-   Please do read all of these as there is some special things that need to
    happen to get the best experance with this core

-   Your SDcard needs to be formated as an exFAT as we have found that FAT32
    formated SDcards have a lot of lag on them due to a bug in the APF
    firmware.(Analogue knows about this bug and is fixing this). Also having a
    cluster size of 32K seams to help as well for access times.

-   With Multi-BIN files you can only have 27 tracks as a maximum limit due to
    the limited data slots the APF can handle (You can use single CUE/BIN Isos
    to be able to get to the 99 tracks tho)

-   Only Cue/BIN ISO's can be used. CHR Iso's cannot be used as the compression
    is too much for the MPU I have designed (74mhz and 64Kbytes of ram is not
    enough for this).

-   Auto Updaters are your friends.
    [@mattpannella](https://github.com/mattpannella/pocket-updater-utility),
    [@Monkeymad2](https://github.com/neil-morrison44/pocket-sync)
    [RetroDriven](https://github.com/RetroDriven/Pocket_Updater) have worked on
    setting up the JSON's for the CUE/BIN files for you. So please send them the
    love needed for this.

-   The only missing part in this core is the SuperGrafx chip and the M128
    memory due to chip size. Will work later on this next for this core to
    complete it.

-   Audio skipping has been found and this could be a firmware issue. An Audio
    delay option has been added to change how long it takes to send the audio to
    the core. (Smaller Number == faster time sending data/Audio)

-   CD Debugging has the track, Minute and Second timers as well as a delay
    counter when a delay happens from the APF framework. This will most likely
    show that the APF or the SDcard is having an issue getting the data and then
    sending it. Default is 954000 clock cycles.

-   you will need to check that the Cue files are being generated correctly for
    timing and popping sounds. the Audio Delay can also help on this.

-   Repeating of some tracks might need to be looked at more as this could be a
    end of file issue with the APF. Have added a error menu that stays on for 2
    seconds if this happens.

Installation and Usage
----------------------

### Easy mode setup

I highly recommend the updater tools with JSON Generations are done by
[@mattpannella](https://github.com/mattpannella/pocket-updater-utility) and
[@Monkeymad2](https://github.com/neil-morrison44/pocket-sync). Im sure that
[RetroDriven](https://github.com/RetroDriven/Pocket_Updater) will be updated
soon.

### Manual mode setup

Download the core by clicking Releases on the right side of this page, then
download the `mazamars312.*.zip` file from the latest release.

To install the core, copy the `Assets`, `Cores`, and `Platform` folders over to
the root of your SD card. Please note that Finder on macOS automatically
*replaces* folders, rather than merging them like Windows does, so you have to
manually merge the folders.

Make sure you have the HuCard bios for the CDRomII in the
\Assets\pcecd\common folder

The CUE/BIN Files are also stored in the \Assets\pcecd\common folders and it is
recommended to have them in their own folders

Then you need to setup the JSON files for the pocket to know which CUE/BIN files
are to be used

### JSON Manual mode creation

in the assets\pcecd\Mazamars312.PC Engine CD folder there a image_template.json
file. depending on how many BIN files files is how many dataslots you use and
folder location.

-   "data_path": "image/", is the folder location in the
    \Assets\pcecd\common folder that you are pointing too

-   DataSlot 100 is always the CUE file

-   DataSlot 101-127 is for each BIN file. If you only have a single BIN (with
    MultiTracks in it) then you only put this bin file in Dataslot 101 and
    delete the rest of the assending slots)

If you do have any issues loading images, you can in the Analouge menu turn on
the file debugging (Tools\Developer\Debug Logging) then try to load the JSON.

Once the error happens, you can then go into the SDCARD then to the folders
\system\logs\Mazamars312.pcecd**date_time** to see what happened in the loading
process. this can help to see if you have the file names incorrect.

Make sure you then turn off the debugging once fixed as it will slow down access
and fill up your SDCARD with logs.

An Error code will come up advising which Data slot is causing the issue.

Features
--------

### Dock Support

Core supports four players/controllers via the Analogue Dock. To enable four
player mode, turn on `Use Turbo Tap` setting.

### 6 button controller

Some games support a 6 button controller. For those games, enable the `Use 6
Button Ctrl` option in `Core Settings`. Please note that this option can break
games that don't support the 6 button controller, so turn it off if you're not
using it.

### Controller Turbo

Like the original PC Engine controllers, this core supports multiple turbo
modes. Adjust the `I` and `II` button turbo modes, and use the `X` and `Y`
buttons (by default) as your turbo buttons. Note that the original PCE
controllers had the turbo on the `I` and `II` buttons directly, rather than
having separate buttons, but since the Pocket has more than just two, we use
them for the turbo.

### Video Modes

The PC Engine is unique in that it can arbitrarily decide what resolution to
display at. The Pocket is more limited, requiring fixed resolutions at all
times. I've tried to compromise and cover the most common resolutions output by
the PCE, but some are better supported than others. You should see the video
centered on the screen with surrounding black bars on some resolutions, but the
aspect ratios should be correct.

### Video Options

-   `Extra Sprites` - Allows extra sprites to be displayed on each line. Will
    decrease flickering in some games

-   `Raw RGB Color` - Use the raw RGB color palette output by the HUC6260. If
    disabled, will use the composite color palette

### Audio Options

The core can be quiet in some games, so there are options to boost the master
audio (`Master Audio Boost`) and ADPCM channels (`PCM Audio Boost`) And CDROM
Channels ("CD Audio Boots").

### Memory Cards

Instead of sharing a memory card (as you would in real life), each game gets its
own save file and therefore memory card. Some games don't have the ability to
initialize a memory card, so each newly created save file is pre-initialized for
use.

 

### What is not done

-   The SFX Duel VDP’s - This is a size of the FPGA causing this and a re-write
    of the VDP would need to be done and memory access to one of the PSRAM’s

-   Able to change the H and V sync locations for some games that use other
    screen locations

-   More audio sync via an internal timer - this would help in this, but right
    now ill need to re-write the whole MPU software

-   Add more ram to the MPU for other functions

-   Clean up the track, minute and second display for both the OSD and the
    playback of audio to the core.

-   If able to, add the M128 Memory option to the core.

-   Fix up bugs for other games that are not displaying correctly.

-   Better support for single BIN files

Licensing
---------

All source included in this project from me or the [MiSTer
project](https://github.com/MiSTer-devel/TurboGrafx16_MiSTer) is licensed as
GPLv2, unless otherwise noted. The original source for
[FPGAPCE](https://github.com/Torlus/FPGAPCE), the project this core is based off
of, is [public domain](https://twitter.com/Torlus/status/1582663978068893696).
The contents of the public domain tweet are reproduced here:

>   Indeed. The main reason why I haven't provided a license is that I didn't
>   know how to deal with the different licenses attached to parts of the cores.
>   Anyway, consider *my own* source code as public domain, i.e do what you want
>   with it, for any use you want. (1/2)

[Additionally, he wrote](https://twitter.com/Torlus/status/1582664299973341184):

>   If stated otherwise in the comments at the beginning of a given source file,
>   the license attached prevails. That applies to my FPGAPCE project
>   (https://github.com/Torlus/FPGAPCE).
