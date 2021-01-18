#define ARRAY_SIZE(array) ((sizeof(array))/(sizeof(array[0])))

// settings
const int debounceTimeout = 50;
const int morseTone = 450;
const int morseDotDuration = 100;
const int morseDashDuration = 3 * morseDotDuration;
const String morseMessage = ". . .   - - -   . . .         ";
const byte yearLedsSequence[6] = {
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
  
  pushData(B00000000, B00000000, B00000000);

  strippedMessage.replace(" ", "");

  Serial.println("INIT");
}

int lastValueBtn[5] = {0, 0, 0, 0, 0};
unsigned long prevValueBtnMillis[5] = {0, 0, 0, 0, 0};

int btnLedIdx[4] = {0, 0, 0, 0};

unsigned long prevResult = 0;
void loop() {
  morseBeep();
  
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

  unsigned long result = btn1;
  result = (result << 5) | btn2;
  result = (result << 5) | btn3;
  result = (result << 5) | btn4;
  result = (result << 1) | isYearSequenceCorrect;
  result = (result << 1) | morseCodeCorrect;
  result = (result << 1) | (morseCodeCorrect && isYearSequenceCorrect ? 1 : 0);
  result = (result << 1) | 0;

  if (result != prevResult) {
    //Serial.println("result: ");
    //Serial.println(result, BIN);
    //Serial.println(result >> 8, BIN);
    //Serial.println(result >> 16, BIN);

    pushData(result, result >> 8, result >> 16);
    prevResult = result;
  }
}
unsigned long last = millis();
boolean prevPressed = false;
int nextMorseIdx = 0;
boolean readMorseCode() {
  unsigned long current = millis();
  unsigned long gap = current - last;
  boolean pressed = isBtnPressed(idxMorseBtn, true);

  if (morseMessage.length() == nextMorseIdx) {
    nextMorseIdx = 0;
  }

  if (pressed && !prevPressed) {
      prevPressed = true;
      last = current;
  } else if (!pressed && prevPressed) {
      prevPressed = false;
      last = current;
      if (gap > morseDotDuration) {
          char sym = (gap > morseDashDuration ? '-' : '.');
          if (strippedMessage[nextMorseIdx] == sym) {
            nextMorseIdx++;
          } else {
            nextMorseIdx = 0;
          }
          Serial.println("sym: " + String(sym) +";" + String(nextMorseIdx));
      }
  }

  return strippedMessage.length() == nextMorseIdx;
}

byte readBtnLedSequence(byte idx) {
  if (isBtnPressed(idx, false)) {
    btnLedIdx[idx - 1] = ARRAY_SIZE(yearLedsSequence) - 1 == btnLedIdx[idx - 1] ? 0 : btnLedIdx[idx - 1] + 1;
    
    //Serial.print("btn " + String(idx) + "; value idx: " + btnLedIdx[idx - 1] + "; output: ");
    //Serial.println(yearLedsSequence[btnLedIdx[idx - 1]], BIN);
  }
  return yearLedsSequence[btnLedIdx[idx - 1]];
}

int nextCharIdx = 0;
unsigned long prevBeep = millis();
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
