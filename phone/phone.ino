#define invert(value) value == LOW ? HIGH : LOW
#include <Keypad.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// quest setup
const char* unlockNumber = "12345";
const int numDigitsInPhoneNumber = 5;
const int successFilename = 10;
const int beepFilename = 11;
const int beepVolume = 10;
const int messageVolume = 30;

// pins setup
const byte hookPin = 2;
const byte lockPin = 13;

// hook setup
const int offHookValue = LOW;
const int debounceTimeout = 50;

// lock setup
const int unlocked = HIGH;
const int locked = invert(unlocked);

// keypad setup
const byte ROWS = 4;
const byte COLS = 4;
const byte rowPins[ROWS] = {7, 8, 9, 10};
const byte colPins[COLS] = {6, 5 ,4 ,3};
const char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'x'},
  {'4', '5', '6', 'y'},
  {'7', '8', '9', 'z'},
  {'*', '0', '#', 'i'}
};

//const byte ROWS = 4;
//const byte COLS = 3;
//const byte rowPins[ROWS] = {6, 7, 8, 9};
//const byte colPins[COLS] = {5 ,4 ,3};
//const char hexaKeys[ROWS][COLS] = {
//  {'1', '2', '3'},
//  {'4', '5', '6'},
//  {'7', '8', '9'},
//  {'*', '0', '#'}
//};

// dfmini setup
SoftwareSerial mySoftwareSerial(12, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

typedef enum { ON_HOOK, OFF_HOOK, DIALLING, UNLOCK } stateType;
stateType state = ON_HOOK;

char number[numDigitsInPhoneNumber + 1];
int digitIndex = 0;

long prevHookMillis = millis();
int lastHookState = -1;

Keypad digitpad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup() {
  Serial.begin(9600);

  mySoftwareSerial.begin(9600);
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("Unable to begin:");
    Serial.println("1.Please recheck the connection!");
    Serial.println("2.Please insert the SD card!");
    while(true);
  }
  Serial.println("DFPlayer online");

  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms

  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  
  pinMode(hookPin, INPUT_PULLUP);
  pinMode(lockPin, OUTPUT);
}


void loop() {
  unsigned long currentMillis = millis();
  char digit = digitpad.getKey();
  int currentHook = currentMillis - prevHookMillis > debounceTimeout ? digitalRead(hookPin) : lastHookState;

  if (currentHook != lastHookState) {
    prevHookMillis = currentMillis;
    lastHookState = currentHook;

    if (currentHook == offHookValue) {
      changeState(OFF_HOOK);
      digitalWrite(lockPin, locked);
      digitIndex = 0;
      myDFPlayer.volume(beepVolume);
      playback(beepFilename);
    }
    else {
      changeState(ON_HOOK);
      stopPlayback();
    }
  }
  
  if (state == OFF_HOOK && digit) {
    changeState(DIALLING);
    stopPlayback();
    myDFPlayer.volume(messageVolume);
  }

  if (state == DIALLING && digit) {
    switch (digit) {
      case '0': playback(0); break;
      case '1': playback(1); break;
      case '2': playback(2); break;
      case '3': playback(3); break;
      case '4': playback(4); break;
      case '5': playback(5); break;
      case '6': playback(6); break;
      case '7': playback(7); break;
      case '8': playback(8); break;
      case '9': playback(9); break;
    }

    number[digitIndex] = digit;
    digitIndex++;
  }

  if (state == DIALLING && digitIndex == numDigitsInPhoneNumber) {
    processNumber(number);
  }
}

void changeState(stateType newState) {
  state = newState;
  Serial.println("State: " + String(state));
}

void processNumber(char* number) {
  waitTillStop();
  if (strcmp(number, unlockNumber) == 0) {
    changeState(UNLOCK);
    digitalWrite(lockPin, unlocked);
    playback(successFilename);
    Serial.println("!!!!UNLOCKED");
  } else {
    changeState(ON_HOOK);
    lastHookState = invert(lastHookState);
  }
}

void playback(int filename) {
  Serial.println("Playback: " + String(filename));
  myDFPlayer.playMp3Folder(filename);
  delay(debounceTimeout);
}

void stopPlayback() {
  myDFPlayer.pause();
  delay(debounceTimeout);
}

void waitTillStop() {
  int ttl = 5000;
  int step = debounceTimeout;
  while (myDFPlayer.readState() != 512 && ttl > 0) {
    ttl -= step;
    delay(step);
  }
}
