PC Engine CD 0.2.3 Beta for Analogue Pocket
============================================

This is a release of the 0.2.3 Beta version, has a requirement of the new Analogue Firmware 2.3 (2024-09-10) for all feactures to work. 

No lower firmware will work with this core and functions added to it.

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
the MPU is only a small processer with about 64K of RAM, which Im already using 
90% of that for this core. CHD uses a lot of compression that would slow down the 
processor and will cause a lot of overhead. 

What will be in the next release?
--------------------------------------------------------

-   Be able to swap CDROM on the menu. I have yet to deside if I need to do a reboot of the whole core, or find out the correct commands to send the core to say a new CDROM    
    is in the core. It is there in the menu and will be fixed soon.

-   (Maybe) CD-G CDROM to be enabled on this core.

-   A new video scalling system to help things look better on the screen. AKA Sherlock homes has a line on the top of the screen and a few other games.

-   New MPU Framework so this and the Amiga cores are more inline with each other for other devs to be able to use.

What is left after this build?
------------------------------

-	Please note that this was a show of concept that the Pocket can do CDROM based cores. So 
	I hope others might take over on this core to improve on it.

Change log from 0.2.0:
---------------------

-   The LARGEST Feacture done is NO MORE JSONS!!! The MPU now requests to load CUE files from the APF Framework. Then it will autoload the bin files from the CUE listing. I have not tested WAV files yet.

-   Multi File games are no longer limited to just 26 files, now we can use 99 bin file tracks!!!

-   Timing of the CDROM data has been corrected so games like Sherlock homes videos work correctly

-   Many games fixes have been fixed due to a ram issue using SRAM (AGG Still needs to use SRAM in his cores thos hehe - Love ya mate)

-   Some Error checking is done in runtime for things like missing files, unable to read a file or over runs. These are coded with a number from 1 - 5 so making issues can be easier to find

-   mpu.bin has been changed to pce_mpu_bios.bin for the MPU operations.


Change log from 0.1.7:
----------------------

-	Massive amount of work on getting the audio and data to the CORE from the MPU 
	cpu in a reliable way.
	
-	Made the MPU get data from the APF bus without having two lots of BRAM. This currently 
	Pauses the Instruction side of the MPU from 1 to 3 Clock cycles with duel ported BRAM 
	modules. This process can be used with the Amiga core maybe???

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

-	Have not tested WAV file access from the cue files yet

-	Some hacked BIN/CUE files do not work or boot up. I have made a info menu that comes up to advise that the core has had an issue

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

-   Only Cue/BIN ISO's can be used. CHR Iso's cannot be used as the compression
    is too much for the MPU I have designed (74mhz and 64Kbytes of ram is not
    enough for this).

-   Auto Updaters are your friends.
    [@mattpannella](https://github.com/mattpannella/pocket-updater-utility),
    [@Monkeymad2](https://github.com/neil-morrison44/pocket-sync)
    [RetroDriven](https://github.com/RetroDriven/Pocket_Updater) have worked on
    setting up the JSON's for the CUE/BIN files for you. So please send them the
    love needed for this. - No longer needed

-   The only missing part in this core is the SuperGrafx chip and the M128
    memory due to chip size.

-   CD Debugging has the track, Minute and Second timers as well as a delay
    counter when a delay happens from the APF framework. This will most likely
    show that the APF or the SDcard is having an issue getting the data and then
    sending it. 

-   There is a OSD for errors that happen, It will adivse if it is having a issue loading files and advise which file it is too.

-   There is a problem with reading a file and will show the eror code from the APF

-   If it trys to load a image that has a weird setup (Game hacks) and crashes it will advise the user there was a problem with the MPU.


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

### JSON Manual mode creation - No longer needed

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

-   If able to, add the M128 Memory option to the core.

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
