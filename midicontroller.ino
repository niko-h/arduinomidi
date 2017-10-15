#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

 /**
 *  Author: Edward Vayne
 *  
 *  Modular Midi-Controller for Arduino
 */

// Variables:
const int switches[] = {4, 5, 6, 7, 8, 9, 10};           // pins for buttons
const int switchesAmount = 7;
int switchStats[switchesAmount];
int currentSwitchStats[switchesAmount];
unsigned long previousSwitchMillis[] = {0, 0, 0, 0, 0, 0, 0};

const int potis[] = {0, 1, 2};                           // pins for potis
const int potisAmount = 3;
int potiStats[potisAmount];
int currentPotiStats[potisAmount];
int midiCCSelects[potisAmount];

// For Toggle
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers
int switchState[switchesAmount];


void setup() {
  MIDI.begin(1);                         // MIDI channel is 1
  
  // Set MIDI baud rate:
  Serial.begin(31250);
  // Serial.begin(9600);

  for(int i = 0; i < switchesAmount; i++) {              //  set the states of the switch pins
    pinMode(switches[i], INPUT);
    digitalWrite(switches[i], HIGH);                     // for arduino-internal pulloff-resistor
    switchStats[i] = HIGH;
    currentSwitchStats[i] = HIGH;
  }
  for(int i = 0; i < potisAmount; i++) {                 //  set the states of the poti pins
    potiStats[i] = 0;
    currentPotiStats[i] = 0;
    midiCCSelects[i] = switchesAmount+1+i;
  }
}

void loop() {
  for(int i = 0; i < switchesAmount; i++) { midiOnSwitch(i); }  // compute each switch
  for(int i = 0; i < potisAmount; i++) { midiOnPoti(i); }       // compute each poti
}
 
// TOGGLE
void midiOnSwitch(int switchId) {
  currentSwitchStats[switchId] = digitalRead(switches[switchId]);

  if (currentSwitchStats[switchId] == LOW && millis() - time > debounce) {
    if (switchStats[switchId] == LOW) {
      switchStats[switchId] = HIGH;
      MIDI.sendControlChange(22+switchId, 0, 1); //send 1 on cc#i, channel 1
    } else {
      switchStats[switchId] = LOW;
      MIDI.sendControlChange(22+switchId, 127, 1); //send 127 on cc#i, channel 1
    }
    time = millis();    
  }
}

// SWITCH
// void midiOnSwitch(int switchId) {
//   currentSwitchStats[switchId] = digitalRead(switches[switchId]);
  
//   if (currentSwitchStats[switchId] == LOW && switchStats[switchId] == HIGH) { // press
//     MIDI.sendControlChange(22+switchId, 127, 1); //send 127 on cc#i, channel 1
//     switchStats[switchId] = LOW;
//   }
//   if (currentSwitchStats[switchId] == HIGH && switchStats[switchId] == LOW) { // release
//     // MIDI.sendControlChange(22+switchId, 0, 1); //send 0 on cc#i, channel 1 // Nimm diese Zeile mit rein, um auch das "Aus-togglen" zu senden.
//     switchStats[switchId] = HIGH;
//   }
// }

void midiOnPoti(int potiId) {
  currentPotiStats[potiId] = round(analogRead(potis[potiId]) / 8); // n/8 for midi conform 0-127
  if ( currentPotiStats[potiId] != potiStats[potiId] ) {    // if analog input has changed
    MIDI.sendControlChange(29+midiCCSelects[potiId], currentPotiStats[potiId], 1); //send on cc#i
    potiStats[potiId] = currentPotiStats[potiId];
  }
}