// settings
const int debounceTimeout = 10;
const int celebrationChangeDelay = 1000;
const int enableLevel = LOW;
const int isEnableLevel = HIGH;
const int lockedLevel = LOW;
const int morseFailedDelay = 1000;
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
const int isEnabledPin = 12;
const int lockPin = 13;

String strippedMessage = morseMessage;

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
      if (gap > morseDotDuration / 2) {
          char sym = (gap > morseDashDuration / 2 ? '-' : '.');
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

void resetMorse() {
  nextMorseIdx = 0;
  prevPressed = false;
}

int lastValueBtn[5] = {0, 0, 0, 0, 0};
unsigned long prevValueBtnMillis[5] = {0, 0, 0, 0, 0};
int btnLedIdx[4] = {0, 0, 0, 0};
byte readBtnLedSequence(byte idx) {
  if (isBtnPressed(idx, false)) {
    btnLedIdx[idx - 1] = yearLedsSequenceLength - 1 == btnLedIdx[idx - 1] ? 0 : btnLedIdx[idx - 1] + 1;
    
    //Serial.print("btn " + String(idx) + "; value idx: " + btnLedIdx[idx - 1] + "; output: ");
    //Serial.println(yearLedsSequence[btnLedIdx[idx - 1]], BIN);
  }
  return yearLedsSequence[btnLedIdx[idx - 1]];
}

void resetYear() {
  lastValueBtn[0] = lastValueBtn[1] = lastValueBtn[2] = lastValueBtn[3] = lastValueBtn[4] = 0;
  prevValueBtnMillis[0] = prevValueBtnMillis[1] = prevValueBtnMillis[2] = prevValueBtnMillis[3] = prevValueBtnMillis[4] = 0;
  btnLedIdx[0] = btnLedIdx[1] = btnLedIdx[2] = btnLedIdx[3] = 0;
}

long beepDuration = 0;
unsigned long prevBeep = millis();
int nextCharIdx = 0;
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

void resetLeds() {
  pushData(B00000000, B00000000, B00000000);
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

#define NOTE_G4  392
#define NOTE_C5  523
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_D5  587

// anthem
int ussr[] = {
  NOTE_G4, NOTE_C5, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_E4, NOTE_E4, 
  NOTE_A4, NOTE_G4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_C4, 
  NOTE_D4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5
};

int noteDurations[] = {
  8, 4, 6, 16, 4, 8, 8, 
  4, 6, 16, 4, 8, 8, 
  4, 8, 8, 4, 8, 8, 4, 8, 8, 2
};

int thisNote = 0;
unsigned long nextNoteMs = millis();
unsigned long silentInMs = millis();
void playAnthem() {
  unsigned long current = millis();
  if (current > nextNoteMs && thisNote < sizeof(ussr) / 2) {
    tone(buzzer, ussr[thisNote]);
    int noteDuration = 1200 / noteDurations[thisNote];
    int pauseBetweenNotes = noteDuration * 0.65;
    silentInMs = current + noteDuration;
    nextNoteMs = current + noteDuration + pauseBetweenNotes;
    thisNote++;
  } else if (current > silentInMs) {
    noTone(buzzer);
  }
}

void resetAnthem() {
  thisNote = 0;
  nextNoteMs = millis();
  silentInMs = millis();
}

const int celebrationSequenceLength = 10;
const byte celebrationSequence[celebrationSequenceLength] = {
  B00000,   // 0
  B10000,   // 1
  B01000,   // 2
  B00100,   // 3
  B00010,   // 4
  B00001,    // 5
  B00010,   // 6
  B00100,   // 7
  B01000,   // 8
  B10000,   // 9
};

int celebrationRow1Idx;
int celebrationRow2Idx;
int celebrationRow3Idx;
int celebrationRow4Idx;
boolean celebrationLed1;
boolean celebrationLed2;
unsigned long nextChange = millis();
unsigned long celebration() {
  unsigned long current = millis();
  if (current > nextChange) {
    nextChange = current + celebrationChangeDelay;

    celebrationRow1Idx = celebrationRow1Idx > celebrationSequenceLength - 1 ? 0 : celebrationRow1Idx + 1;
    celebrationRow2Idx = celebrationRow2Idx > celebrationSequenceLength - 1 ? 0 : celebrationRow2Idx + 1;
    celebrationRow3Idx = celebrationRow3Idx > celebrationSequenceLength - 1 ? 0 : celebrationRow3Idx + 1;
    celebrationRow4Idx = celebrationRow4Idx > celebrationSequenceLength - 1 ? 0 : celebrationRow4Idx + 1;
    celebrationLed1 = !celebrationLed1;
    celebrationLed2 = !celebrationLed2;
  }
  
  unsigned long result = celebrationSequence[celebrationRow1Idx];
  result = (result << 5) | celebrationSequence[celebrationRow2Idx];
  result = (result << 5) | celebrationSequence[celebrationRow3Idx];
  result = (result << 5) | celebrationSequence[celebrationRow4Idx];
  result = (result << 1) | celebrationLed1;
  result = (result << 1) | celebrationLed2;
  result = (result << 1) | true;
  result = (result << 1) | 0;

  return result;
}

void resetCelebration() {
  randomSeed(analogRead(0));
  celebrationRow1Idx = random(0, celebrationSequenceLength - 1);
  celebrationRow2Idx = random(0, celebrationSequenceLength - 1);
  celebrationRow3Idx = random(0, celebrationSequenceLength - 1);
  celebrationRow4Idx = random(0, celebrationSequenceLength - 1);
  celebrationLed1 = random(0, 1) == 1;
  celebrationLed2 = random(0, 1) == 1;
}

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
  pinMode(isEnabledPin, OUTPUT);
  pinMode(lockPin, OUTPUT);
  
  digitalWrite(isEnabledPin, isEnableLevel == HIGH ? LOW : isEnableLevel);
  digitalWrite(lockPin, lockedLevel);
  resetLeds();

  strippedMessage.replace(" ", "");
  resetCelebration();

  Serial.println("INIT");
}

unsigned long prevResult = 0;
boolean isEnabled = false;
void loop() {
  if (
    (isEnabled && (digitalRead(globalEnable) != enableLevel)) ||
    (!isEnabled && (digitalRead(globalEnable) == enableLevel))
  ) {
    isEnabled = digitalRead(globalEnable) == enableLevel;

    if (isEnabled) {
      digitalWrite(isEnabledPin, isEnableLevel);
    } else {

      resetMorseBeep();
      resetMorse();
      resetLeds();
      resetYear();
      resetAnthem();
      resetCelebration();
      digitalWrite(isEnabledPin, isEnableLevel == HIGH ? LOW : isEnableLevel);
      digitalWrite(lockPin, lockedLevel);
    }
  }

  if (!isEnabled) {
    delay(100);
    return;
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

  boolean allCorrect = morseCodeCorrect && isYearSequenceCorrect;

  unsigned long result;

  if (allCorrect) {
    playAnthem();
    result = celebration();
    digitalWrite(lockPin, lockedLevel == HIGH ? LOW : HIGH);
  } else {
    result = btn1;
    result = (result << 5) | btn2;
    result = (result << 5) | btn3;
    result = (result << 5) | btn4;
    result = (result << 1) | isYearSequenceCorrect;
    result = (result << 1) | morseCodeCorrect;
    result = (result << 1) | allCorrect;
    result = (result << 1) | 0;

    digitalWrite(lockPin, lockedLevel);

    if (!morseCodeCorrect) {
      resetAnthem();
      morseBeep();
    } else {
      resetMorseBeep();
    }
  }

  if (result != prevResult) {
//    Serial.println("result: "  + String(morseCodeCorrect));
//    Serial.println(result, BIN);
//    Serial.println(result >> 8, BIN);
//    Serial.println(result >> 16, BIN);

    pushData(result, result >> 8, result >> 16);
    prevResult = result;
  }
}
