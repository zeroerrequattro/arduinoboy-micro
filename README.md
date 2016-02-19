#Arduinoboy Micro

This project is meant to make a functional Arduinoboy with boards that mounts ATMega32U4 chips (Eg: Arduino Leonardo, Arduino Micro, Sparkfun Arduino Pro Micro).

As stated by myself and by various users around the web, the code made by Timothy Lamb (aka Trash80) works only on Arduino devices that has the ATMega168/328 chip (Eg: Arduino Uno, Arduino Mini etc.).

This project is intended to work with LSDJ, Nanoloop and mGB.

This is a modified version of the code made by Timothy and it's under GPLv2 License.

You can find the original source code of Arduinoboy for ATMega168/328 here: https://github.com/trash80/Arduinoboy

###Version 1.1

Fixed code in MidiOut Mode. Now it should works (Thanks to Mario Schallner and Riccardo Binetti).<br>
Still have Lamb's commentary. It's too funny to delete.

###Version 1.0

First Release of the code.<br>
Still have Initial Lamb’s commentary

_ _ _

### Modes:

The modes checked have been tested.

 - [x] LSDJ Slave Mode
 - [x] LSDJ Master Mode
 - [x] LSDJ Keyboard Mode
 - [x] mGB Mode
 - [x] LSDJ MIDIOUT Mode _(should work on v1.1)_
 - [ ] LSDJ Live Map Mode
 - [ ] Nanoloop Mode