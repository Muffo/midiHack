/*
 MIDI Hack
 */

#define PRINT_MONITOR
// #define WRITE_MIDI


// Standard Midi values
#define NOTE_OFF_CH0 0x80
#define NOTE_ON_CH0  0x90
#define CC_CH0  0xB0


int midiValX = 0;
int midiValY = 0;

void setup() {
  //  Set MIDI baud rate:
  Serial.begin(9600);
}

void loop() {
  delay(10);
  readJoystick(midiValX, midiValY);

  
  sendCommand(0x10, midiValX);
  sendCommand(0x11, midiValY);
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

