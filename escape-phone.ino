#define invert(value) value == LOW ? HIGH : LOW
#include <Keypad.h>

// quest setup
const char* unlockNumber = "12345";
const int numDigitsInPhoneNumber = 5;

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


typedef enum { ON_HOOK, OFF_HOOK, DIALLING, UNLOCK } stateType;
stateType state = ON_HOOK;

char number[numDigitsInPhoneNumber + 1];
int digitIndex = 0;

long prevHookMillis = millis();
int lastHookState = -1;

Keypad digitpad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup() {
  pinMode(hookPin, INPUT_PULLUP);
  pinMode(lockPin, OUTPUT);
  Serial.begin(9600);
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
      playback("beep.wav");
    }
    else {
      changeState(ON_HOOK);
      stopPlayback();
    }
  }
  
  if (state == OFF_HOOK && digit) {
    changeState(DIALLING);
    stopPlayback();
  }

  if (state == DIALLING && digit) {
    switch (digit) {
      case 0: return playback("000.wav");
      case 1: return playback("001.wav");
      case 2: return playback("002.wav");
      case 3: return playback("003.wav");
      case 4: return playback("004.wav");
      case 5: return playback("005.wav");
      case 6: return playback("006.wav");
      case 7: return playback("007.wav");
      case 8: return playback("008.wav");
      case 9: return playback("009.wav");
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
  if (strcmp(number, unlockNumber) == 0) {
    changeState(UNLOCK);
    digitalWrite(lockPin, unlocked);
    playback("unlock.wav");
    Serial.println("!!!!UNLOCK");
  } else {
    changeState(ON_HOOK);
    lastHookState = invert(lastHookState);
  }
}

void playback(char* filename) {
  //TODO
}

void stopPlayback() {
  //TODO
}
