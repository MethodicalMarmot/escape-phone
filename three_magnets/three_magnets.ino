const int triggerDelay = 500;

const int mag1 = 2;
const int mag2 = 3;
const int mag3 = 4;

const int lock = LED_BUILTIN;

const int magTriggerLevel = LOW;
const int unlockLevel = HIGH;

const int sequence[3] = {
  mag2, mag1, mag3
};
const int sequenceSize = sizeof(sequence) / sizeof(sequence[0]);

void setup() {
  Serial.begin(9600);

  pinMode(mag1, INPUT_PULLUP);
  pinMode(mag2, INPUT_PULLUP);
  pinMode(mag3, INPUT_PULLUP);

  pinMode(lock, OUTPUT);
  digitalWrite(lock, unlockLevel ? LOW : HIGH);
}

int currentIdx = 0;
void loop() {
  for (int i = 0; i < sequenceSize; i++) {
    boolean currentState = digitalRead(sequence[i]) == magTriggerLevel;
    if (!currentState) {
      delay(100);
      continue;
    }
    Serial.println("1: currentIdx: " + String(currentIdx) + "; currentState: " + String(currentState));
    delay(triggerDelay);
    currentState = digitalRead(sequence[i]) == magTriggerLevel;
    Serial.println("2: currentIdx: " + String(currentIdx) + "; currentState: " + String(currentState));

    if (i == currentIdx && currentState) {
      if (currentIdx == sequenceSize - 1) {
        Serial.println("Unlock!");
        digitalWrite(lock, unlockLevel);
      }
      while (digitalRead(sequence[i]) == magTriggerLevel) delay(100);
      digitalWrite(lock, unlockLevel ? LOW : HIGH);
      currentIdx++;
      Serial.println("3: currentIdx: " + String(currentIdx) + "; sequenceSize: " + String(sequenceSize));
      break;
    } else if (i != currentIdx && currentState) {
      currentIdx = 0;
      break;
    }
  }

  if (currentIdx >= sequenceSize) {
    currentIdx = 0;
  }
}
