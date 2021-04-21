const int pressure1Pin = A0;
const int pressure2Pin = A1;
const int pressure3Pin = A2;

const int lockPin = 13;

const int pressureResolutionFactor = 10;

const int pressure1Target = 3;
const int pressure2Target = 4;
const int pressure3Target = 6;

void setup() {
  Serial.begin(9600);

  pinMode(lockPin, OUTPUT);
}

void loop() {
  int p1 = psiFromSensor(analogRead(pressure1Pin));
  int p2 = psiFromSensor(analogRead(pressure2Pin));
  int p3 = psiFromSensor(analogRead(pressure3Pin));

  Serial.println("Pressure 1: " + (String) p1);
  Serial.println("Pressure 2: " + (String) p2);
  Serial.println("Pressure 3: " + (String) p3);

  digitalWrite(lockPin, p1 == pressure1Target && p2 == pressure2Target && p3 == pressure3Target ? LOW : HIGH);
}

void int psiFromSensor(int analogReading) {
  return analogReading / 13;
}
