# PC Engine CD for Analogue Pocket

This core has been ported from the [MiSTer project](https://github.com/MiSTer-devel/Wiki_MiSTer/wiki). The core was originally developed by [Gregory Estrade](https://github.com/Torlus/FPGAPCE) and heavily modified by [@srg320](https://github.com/srg320) and [@greyrogue](https://github.com/greyrogue) and many fixes by [@dshadoff](https://github.com/dshadoff).  Then this was ported over to the Pocket by [Agg233](https://github.com/agg23/openfpga-pcengine) . Core icon based on TG-16 icon by [spiritualized1997](https://github.com/spiritualized1997). Latest upstream available at https://github.com/MiSTer-devel/TurboGrafx16_MiSTer

Please report any issues encountered to this repo. Most likely any problems are a result of my port, not the original core. Issues will be upstreamed as necessary.

## Inportent Notices For running this core
* Please do read all of these as there is some special things that need to happen to get the best experance with this core
* Your SDcard needs to be formated as an exFAT as we have found that FAT32 formated SDcards have a lot of lag on them due to a bug in the APF firmware.(Analogue knows about this bug and is fixing this)
* With Multi-BIN files you can only have 27 tracks as a maximum limit due to the limited data slots the APF can handle (So make sure you use single CUE/BIN Isos)
* Only Cue/BIN ISO's can be used. CHR Iso's cannot be used as the compression is too much for the MPU I have designed (74mhz and 64Kbytes of ram is not enough for this).
* Auto Updaters are your friends. Both [@mattpannella](https://github.com/mattpannella/pocket-updater-utility) and [@Monkeymad2](https://github.com/neil-morrison44/pocket-sync) have worked on setting up the JSON's for the CUE/BIN files for you. SO please send them the love needed for this.
* The only missing part in this core is the SuperGrafx chip due to chip size. Will work on this next for this core to complete it.

## Installation and Usage

### Easy mode setup

I highly recommend the updater tools with JSON Generations are done by [@mattpannella](https://github.com/mattpannella/pocket-updater-utility) and [@Monkeymad2](https://github.com/neil-morrison44/pocket-sync). Im sure that [the RetroDriven GUI](https://github.com/RetroDriven/Pocket_Updater) will be updated soon.

### Manual mode setup
Download the core by clicking Releases on the right side of this page, then download the `mazamars312.*.zip` file from the latest release.

To install the core, copy the `Assets`, `Cores`, and `Platform` folders over to the root of your SD card. Please note that Finder on macOS automatically _replaces_ folders, rather than merging them like Windows does, so you have to manually merge the folders.

Make sure you have the HuCard bios for the CDRomII in the \Assets\pcecd\common\ folder

The CUE/BIN Files are also stored in the \Assets\pcecd\common\ folders and it is recommended to have them in their own folders

Then you need to setup the JSON files for the pocket to know which CUE/BIN files are to be used

### JSON Manual mode creation

in the assets\pcecd\Mazamars312.PC Engine CD\ folder there a image_template.json file. depending on how many BIN files files is how many dataslots you use and folder location.

* "data_path": "image/", is the folder location in the \Assets\pcecd\common\ folder
* DataSlot 100 is always the CUE file
* DataSlot 101-127 is for each BIN file. If you only have a single BIN (with MultiTracks in it) then you only put this bin file in Dataslot 101 and delete the rest of the assending slots)

If you do have any issues loading images, you can in the Analouge menu turn on the file debugging (Tools\Developer\Debug Logging) then try to load the JSON. 

Once the error happens, you can then go into the SDCARD then to the folders \system\logs\Mazamars312.pcecd**date_time** to see what happened in the loading process. this can help to see if you have the file names incorrect.

Make sure you then turn off the debugging once fixed as it will slow down access and fill up your SDCARD with logs.

## Features

### Dock Support

Core supports four players/controllers via the Analogue Dock. To enable four player mode, turn on `Use Turbo Tap` setting.

### 6 button controller

Some games support a 6 button controller. For those games, enable the `Use 6 Button Ctrl` option in `Core Settings`. Please note that this option can break games that don't support the 6 button controller, so turn it off if you're not using it.

### Controller Turbo

Like the original PC Engine controllers, this core supports multiple turbo modes. Adjust the `I` and `II` button turbo modes, and use the `X` and `Y` buttons (by default) as your turbo buttons. Note that the original PCE controllers had the turbo on the `I` and `II` buttons directly, rather than having separate buttons, but since the Pocket has more than just two, we use them for the turbo.

### Video Modes

The PC Engine is unique in that it can arbitrarily decide what resolution to display at. The Pocket is more limited, requiring fixed resolutions at all times. I've tried to compromise and cover the most common resolutions output by the PCE, but some are better supported than others. You should see the video centered on the screen with surrounding black bars on some resolutions, but the aspect ratios should be correct.

### Video Options

* `Extra Sprites` - Allows extra sprites to be displayed on each line. Will decrease flickering in some games
* `Raw RGB Color` - Use the raw RGB color palette output by the HUC6260. If disabled, will use the composite color palette

### Audio Options

The core can be quiet in some games, so there are options to boost the master audio (`Master Audio Boost`) and ADPCM channels (`PCM Audio Boost`) And CDROM Channels ("CD Audio Boots").

### Memory Cards

Instead of sharing a memory card (as you would in real life), each game gets its own save file and therefore memory card. Some games don't have the ability to initialize a memory card, so each newly created save file is pre-initialized for use.

## Licensing

All source included in this project from me or the [MiSTer project](https://github.com/MiSTer-devel/TurboGrafx16_MiSTer) is licensed as GPLv2, unless otherwise noted. The original source for [FPGAPCE](https://github.com/Torlus/FPGAPCE), the project this core is based off of, is [public domain](https://twitter.com/Torlus/status/1582663978068893696). The contents of the public domain tweet are reproduced here:

> Indeed. The main reason why I haven't provided a license is that I didn't know how to deal with the different licenses attached to parts of the cores.
Anyway, consider *my own* source code as public domain, i.e do what you want with it, for any use you want. (1/2)

[Additionally, he wrote](https://twitter.com/Torlus/status/1582664299973341184):

> If stated otherwise in the comments at the beginning of a given source file, the license attached prevails. That applies to my FPGAPCE project (https://github.com/Torlus/FPGAPCE).
