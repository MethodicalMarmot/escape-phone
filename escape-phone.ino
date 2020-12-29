#include <SD.h>
#include <TMRpcm.h>

const char* unlockNumber = "12345";
const int numDigitsInPhoneNumber = 5;
const int toneFrequency = 600;

const byte inputPin = A0;
const byte phonePin = 9;
const int triggerReadings = 50;         //ms
const int diallingReadings = 10;        //ms

const byte SD_ChipSelectPin = 4;
TMRpcm tmrpcm;

int prevReading = 0;
int readIndex = 0;
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
  
  Serial.begin(9600);
  Serial.println("INIT");
}

void processValue(int hookValue, int sameValueMs) {    
  //Serial.println("hookValue: " + String(hookValue) + "; state: " + stateString(state) + "; sameValueMs: " + String(sameValueMs));
// phone state changes
  // ON_HOOK => OFF_HOOK
  if (sameValueMs >= triggerReadings && hookValue > 1 && hookValue < 5 && state == ON_HOOK) {
    state = OFF_HOOK;
    Serial.println("OFF_HOOK");
    tone(phonePin, toneFrequency);
    isPulse = false;
    currentDigit = 0;
    digitIndex = 0;
  }
  // OFF_HOOK => DIALLING
  else if (sameValueMs >= triggerReadings && hookValue == 1 && state == OFF_HOOK) {
    state = DIALLING;
    Serial.println("DIALLING");
    noTone(phonePin);
  }
  // ??? => ON_HOOK
  else if (hookValue > 4 && (state == OFF_HOOK || (state == DIALLING && sameValueMs >= triggerReadings))) {
    state = ON_HOOK;
    Serial.println("ON_HOOK");
    noTone(phonePin);
  }

// DIALLING state processing
  if (state == DIALLING && sameValueMs >= diallingReadings) {
    // pulse is in progress
    if (hookValue == 5) {
      isPulse = true;
    }
    // pulse is fading out
    else if (hookValue < 5 && isPulse) {
      isPulse = false;
      currentDigit++;
    }
    // some digit dialled
    else if (sameValueMs > triggerReadings && currentDigit > 0) {
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

void loop() {
  delay(1);
  int hookValue = readInput();
  readIndex = hookValue == prevReading ? readIndex + 1 : 0;
  prevReading = hookValue;
  processValue(hookValue, readIndex);
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
