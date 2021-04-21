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
  int a1 = analogRead(pressure1Pin);
  int p1 = psiFromSensor(a1);

  int a2 = analogRead(pressure2Pin);
  int p2 = psiFromSensor(a2);

  int a3 = analogRead(pressure3Pin);
  int p3 = psiFromSensor(a3);

  Serial.println("Pressure 1: " + (String) p1 + "psi; analog: " + (String) a1);
  Serial.println("Pressure 2: " + (String) p2 + "psi; analog: " + (String) a2);
  Serial.println("Pressure 3: " + (String) p3 + "psi; analog: " + (String) a3);

  digitalWrite(lockPin, p1 == pressure1Target && p2 == pressure2Target && p3 == pressure3Target ? LOW : HIGH);

  delay(100);
}

int psiFromSensor(int analogReading) {
  return analogReading / 13;
}
