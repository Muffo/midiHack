/*
 MIDI Hack
 */


/* ===== Buttons ===== */

#define NUM_BUTTONS 4
#define BUTTON_BASE 4
int buttons[NUM_BUTTONS];
int newButtons[NUM_BUTTONS];


/* ===== Sonar ===== */

#include <NewPing.h>

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.

// Maximum sensor distance is rated at 400-500cm.
#define MAX_DISTANCE 40  // Maximum distance we want to ping for (in centimeters). 

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

int midiValDist = 0;
int sentMidiValDist = 0;
int sonarDelay = 0;


/* ===== Joystick ===== */

int midiValX = 0;
int midiValY = 0;
int sentMidiValX = 0;
int sentMidiValY = 0;


/* ===== Midi ===== */

// #define PRINT_MONITOR
#define WRITE_MIDI

// Standard Midi values
#define NOTE_OFF_CH0 0x80
#define NOTE_ON_CH0  0x90
#define CC_CH0  0xB0

/* ===== Debug ===== */

// #define BUTTON_DEBUG
// #define SONAR_DEBUG
// #define STICK_DEBUG

void setup() {
  for (int i=0; i < BUTTON_BASE; i++) {
    pinMode(BUTTON_BASE + i, INPUT);
  }
  Serial.begin(9600);
}



void loop() {
  
  delay(2);
  sonarDelay++;
  readJoystick(midiValX, midiValY);
  if (sonarDelay == 10) {
    midiValDist = readSonar();
    sonarDelay = 0;
  }
  readButtons(newButtons);
  

  if (sentMidiValX != midiValX) {
    sendCommand(0x10, midiValX);
    sentMidiValX = midiValX;
  }
  
  if (sentMidiValY != midiValY) {
    sendCommand(0x11, midiValY);
    sentMidiValY = midiValY;
  }

  if (sentMidiValDist != midiValDist) {
    sendCommand(0x12, midiValDist);
    sentMidiValDist = midiValDist;
  }

  for (int i=0; i < NUM_BUTTONS; i++) {
    if (buttons[i] != newButtons[i]) {
      if (newButtons[i] == 0) { 
        noteOff(24 + i, 127);        
      } else {
        noteOn(24 + i, 127);
      }
      #ifdef BUTTON_DEBUG
         Serial.print(i);
         Serial.print(" button: ");
         Serial.println(newButtons[i]);
       #endif
      buttons[i] = newButtons[i];
    }
  }
}

void readButtons(int *btns) {
  for (int i=0; i < NUM_BUTTONS; i++) {
    btns[i] = !digitalRead(BUTTON_BASE + i);    
  }
}

int readSonar() {
  unsigned int pingVal = 0;
  byte result = 0;
 
  pingVal = sonar.ping_median(5);
  pingVal = sonar.convert_cm(pingVal);

  // Remove offset and re-scale
  pingVal = 3 * (max(pingVal - 7, 0));
  result = min(pingVal, 127);

  result = 127 - result;

#ifdef SONAR_DEBUG
  Serial.println(result);
#endif
}

void readJoystick(int &horiz, int &vert) {

  int horizVal = 0; 
  int horizPin = A0;
  int vertVal = 0; 
  int vertPin = A1;

  horizVal = analogRead(horizPin);
  horiz = horizVal / 8;

  vertVal = analogRead(vertPin);
  vert = vertVal / 8;

#ifdef STICK_DEBUG
  Serial.print(vert);
  Serial.print(" - ");
  Serial.println(horiz);
#endif
}


void sendCommand(int cc, int value) {
  writeMidi(CC_CH0, cc, value);
}

//  Plays a MIDI note.  Doesn't check to see that
//  cmd is greater than 127, or that data values are  less than 127:
void noteOn(int pitch, int velocity) {
  writeMidi(NOTE_ON_CH0, pitch, velocity);
}

void noteOff(int pitch, int velocity) {
  writeMidi(NOTE_OFF_CH0, pitch, velocity);
}



void writeMidi(int byte1, int byte2, int byte3) {
#ifdef PRINT_MONITOR
  Serial.println(byte1);
  Serial.println(byte2);
  Serial.println(byte3);
#endif

#ifdef WRITE_MIDI
  Serial.write(byte1);
  Serial.write(byte2);
  Serial.write(byte3);
#endif
}

