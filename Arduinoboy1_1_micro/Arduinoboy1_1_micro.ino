/***************************************************************************
 ***************************************************************************
 *                         __      _             __                        *
 *         ____  _________/ /_  __(_)___  ____  / /_  ____  __  __         *
 *        / __ `/ ___/ __  / / / / / __ \/ __ \/ __ \/ __ \/ / / /         *
 *       / /_/ / /  / /_/ / /_/ / / / / / /_/ / /_/ / /_/ / /_/ /          *
 *       \__,_/_/   \__,_/\__,_/_/_/ /_/\____/_.___/\____/\__, /           *
 *                                             (_)       /____/            *
 *                                      /  ` \/ / __/ __/ _ \              *
 *                                     /_/_/_/_/\__/_/  \___/              *
 *                                                                         *
 ***************************************************************************
 ***************************************************************************
 *                                                                         *
 * Version: 1.1 for ATMega32U4                                             *
 * Date:    April 2 2014                                                   *
 * Original Programmer:    Timothy Lamb                                    *
 * aBoy Micro Programmer:  Daniele Biggiogero                              *
 *                                                                         *
 ***************************************************************************
 ***************************************************************************
 *                                                                         *
 * NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE   *
 * NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE   *
 * NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE   *
 * NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE   *
 * NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE   *
 *                                                                         *
 *  This is a variation of the Arduinoboy project created by Timothy Lamb. *
 *  The original code is made to work with Arduino boards that run with    *
 *  ATMega168/328 chip.                                                    * 
 *                                                                         *
 *  ATMega32U4 has a different port mapping (it has also more pin than     *
 *  ATMega168/328) and a different method to access to the serial ports.   *
 *                                                                         *
 *  Modifications, and a little swapping on the analog pin, made the       *
 *  Arduino Micro works with the most of the modes of the original aBoy.   *
 *                                                                         *
 *  Hope to resolve the various issues soon.                               *
 *                                                                         *
 *  Original code:                                                         *
 *  https://github.com/trash80/Arduinoboy                                  *
 *                                                                         *
 *   Arduino pin settings:  (Layout is final)                              *
 *     - 6 LEDS on pins 8 to 13                                            *
 *     - Push button on pin 3 (for selecting mode)                         *
 *     - MIDI Opto-isolator power on pin 4                                 *
 *     - Gameboy Clock line on Analog In pin 5                             *
 *     - Gameboy Serial Data input on Analog In pin 4                      *
 *     - Serial Data from gameboy on Analog In pin 3                       *
 *                                                                         *
 * Program Information:                                                    *
 *    LSDJ Slave Mode Midi Note Effects:                                   *
 *      48 - C-2 Sends a Sequencer Start Command                           *
 *      49 - C#2 Sends a Sequencer Stop Command                            *
 *      50 - D-2 Toggles Normal Tempo                                      *
 *      51 - D#2 Toggles 1/2 Tempo                                         *
 *      52 - E-2 Toggles 1/4 Tempo                                         *
 *      53 - F-2 Toggles 1/8 Tempo                                         *
 *                                                                         *
 *    LSDJ Keyboard Mode:                                                  *
 *      48 - C-2 Mute Pu1 Off/On                                           *
 *      49 - C#2 Mute Pu2 Off/On                                           *
 *      50 - D-2 Mute Wav Off/On                                           *
 *      51 - D#2 Mute Noi Off/On                                           *
 *      52 - E-2 Livemode Cue Sequence                                     *
 *      53 - F-2 Livemode Cursor Up                                        *
 *      54 - F#2 Livemode Cursor Down                                      *
 *      55 - G-2 Livemode Cursor Left                                      *
 *      56 - G#2 Livemode Cursor Right                                     *
 *      57 - A-2 Table Up                                                  *
 *      58 - A#2 Table Down                                                *
 *      59 - B-2 Cue Table                                                 *
 *      60 - C-3 to C-8 Notes!                                             *
 *      Program Change to select from instrument table                     *
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#include <EEPROM.h>
#define MEM_MAX 65
#define NUMBER_OF_MODES 7    //Right now there are 7 modes, Might be more in the future

// !!! do not edit these, they are the position in EEPROM memory 
// that contain the value of each stored setting
#define MEM_CHECK 0
#define MEM_VERSION_FIRST 1
#define MEM_VERSION_SECOND 2
#define MEM_MODE 5
#define MEM_FORCE_MODE 4

#define MEM_LSDJSLAVE_MIDI_CH 6

#define MEM_LSDJMASTER_MIDI_CH 7
#define MEM_KEYBD_CH 8

#define MEM_KEYBD_COMPAT_MODE 9
#define MEM_KEYBD_CH_TO_INST 10

#define MEM_MIDIOUT_NOTE_CH 11
#define MEM_MIDIOUT_CC_CH 15
#define MEM_MIDIOUT_CC_MODE 19
#define MEM_MIDIOUT_CC_SCALING 23
#define MEM_MIDIOUT_CC_NUMBERS 27

#define MEM_MGB_CH 55
#define MEM_LIVEMAP_CH 60

#define MEM_MIDIOUT_BIT_DELAY 61
#define MEM_MIDIOUT_BYTE_DELAY 63

/***************************************************************************
* User Settings
***************************************************************************/

//set to true to always use the settings below, else they are pulled from memory for the software editor
boolean alwaysUseDefaultSettings = false;

//to use usb for serial communication as oppose to MIDI - sets baud rate to 38400
boolean usbMode = false;

byte defaultMemoryMap[MEM_MAX] = {
  0x7F,0x01,0x02,0x7F, // memory init check
  0x00, // force mode (forces lsdj to be sl)
  0x00, // mode
  
  15, // sync effects midi channel (0-15 = 1-16)
  15, // masterNotePositionMidiChannel - LSDJ in master mode will send its song position on the start button via midi note. (0-15 = 1-16)
  
  15, // keyboardInstrumentMidiChannel - midi channel for keyboard instruments in lsdj. (0-15 = 1-16)
  1,  // Keyboard Compatability Mode
  1,  // Set to true if you want to have midi channel set the instrument number / doesnt do anything anymore
  
  0,1,2,3, // midiOutNoteMessageChannels - midi channels for lsdj midi out note messages Default: channels 1,2,3,4
  0,1,2,3, // midiOutCCMessageChannels - midi channels for lsdj midi out CC messages Default: channels 1,2,3,4
  1,1,1,1, // midiOutCCMode - CC Mode, 0=use 1 midi CC, with the range of 00-6F, 1=uses 7 midi CCs with the
           // range of 0-F (the command's first digit would be the CC#), either way the value is scaled to 0-127 on output
  1,1,1,1, // midiOutCCScaling - CC Scaling- Setting to 1 scales the CC value range to 0-127 as oppose to lsdj's incomming 00-6F (0-112) or 0-F (0-15) 
  1,2,3,7,10,11,12, // pu1: midiOutCCMessageNumbers - CC numbers for lsdj midi out, if CCMode is 1, all 7 ccs are used per channel at the cost of a limited resolution of 0-F
  1,2,3,7,10,11,12, // pu2
  1,2,3,7,10,11,12, // wav
  1,2,3,7,10,11,12, // noi
  
  0, 1, 2, 3, 4, // mGB midi channels (0-15 = 1-16)
  15,   //livemap / sync map midi channel (0-15 = 1-16)
  80,1, //midiout bit check delay & bit check delay multiplier 
  0,0   //midiout byte received delay & byte received delay multiplier 
};
byte memory[MEM_MAX];

/***************************************************************************
* Lets Assign our Arduino Pins ...
***************************************************************************/

int pinGBClock     = A5;   // Analog In 0 (PF0) - clock out to gameboy
int pinGBSerialOut = A4;   // Analog In 1 (PF1) - serial data to gameboy
int pinGBSerialIn  = A3;   // Analog In 2 (PF4) - serial data from gameboy

int pinMidiInputPower = 4; // power pin for midi input opto-isolator

int pinStatusLed = 13;     // Status LED
int pinLeds[] = {12,11,10,9,8,13}; // LED Pins

int pinButtonMode = 3;     // Toggle button for selecting the mode

/***************************************************************************
 * Memory
 ***************************************************************************/

int eepromMemoryByte = 0;  // Location of where to store settings from mem

/***************************************************************************
 * Sysex Settings & vars
 ***************************************************************************/

boolean sysexReceiveMode = 0;
boolean sysexProgrammingMode = 0;
boolean sysexProgrammingWaiting = 0;
boolean sysexProgrammingConnected = 0;

unsigned long sysexProgrammerWaitTime = 2000; // 2 seconds
unsigned long sysexProgrammerCallTime = 1000; // 1 second
unsigned long sysexProgrammerLastResponse = 0;
unsigned long sysexProgrammerLastSent = 0;

byte sysexManufacturerId = 0x69; // har har harrrrr :)
int sysexPosition;
byte sysexData[128];
byte longestSysexMessage = 128;

int midioutBitDelay = 0;
int midioutByteDelay = 0;

/***************************************************************************
 * Switches and states
 ***************************************************************************/
boolean sequencerStarted    = false;                // Sequencer has Started
boolean midiSyncEffectsTime = false;
boolean midiNoteOnMode      = false;
boolean midiNoteOffMode     = false;
boolean midiProgramChange   = false;
boolean midiAddressMode     = false;
boolean midiValueMode       = false;

int midiOutLastNote[4] = {-1,-1,-1,-1};

boolean statusLedIsOn       = false;
boolean statusLedBlink      = false;

boolean nanoState           = false;
boolean nanoSkipSync        = false;

int buttonDepressed;
int buttonState;
unsigned long int buttonProgrammerWaitTime = 2000;        // 2 whole seconds
unsigned long int buttonTime;


boolean blinkSwitch[6];
unsigned long int blinkSwitchTime[6];
int switchLight = 0;

int blinkMaxCount = 1000;

unsigned long midioutNoteTimer[4];
byte midioutNoteHold[4][4];
byte midioutNoteHoldCounter[4];
int midioutNoteTimerThreshold = 10;

/***************************************************************************
* Counter vars
***************************************************************************/
int countLSDJTicks     = 0; 	 // for loop int (we need to cycle 8 pulses)
int countSyncTime      = 0;
int countSyncLightTime = 0;
int countSyncSteps     = 0;
int countSyncPulse     = 0;
int countGbClockTicks  = 0;
int countClockPause    = 0;
int countIncommingMidiByte = 0;
int countStatusLedOn   = 0;
unsigned int waitClock = 0;

int miscLastLed;
unsigned long int miscLedTime;
unsigned long int miscLedMaxTime;

/***************************************************************************
* Inbound Data Placeholders
***************************************************************************/
byte incomingMidiByte;  //incomming midi message
byte readgbClockLine;
byte readGbSerialIn;
byte bit;
byte midiData[] = {0, 0, 0};
byte lastMidiData[] = {0, 0, 0};

int incomingMidiNote = 0;
int incomingMidiVel  = 0;
byte readToggleMode;
byte serialWriteBuffer[256];
byte midiDefaultStartOffset;
int writePosition = 0;
int readPosition  = 0;
int lastMode      = 0; //Stores the last selected mode for leds.

byte midiSyncByte;
byte midiSyncByteLast;

byte midiStatusType;
byte midiStatusChannel;

/***************************************************************************
* LSDJ Keyboard mode settings
***************************************************************************/      
byte keyboardNotes[] = {0x1A,0x1B,0x22,0x23,0x21,0x2A,0x34,0x32,0x33,0x31,0x3B,0x3A,
                        0x15,0x1E,0x1D,0x26,0x24,0x2D,0x2E,0x2C,0x36,0x35,0x3D,0x3C};
byte keyboardOctDn = 0x05;
byte keyboardOctUp = 0x06;

byte keyboardInsDn = 0x04;
byte keyboardInsUp = 0x0C;

byte keyboardTblDn = 0x03;
byte keyboardTblUp = 0x0B;

byte keyboardTblCue= 0x29;

byte keyboardMut1 = 0x01;
byte keyboardMut2 = 0x09;
byte keyboardMut3 = 0x78;
byte keyboardMut4 = 0x07;

byte keyboardCurL = 0x6B;
byte keyboardCurR = 0x74;
byte keyboardCurU = 0x75;
byte keyboardCurD = 0x72;
byte keyboardPgUp = 0x7D;
byte keyboardPgDn = 0x7A;
byte keyboardEntr = 0x5A;

int keyboardCurrentOct = 0;
int keyboardCurrentIns = 0;
int keyboardCurrentTbl = 0;

int keyboardLastOct = 0;
int keyboardLastIns = 0;
int keyboardLastTbl = 0;

int keyboardDiff  = 0;
int keyboardCount = 0;
byte keyboardStartOctave = 0x24;
byte keyboardNoteStart  = 0;
byte keyboardNoteOffset = 0;
byte keyboardCommands[12];

void setup() {
  // Init Memory
  initMemory(0);
  
  // Init Pins
  for(int led=0;led<=5;led++) pinMode(pinLeds[led],OUTPUT);
  pinMode(pinStatusLed,OUTPUT);
  pinMode(pinButtonMode,INPUT); 
  DDRF = B00111111; //Set analog in pins as outputs
  
  // Set MIDI Serial Rate
  if(usbMode == true) {
    Serial1.begin(38400); //31250
  } else {
    pinMode(pinMidiInputPower,OUTPUT); 
    digitalWrite(pinMidiInputPower,HIGH); // turn on the optoisolator
    Serial1.begin(31250); //31250
  }
  
  // Set Pin States
  digitalWrite(pinGBClock,HIGH);    // gameboy wants a HIGH line
  digitalWrite(pinGBSerialOut,LOW); // no data to send
  digitalWrite(pinGBSerialIn,LOW);  // no data to receive
  
  // Misc Startup
  keyboardNoteStart = keyboardStartOctave + 12; // Set the octave where the actual notes start (the octave below is for the mutes, cursor, etc)

  // Assign the keyboard mode command array for the first octave
  keyboardCommands[0]  = keyboardMut1;
  keyboardCommands[1]  = keyboardMut2;
  keyboardCommands[2]  = keyboardMut3;
  keyboardCommands[3]  = keyboardMut4;
  keyboardCommands[4]  = keyboardCurL;
  keyboardCommands[5]  = keyboardCurR;
  keyboardCommands[6]  = keyboardCurU;
  keyboardCommands[7]  = keyboardCurD;
  keyboardCommands[8]  = keyboardEntr;
  keyboardCommands[9]  = keyboardTblDn;
  keyboardCommands[10] = keyboardTblUp;
  keyboardCommands[11] = keyboardTblCue;

  // Load Settings from EEPROM
  if(!memory[MEM_FORCE_MODE]) memory[MEM_MODE] = EEPROM.read(MEM_MODE);
  lastMode = memory[MEM_MODE];
  
  startupSequence();
  
  showSelectedMode(); //Light up the LED that shows which mode we are in.
}

// Main Loop, which we don't use to be able to isolate each mode into its own setup and loop functions
void loop () {
  setMode();
  switchMode();
}
