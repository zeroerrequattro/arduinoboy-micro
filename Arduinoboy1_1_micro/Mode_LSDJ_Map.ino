/**************************************************************************
 * Name:    Timothy Lamb                                                  *
 * Email:   trash80@gmail.com                                             *
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

boolean midiTickHit;
unsigned long midiTickTime;
unsigned long currentTime;
int midiTickWait = 4000; //1 ms

void modeLSDJMapSetup()
{
  digitalWrite(pinStatusLed,LOW);
  DDRF  = B11110011; //Set analog in pins as outputs
  PORTF = B00000001;
  midiSyncByte = 0xFF;
  blinkMaxCount=1000;
  modeLSDJMap();
}

void modeLSDJMap()
{
  while(1){  //Loop forever

  checkClockTick();
  if (Serial1.available()) {                 //If MIDI Byte Availaibleleleiel
    incomingMidiByte = Serial1.read();           //Read it
    checkForProgrammerSysex(incomingMidiByte);
    
    if(incomingMidiByte & 0x80) {                //If we have received a MIDI Status Byte
      switch (incomingMidiByte) {                    
        case 0xF8:                                //Case: Clock Message Recieved
          if(sequencerStarted){
            midiTickHit=true;
            midiTickTime=micros();
          }
          midiSyncByte = 0xFF;
          break;
        case 0xFA:                                // Case: Transport Start Message
        case 0xFB:                                // and Case: Transport Continue Message
          sequencerStart();                     // Start the sequencer
          midiTickHit=false;
          break;
        case 0xFC:                                // Case: Transport Stop Message
          sequencerStop();                        // Stop the sequencer
          midiTickHit=false;
          break;
        default:
          midiData[0] = incomingMidiByte;
          midiNoteOnMode = true;
          if(midiData[0] == (0x90+memory[MEM_LIVEMAP_CH])) midiTickHit=false;
        }
    } else if(midiNoteOnMode) {   //if we've received a message thats not a status and our note capture mode is true
      midiNoteOnMode = false;
      midiData[1] = incomingMidiByte;
      if(midiData[0] == (0x90+memory[MEM_LIVEMAP_CH])) midiTickHit=false;
    } else {
      midiNoteOnMode = true;
      if(midiData[0] == (0x90+memory[MEM_LIVEMAP_CH]) && incomingMidiByte) {
          sendByteToGameboy(midiData[1]);
          updateVisualSync();
          midiTickHit = false;
      }
    }
    //Serial1.write(incomingMidiByte);
    checkClockTick();
  } else {
    setMode();         //Check if the mode button was depressed
    updateStatusLight();
    checkClockTick();
    updateBlinkLights();
  }
  }
}

void checkClockTick()
{
  currentTime = micros();
  if(midiTickHit && currentTime>(midiTickTime+midiTickWait)) {
      sendByteToGameboy(0xFF);
      updateVisualSync();
      midiTickHit=false;
  }  
}
