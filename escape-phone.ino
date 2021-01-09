#include <SD.h>
#include <TMRpcm.h>

const char* unlockNumber = "12345";
const int numDigitsInPhoneNumber = 5;
const int toneFrequency = 600;

const byte inputPin = A0;
const byte phonePin = 9;
const int hookTimeout = 100;         //ms
const int pulseTimeout = 70;        //ms
const int digitTimeout = 300;        //ms

const byte SD_ChipSelectPin = 4;
TMRpcm tmrpcm;

int prevValue = 0;
int valueDuration = 0;
char number[numDigitsInPhoneNumber + 1];
int currentDigit = 0;
int digitIndex = 0;
boolean isPulse = false;

typedef enum { ON_HOOK, OFF_HOOK, DIALLING, CONNECTED } stateType;
stateType state = ON_HOOK;

void processNumber(char* number) {
  if (strcmp(number, unlockNumber) == 0) {
    Serial.println("!!!!UNLOCK");
    playback("010.wav");
  }
}

void setup() {
  if (!SD.begin(SD_ChipSelectPin)) {
    return;
  }

  tmrpcm.speakerPin = phonePin;
  tmrpcm.setVolume(7);
  tmrpcm.loop(0);
  tmrpcm.quality(1);
  
  Serial.begin(115200);
  Serial.println("INIT");
}

void loop() {
  delay(1);
  int currentValue = readInput();
  int prevValueDuration = valueDuration;
  valueDuration = currentValue == prevValue ? valueDuration + 1 : 0;

  processValue(currentValue, valueDuration, prevValue, prevValueDuration);
  prevValue = currentValue;
}

void processValue(int currentValue, int valueDuration, int prevValue, int prevValueDuration) {
  if ((state == OFF_HOOK && currentValue == 5) || (prevValue == 5 && currentValue < 5)) {
    Serial.println("currentValue: " + String(currentValue)
      + "; state: " + stateString(state) 
      + "; valueDuration: " + String(valueDuration)
      + "; prevValue: " + String(prevValue)
      + "; prevValueDuration: " + String(prevValueDuration)
    );
  }

// phone state changes
  // ON_HOOK => OFF_HOOK
  if (valueDuration >= hookTimeout && currentValue < 5 && state == ON_HOOK) {
    state = OFF_HOOK;
    Serial.println("OFF_HOOK");
    tone(phonePin, toneFrequency);
    currentDigit = 0;
    digitIndex = 0;
  }
  // OFF_HOOK => DIALLING
  else if (prevValueDuration == pulseTimeout && currentValue == 1 && /*prevValue == 5 && currentValue < 5 &&*/ state == OFF_HOOK) {
    state = DIALLING;
    Serial.println("DIALLING");
    noTone(phonePin);
  }
  // ??? => ON_HOOK
  else if (valueDuration >= hookTimeout && currentValue == 5 && state != ON_HOOK) {
    state = ON_HOOK;
    Serial.println("ON_HOOK");
    noTone(phonePin);
  }

// DIALLING state processing
  if (state == DIALLING) {
    // pulse is fading out
    if (prevValueDuration <= pulseTimeout && currentValue < 5 && prevValue == 5) {
      currentDigit++;
    }
    // some digit dialled
    else if (valueDuration > digitTimeout && currentDigit > 0) {
      processDigit(digitIndex, currentDigit, number);
      currentDigit = 0;
      digitIndex++;
    }

    // number is dialled
    if (digitIndex == numDigitsInPhoneNumber) {
      processNumber(number);
      state = ON_HOOK;
    }
  }
}

void processDigit(int index, int digit, char* number) {
  digit = digit < 10 ? digit : 0;
  number[index] = digit | '0';
  Serial.println("!!!! " + String(digit));

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
}

String stateString(int type) {
  switch (type) {
    case ON_HOOK: return "ON_HOOK";
    case OFF_HOOK: return "OFF_HOOK";
    case DIALLING: return "DIALLING";
    case CONNECTED: return "CONNECTED";
  }
}

void playback(char* filename) {
  tmrpcm.play(filename);
  while (tmrpcm.isPlaying()) {
    delay(50);
  }
  tmrpcm.disable();
}

int readInput() {
  int analog = analogRead(inputPin);
  return map(analog, 1, 1020, 1, 5);
}
