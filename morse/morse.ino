#include "pitches.h"

// settings
const int debounceTimeout = 10;
const int enableLevel = LOW;
const int morseFailedDelay = 2000;
const int morseTone = 450;
const int morseDotDuration = 100;
const int morseDashDuration = 3 * morseDotDuration;
const String morseMessage = ". . .   - - -   . . .         ";
const int yearLedsSequenceLength = 6;
const byte yearLedsSequence[yearLedsSequenceLength] = {
  B00000,   // 0
  B10000,   // 1
  B11000,   // 2
  B11100,   // 3
  B11110,   // 4
  B11111    // 5
};
const byte yearCorrectSequence[4] = {
  yearLedsSequence[3],
  yearLedsSequence[1],
  yearLedsSequence[4],
  yearLedsSequence[3],
};

const byte idxMorseBtn = 0;
const byte idxBtn1 = 1;
const byte idxBtn2 = 2;
const byte idxBtn3 = 3;
const byte idxBtn4 = 4;

const int btnActiveLevel[5] = {
  LOW,    // morse
  HIGH,    // btn1
  HIGH,    // btn2
  HIGH,    // btn3
  HIGH     // btn4
};

// input
const int btnPins[5] = {
  5,    // morse
  6,    // btn1
  7,    // btn2
  8,    // btn3
  9     // btn4
};

// output
const int shiftData = 2;    //3 on shifter
const int shiftLatch = 3;   //5 on shifter
const int shiftClock = 4;   //6 on shifter
const int buzzer = 10;

const int globalEnable = 11;

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

// anthem
int ussr[] = {
  NOTE_G4, NOTE_C5, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_E4, NOTE_E4, 
  NOTE_A4, NOTE_G4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_C4, 
  NOTE_D4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5
  //, 
//  NOTE_E5, NOTE_D5, NOTE_C5, NOTE_D5, NOTE_B4, NOTE_G4, 
//  NOTE_C5, NOTE_B4, NOTE_A4, NOTE_B4, NOTE_E4, NOTE_E4, 
//  NOTE_A4, NOTE_G4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_C4, 
//  NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_B4, NOTE_C5, NOTE_D5, 
//  NOTE_E5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_G4, NOTE_G4, NOTE_B4, NOTE_C5, NOTE_D5,
//  NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_E4, NOTE_E4, NOTE_G4, NOTE_A4, NOTE_B4,
//  NOTE_C5, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_A4, NOTE_C5, NOTE_F5,
//  NOTE_F5, NOTE_E5, NOTE_D5, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_C5, NOTE_C5,
//  NOTE_D5, NOTE_C5, NOTE_B4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_A4, NOTE_A4,
//  NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_C4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5
};

int noteDurations[] = {
  8, 4, 6, 16, 4, 8, 8, 
  4, 6, 16, 4, 8, 8, 
  4, 8, 8, 4, 8, 8, 4, 8, 8, 2
  //,
//  4, 6, 16, 4, 8, 8, 
//  4, 6, 16, 4, 8, 8, 
//  4, 6, 16, 4, 6, 16, 
//  4, 6, 16, 8, 8, 8, 8, 
//  2, 8, 8, 8, 8, 3, 8, 8, 8, 8, 8,
//  2, 8, 8, 8, 8, 3, 8, 8, 8, 8, 8,
//  4, 6, 16, 4, 6, 16, 4, 8, 8, 2,
//  2, 8, 8, 8, 8, 3, 8, 2,
//  2, 8, 8, 8, 8, 3, 8, 2,
//  4, 6, 16, 4, 4, 2, 4, 4, 1
};


String strippedMessage = morseMessage;

void setup() {
  Serial.begin(9600);

  pinMode(btnPins[idxMorseBtn], INPUT_PULLUP);
  pinMode(btnPins[idxBtn1], INPUT_PULLUP);
  pinMode(btnPins[idxBtn2], INPUT_PULLUP);
  pinMode(btnPins[idxBtn3], INPUT_PULLUP);
  pinMode(btnPins[idxBtn4], INPUT_PULLUP);

  pinMode(shiftData, OUTPUT);
  pinMode(shiftLatch, OUTPUT);
  pinMode(shiftClock, OUTPUT);
  pinMode(buzzer, OUTPUT);

  pinMode(globalEnable, INPUT_PULLUP);

  pushData(B00000000, B00000000, B00000000);

  strippedMessage.replace(" ", "");

  Serial.println("INIT");
}

int lastValueBtn[5] = {0, 0, 0, 0, 0};
unsigned long prevValueBtnMillis[5] = {0, 0, 0, 0, 0};
int btnLedIdx[4] = {0, 0, 0, 0};

unsigned long prevResult = 0;
unsigned long prevBeep = millis();
int nextCharIdx = 0;
boolean isEnabled = false;
void loop() {
  if (digitalRead(globalEnable) != enableLevel) {
    resetMorseBeep();
    pushData(0, 0, 0);
    isEnabled = false;
    
    delay(100);
    return;
  }

  if (!isEnabled) {
    //play music
    for (int thisNote = 0; thisNote < sizeof(ussr) / 2; thisNote++) {
      int noteDuration = 2000 / noteDurations[thisNote];
      tone(buzzer, ussr[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(buzzer);
    }
    isEnabled = true;
    
    delay(morseFailedDelay);
  }
  
  byte btn1 = readBtnLedSequence(idxBtn1);
  byte btn2 = readBtnLedSequence(idxBtn2);
  byte btn3 = readBtnLedSequence(idxBtn3);
  byte btn4 = readBtnLedSequence(idxBtn4);

  boolean isYearSequenceCorrect =
    btn1 == yearCorrectSequence[0] &&
    btn2 == yearCorrectSequence[1] &&
    btn3 == yearCorrectSequence[2] &&
    btn4 == yearCorrectSequence[3];

  boolean morseCodeCorrect = readMorseCode();

  if (morseCodeCorrect) {
    resetMorseBeep();
  } else {
    morseBeep();
  }

  unsigned long result = btn1;
  result = (result << 5) | btn2;
  result = (result << 5) | btn3;
  result = (result << 5) | btn4;
  result = (result << 1) | isYearSequenceCorrect;
  result = (result << 1) | morseCodeCorrect;
  result = (result << 1) | successValue(morseCodeCorrect && isYearSequenceCorrect ? 1 : 0);
  result = (result << 1) | 0;

  if (result != prevResult) {
    Serial.println("result: "  + String(morseCodeCorrect));
    Serial.println(result, BIN);
    Serial.println(result >> 8, BIN);
    Serial.println(result >> 16, BIN);

    pushData(result, result >> 8, result >> 16);
    prevResult = result;
  }
}
unsigned long last = millis();
unsigned long lastPressed = millis();
boolean prevPressed = false;
int nextMorseIdx = 0;
boolean readMorseCode() {
  unsigned long current = millis();
  unsigned long gap = current - last;
  boolean pressed = isBtnPressed(idxMorseBtn, true);

  if (pressed && !prevPressed) {
      prevPressed = true;
      last = current;
  } else if (!pressed && prevPressed) {
      if (strippedMessage.length() == nextMorseIdx || (current - lastPressed) > morseFailedDelay) {
        nextMorseIdx = 0;
      }

      lastPressed = millis();
      prevPressed = false;
      last = current;
      if (gap > morseDotDuration) {
          char sym = (gap > morseDashDuration ? '-' : '.');
          if (strippedMessage[nextMorseIdx] == sym) {
            nextMorseIdx++;
          } else {
            nextMorseIdx = 0;
          }
          Serial.println("sym: " + String(sym) +";" + String(nextMorseIdx) +"; gap: "+ String(gap));
      }
  }

  return strippedMessage.length() == nextMorseIdx;
}

byte readBtnLedSequence(byte idx) {
  if (isBtnPressed(idx, false)) {
    btnLedIdx[idx - 1] = yearLedsSequenceLength - 1 == btnLedIdx[idx - 1] ? 0 : btnLedIdx[idx - 1] + 1;
    
    //Serial.print("btn " + String(idx) + "; value idx: " + btnLedIdx[idx - 1] + "; output: ");
    //Serial.println(yearLedsSequence[btnLedIdx[idx - 1]], BIN);
  }
  return yearLedsSequence[btnLedIdx[idx - 1]];
}

long beepDuration = 0;
void morseBeep() {
  unsigned long currentMillis = millis();
  if (currentMillis - prevBeep > beepDuration) {
    prevBeep = currentMillis;
    switch (morseMessage[nextCharIdx]) {
      case '.':
        tone(buzzer, morseTone);
        beepDuration = morseDotDuration;
        break;
      case '-':
        tone(buzzer, morseTone);
        beepDuration = morseDashDuration;
        break;
      case ' ':
      default:
        noTone(buzzer);
        beepDuration = morseDotDuration;
        break;
    }

    nextCharIdx = morseMessage.length() - 1 == nextCharIdx ? 0 : nextCharIdx + 1;
  }
}

void resetMorseBeep() {
  noTone(buzzer);
  prevBeep = millis();
  nextCharIdx = 0;
}

boolean isBtnPressed(byte idx, boolean returnCurrentState) {
  unsigned long currentMillis = millis();
  unsigned long prevValueMillis = prevValueBtnMillis[idx];
  int lastValue = lastValueBtn[idx];

  int value = currentMillis - prevValueMillis > debounceTimeout ? digitalRead(btnPins[idx]) : lastValue;

  boolean result = false;
  if (returnCurrentState) {
    result = value == btnActiveLevel[idx];
  } else if (value != lastValue) {
    result = value == btnActiveLevel[idx];
    prevValueBtnMillis[idx] = currentMillis;
    lastValueBtn[idx] = value;
  }
  return result;
}

void pushData(byte ioo, byte oio, byte ooi) {
  digitalWrite(shiftLatch, LOW);
  shiftOut(shiftData, shiftClock, LSBFIRST, ioo);
  shiftOut(shiftData, shiftClock, LSBFIRST, oio);
  shiftOut(shiftData, shiftClock, LSBFIRST, ooi);
  digitalWrite(shiftLatch, HIGH);
}

int lastSucccessChange = millis();
boolean tmpSuccessValue = true;
boolean successValue(boolean success) {
  if (!success) return success;

  int current = millis();

  if ((current - lastSucccessChange) > 1000) {
    tmpSuccessValue = !tmpSuccessValue;
    lastSucccessChange = current;
  }

  return tmpSuccessValue;
}
