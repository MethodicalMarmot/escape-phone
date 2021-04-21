enum color {
  red = 0,
  blue = 1,
  yellow = 2
};

const color answer[3][2] = {
  {blue, red},
  {blue, yellow},
  {yellow, red}
};

const int colorPins[3][2] = {
  {2, 3},   // red
  {4, 5},   // blue
  {6, 7}    // yellow
};

const int lockPin = 13;

void setup() {
  Serial.begin(9600);
  
  for (int* p : colorPins) {
    pinMode(p[0], INPUT_PULLUP);
    pinMode(p[1], INPUT_PULLUP);
  }

  pinMode(lockPin, OUTPUT);
}

void loop() {
  boolean correct = true;
  for (color* c : answer) {
    correct = checkConnected(c) && correct;
  }

  digitalWrite(lockPin, correct ? LOW : HIGH);

  delay(100);
}

boolean checkConnected(color colors[2]) {
  int* onePins = colorPins[colors[0]];
  int* twoPins = colorPins[colors[1]];
  bool result = false;

  for (int i = 0; i < 2 && !result; i++) {
    pinMode(onePins[i], OUTPUT);
    digitalWrite(onePins[i], LOW);
    for (int j = 0; j < 2 && !result; j++) {
      result = !digitalRead(twoPins[j]);
    }

    pinMode(onePins[i], INPUT_PULLUP);
  }
  
  return result;
}
