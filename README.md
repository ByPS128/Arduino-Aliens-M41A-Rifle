

# Welcome,

the project is a simple simulator of rifle and grenade launcher firing mockups of the popular M41A weapon from the legendary Aliens movie.
The hardware is implemented using an Arduino Nano and a DF Player (also known as the DF Robot) along with a few other peripherals.

#### In the document I will describe both HW and SW, browse it to the places you are interested in.



## So what does intendedHW version consist of:

There are many M41A mock-ups on the market, including 3D printed ones. The version I was sounding did not have a working grenade launcher and it also did not have a retractable slide. We used the magazine space to house the electronics and so this version of the 3D printed rifle has quite a lot of limitations.
However, the software is easy to adapt for a larger application with multiple input signals.

* Arduino Nano
* DF Player for reproducing sounds
* Rotary Encoder with button to control volume, can be without button and it will work without the whole rotary encoder
* LED to illuminate the flashes in the barrel of the rifle
* LED to illuminate the barrel of the rifle
* Button to trigger the rifle's firing pin
* Button to trigger loading / firing of the grenade launcher
* Button to trigger the sound of the rifle bolt retracting
* Button to play magazine change sound
* Double digit 7 segment display
* Required resistors, capacitor and shocky protection diode



## Schematic

I developed the scheme in KiCad (free). It's my first schematic, so please excuse me if I didn't choose some brands correctly. Alternatively, please modify the scheme and make a pull request. The schematic source files are in the KiCad directory.

https://www.kicad.org/

Schema preview:



Schema file resides here: /KiCad/M41A/M41A.kicad_sch

and here is PDF version: /KiCad/M41A-Schematic.pdf

If you would like to open the schema, import the libraries I created. I couldn't find schematic marks for some components, so I created them with their footprints.
The library is in the directory: /KiCad/Project_Library.kicad_sym



### DF Player

I decided to use DF Player for playing sounds for several reasons:

* Low purchase price
* The possibility of controlling the serial line with its own simple protocol
* Clear organization of sound files on the SD card
* A large number of existing compatible libraries to support the protocol

However hend I made the experience that not every clone works properly. I was unlucky and the very first one I bought was built on the `MH2024K-24SS` chip. This has huge latency from the instruction to play the audio to the actual start of playback takes a second.
And that's unusable. That's why I bought a few more from different stores and came across a module with a `TD5580A` chip and it responds instantly. And that's why I recommend you use it. 

I used the protocol handler from an existing library and modified it to suit me better.



### Speaker

I used a 3W speaker. The specification of the DF Player says that the built-in amplifier will drive the 3W speaker to the maximum.
If you wanted to use a more powerful speaker, you would have to insert another amplifier between the player and the speaker and use a separate power supply.

**You can find the documentation for the DF player in the DOC directory, including communication protocol specs.**



### 2 digit 7 segment display

For displaying the remaining bullets in the rifle's magazine, I used, how else, red segment displays. And to get rid of a bunch of wires and resistors, I used a driver that is beautifully compact, works perfectly.
It contains a 74HC595 circuit for each segment. The displays used shared anode, so the segment lights up when its pin is at logic zero.


https://www.aliexpress.us/item/3256805033000283.html



### Rotary Encoder with button

I used this one, with built-in RC filter. Thanks to the filter, the dream was not to treat false presses.



https://www.aliexpress.us/item/3256805548024336.html



### LED diods

I used high brightness 5mm, warm white, 30mA LEDs
https://www.gme.cz/v/1492064/OptoSupply-OSM5DK5201A-LED-5mm-tepla-bila



## What this module can do, how it was designed

* Pressing and holding the trigger of the rifle starts playing the sound of firing and with each round fired the LED flashes to simulate a flash. At the same time, each shot will be read from the counter.
* When all rounds have been fired, the display shows 00 and the sound of the hammer striking empty is heard.
* Each subsequent pull of the trigger with an empty magazine will play this sound.
* The slide release button will also cause the magazine to load to 95 and cause the slide release sound to play.
* The magazine change button will set the magazine count to 95 rounds and trigger the magazine change and slide release sound.
* the grenade launcher trigger button will load a grenade with one press and play a sound consisting of several: chamber opening, grenade insertion and chamber closing.
* another press of the grenade launcher trigger will fire the grenade and play the sound of the grenade being fired. At the same time, the time to wait for the grenade to explode is randomly calculated. The next press of the grenade launcher trigger repeats charge, shot, charge, shot, etc.
* Rotating the rotary encoder changes the volume. When the gun is at rest and you change the volume, the display shows the volume numbers. The volume can also be changed while the sounds are playing. If a firing sound is playing and you are changing the volume, the display shows the cartridge level in the magazine as a priority.
* Pressing/pushing on the rotary encoder has 2 functions. A short press shows the current volume value on the display. A long press (a second or more) causes the status to be entered 



## Other features:

I made some sounds in multiple variations. This is so that the simulation does not appear monotonous. So every time I start shooting, a different option comes up. They repeat over time, but for the shot I made it so that each new playback plays a different file.
Similarly, the pulling of the weapon's bolt is solved, the sound of squeezing the trigger with an empty magazine is also solved.



## Sounds on SD card

The program is somehow built and expects concrete files on the sd card. DF Player uses mtak that I select a directory and a file in it. This is defined by numbers. So the directories are numbered and always with two digits, i.e. "01". I don't use them anymore. It would be easy to use the rotary encoder to select a different directory and thus select a different set of sounds, and suddenly the sounds could be from a specific existing weapon, or totally crazy sci-fi sounds, etc. In directory 01 there are files that must also be numbered in 3 places, i.e. 001, 002, etc. In the constants of the application, it is then defined which number has which sound.
For example (excerpt from AppConstants.h)

```
// I have some sounds in multiple variants and I randomly choose which variant to play.
// For this there are FIRST and LAST containers, which define the file numbers of the sounds of a given type.

// Sound file interval for rifle fire
static const int SAMPLE_RIFLE_FIRE_FIRST = 1;
static const int SAMPLE_RIFLE_FIRE_LAST = 6;

// Sound file interval for cock sound
static const int SAMPLE_COCK_FIRST = 10;
static const int SAMPLE_COCK_LAST = 14;

// Sound file interval for empty magazine, you rpes rifle fire and this empty file will play
static const int SAMPLE_EMPTY_MAGAZINE_FIRST = 20;
static const int SAMPLE_EMPTY_MAGAZINE_LAST = 24;
```



## File list on SD card.

Files must start with a number, but can continue with any name that describes the sound.
This is what my SD card looks like:

```
001-Shoot-95.wav
002-Shoot-95.wav
003-Shoot-95.wav
004-Shoot-95.wav
005-Shoot-95.wav
006-Shoot-95.wav
010-Cock-01-Sniper.wav
011-Cock-02.wav
012-Cock-03.wav
013-Cock-04.wav
014-Cock-05.wav
020-Empty.wav
021-Empty.wav
022-Empty.wav
023-Empty.wav
024-Empty.wav
030-MagazineEject-01.wav
031-MagazineEject-02.wav
040-MagazineLoad-01.wav
041-MagazineLoad-02.wav
042-MagazineLoad-03.wav
050-MagazineReload-01.wav
051-MagazineReload-02.wav
053-MagazineReloadCock-01.wav
054-MagazineReloadCock-02.wav
060-GranadeFire-01.wav
061-GranadeFire-02.wav
062-GranadeFire-03.wav
065-GranadeLoad-01.wav
066-GranadeExplosion-01.wav
067-GranadeExplosion-02.wav
070-Volume-01-Down.wav
071-Volume-02-Down.wav
072-Volume-03-Down.wav
073-Volume-04-Down.wav
074-Volume-05-Down.wav
080-Volume-01-Up.wav
081-Volume-02-Up.wav
082-Volume-03-Up.wav
083-Volume-04-Up.wav
084-Volume-05-Up.wav
090-CardInserted.wav
091-CardRemoved.wav
092-LowBattery.wav
093-VolumeSaved.wav
094-WeaponReady.wav
095-Click-01.wav
```

But in reality, the application, as its source codes define, uses the following files:
(Starting to play unused sounds is commented out in the application.)

```
001-Shoot-95.wav
002-Shoot-95.wav
003-Shoot-95.wav
004-Shoot-95.wav
005-Shoot-95.wav
006-Shoot-95.wav
010-Cock-01-Sniper.wav
011-Cock-02.wav
012-Cock-03.wav
013-Cock-04.wav
014-Cock-05.wav
020-Empty.wav
021-Empty.wav
022-Empty.wav
023-Empty.wav
024-Empty.wav
053-MagazineReloadCock-01.wav
054-MagazineReloadCock-02.wav
060-GranadeFire-01.wav
065-GranadeLoad-01.wav
066-GranadeExplosion-01.wav
093-VolumeSaved.wav
```

It is also essential to use the DriveSort app: [Anerty's Lair - DriveSort](https://www.anerty.net/software/file/DriveSort/) which sorts the files on the SD card so that they are physically organized in sequence.
This has a major impact on the delay/latency before the sound starts playing.



## How I made the sounds.

I searched in sound banks which provides free sounds for free download and use.
For example, I searched for gun cocking sounds under the keyword "cock" and one of the many useful ones I found is, for example, this one:
https://artlist.io/sfx/track/battle-rifles---fn-fal-cocking-gun/80693

It will probably take you some time to find the right sounds that you will like and fit for your application.

The firing of the M41A rifle is specific sound and is difficult to imitate. So I bought the game "Aliens Colonial Marines" and used the sounds from it.

To edit the sounds, I used the AudaCity software, which is free and can do everything necessary in it.
[https://www.audacityteam.org/](https://www.audacityteam.org/)



## Audio file format

DF Player can play many formats including MP3 in CD quality and stereo.
(It even has a simple equalizer, but I haven't used it, but you can, I have implemented functions to adjust the equalizer, just use them.)

The connection I used is monophonic, so using files containing stereo tracks is unnecessary workload for DF Player, and also such files are larger, which means that more power and time are needed to upload and process the file.
And this app (M41A) works well only when it responds instantly.
So, for these reasons, even after several attempts and tests, I decidedto use uncompresed WAV format, 8bit, mono with 22050 sample rate.
It sounds as good from that 3W speaker as the 16bit mono 44100. If you used a better speaker, then it is possible that you would use a better quality sound.

If you would use better quality sounds, there is no need to change anything in the application. The detection of the audio file format is fully under the control of DF Player.

When I wrote about organizing files on the SD card above, I mentioned the 01 directory that I use.
But you can easily have more such directories and the same numbered files in them. These can contain the same but higher quality sounds. By simply changing the value of one constant, you tell the application to use a different directory. And so you can easily test which sounds are better in your application.
The constant is named `DEFAULT_DIRECTORY_NUMBER` and resides in DFPlay.h file
