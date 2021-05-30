#include <Keypad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

const String unlockCode = "12345";
const long codeCheckTimeout = 2000L;                          // ms
const long initialDisplayTimeSec = 72L * 60L * 60L;           // 72 hours
const long initialRealTimeSec = 60L * 60L;                    // 1 hour
const int initialMultiplierIndex = 3;
const int timeMultiplier[initialMultiplierIndex] = {1, 10, 40};

const byte hourI2C = 0x71;
const byte minSecI2C = 0x70;

const byte lockPin = 13;
const int lockedState = HIGH;

const byte numRows = 4; //number of rows on the keypad
const byte numCols = 3; //number of columns on the keypad

//keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[numRows][numCols] =
{
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {9, 8, 7, 6}; //Rows 0 to 3
byte colPins[numCols] = {5, 4, 3}; //Columns 0 to 3

//initializes an instance of the Keypad class
Keypad digitpad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);


Adafruit_7segment hourDisplay = Adafruit_7segment();
Adafruit_7segment minSecDisplay = Adafruit_7segment();

void setup()
{
  Serial.begin(9600);
  pinMode(lockPin, OUTPUT);
  digitalWrite(lockPin, lockedState);

  hourDisplay.begin(hourI2C);
  minSecDisplay.begin(minSecI2C);
}

unsigned long lastTimeChanged = millis();
unsigned long lastDisplayTimeChanged = millis();
unsigned long lastKeyPressed = millis();
long displayTimeSec = initialDisplayTimeSec;
long realTimeSec = initialRealTimeSec;
String currentCode = "";
int displayTimeInterval = 1000;
int currentMultiplierIndex = initialMultiplierIndex;
boolean unlocked = false;
void loop()
{
  unsigned long currentMillis = millis();
  char digit = digitpad.getKey();

  if (digit != NO_KEY) {
    if (unlocked) {
      Serial.println("LOCKED");
      reset();
      return;
    }

    Serial.println(digit);
    currentCode.concat(digit);
    lastKeyPressed = currentMillis;

    Serial.println("currentCode: " + currentCode);
  }

  if (currentCode != "" && currentMillis - lastKeyPressed > codeCheckTimeout) {
    unlocked = unlockCode == currentCode;
    currentCode = "";

    if (!unlocked && currentMultiplierIndex > 0) {
      currentMultiplierIndex--;
      displayTimeInterval = timeMultiplier[currentMultiplierIndex] * int(1000 * realTimeSec / displayTimeSec);
      Serial.println("displayTimeInterval: " + String(displayTimeInterval));
      Serial.println("timeMultiplier: " + String(timeMultiplier[currentMultiplierIndex]));
    }
    else if (unlocked) {
      digitalWrite(lockPin, lockedState == HIGH ? LOW : HIGH);
      Serial.println("UNLOCKED!");
    }
  }

  if (!unlocked && currentMillis - lastDisplayTimeChanged > displayTimeInterval) {
    long hour = displayTimeSec / 60 / 60;
    long mins = (displayTimeSec - (hour * 60 * 60)) / 60;
    long sec = (displayTimeSec - (hour * 60 * 60 + mins * 60));
    display(hour, mins, sec);
    displayTimeSec -= 1;
    lastDisplayTimeChanged = currentMillis;
  }

  if (currentMillis - lastTimeChanged > 1000) {
    realTimeSec -= 1;
    lastTimeChanged = currentMillis;
  }
}

void reset() {
  unlocked = false;
  currentCode = "";
  displayTimeSec = initialDisplayTimeSec;
  realTimeSec = initialRealTimeSec;
  displayTimeInterval = 1000;
  currentMultiplierIndex = initialMultiplierIndex;
  digitalWrite(lockPin, lockedState);
}

void display(long hour, long mins, long sec) {
  Serial.print(padZero(hour));
  Serial.print(":");
  Serial.print(padZero(mins));
  Serial.print(":");
  Serial.println(padZero(sec));

  hourDisplay.print(hour);      //TODO pad with zero

  minSecDisplay.print(mins*100 + sec);
  if (mins < 10) {
    minSecDisplay.writeDigitNum(1, 0);
  }
  if (sec < 10) {
    minSecDisplay.writeDigitNum(2, 0);
  }

  hourDisplay.writeDisplay();
  minSecDisplay.writeDisplay();
}

String padZero(long value) {
  return value < 10 ? "0" + String(value) : String(value);
}
