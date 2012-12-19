/**
 *  @author: Edward Vayne
 *  
 *  Midi-Controller for Arduino
 */

#include <MIDI.h>

// Variables:
const int switches[] = {4, 5, 6, 7, 8, 9, 10};                  // pins for buttons
const int switchesAmount = sizeof(switches) / sizeof(int);
int switchStats[switchesAmount];
int currentSwitchStats[switchesAmount];

const int potis[] = {0, 1, 2};                                  // pins for potis
const int potisAmount = sizeof(potis) / sizeof(int);
int potiStats[potisAmount];
int currentPotiStats[potisAmount];
int midiCCSelects[potisAmount];

void setup() {
  MIDI.begin(4);                                                // MIDI channel is 4
  MIDI.turnThruOff();  
  pinMode(LEDpin, OUTPUT);  
  for(int i = 0; i < switchesAmount; i++) {                     //  set the states of the switch pins:
    pinMode(switches[i], INPUT);
    digitalWrite(switches[i], HIGH);                            // for arduino-internal pulloff-resistor
    switchStats[i] = LOW;
    currentSwitchStats[i] = LOW;
  }
  for(int i = 0; i < potisAmount; i++) {                        //  set the states of the poti pins:
    potiStats[i] = 0;
    currentPotiStats[i] = 0;
    midiCCSelects[i] = 20+i;
  }
}

void loop() {
  for(int i = 0; i < switchesAmount; i++) { midiOnSwitch(i); }  // compute each switch
  for(int i = 0; i < potisAmount; i++) { midiOnPoti(i); }       // compute each poti
}
 
void midiOnSwitch(int switchId) {
  currentSwitchStats[switchId] = digitalRead(switches[switchId]);
  if (currentSwitchStats[switchId] == HIGH && switchStats[switchId] == LOW) {  // press
    MIDI.sendNoteOn(switchId, 127, 1);                          // Send a Note (pitch i, velocity 127 on channel 1)
  }
  if (currentSwitchStats[switchId] == LOW && switchStats[switchId] == HIGH) {  // release
    MIDI.sendNoteOn(switchId, 0, 1);
  }
  switchStats[switchId] = currentSwitchStats[switchId];
}

void midiOnPoti(int potiId) {
  currentPotiStats[potiId] = analogRead(potis[potiId]) / 8;     // divides by 8 to reduce to midi conform 0-127
  if ( currentPotiStats[potiId] != potiStats[potiId] ) {        // check if analog input has changed
    MIDI.sendControlChange(midiCCSelects[potiId], currentPotiStats[potiId], 1); //send control change on cc#i
  }
  potiStats[potiId] = currentPotiStats[potiId];
}
