const int pressure1Pin = A0;
const int pressure2Pin = A1;
const int pressure3Pin = A2;

const int lockPin = 13;

const int pressure1Target = 2;
const int pressure2Target = 4;
const int pressure3Target = 9;

const int analogPsi[] = {
//  analog     psi
    108,    // 1
    117,    // 2
    126,    // 3
    135,    // 4
    143,    // 5
    151,    // 6
    160,    // 7
    169,    // 8
    177,    // 9
    185,    // 10
    194,    // 11
    203,    // 12
    212,    // 13
    221,    // 14
};

void setup() {
  Serial.begin(9600);

  pinMode(lockPin, OUTPUT);
}

void loop() {
  int a1 = analogRead(pressure1Pin);
  int p1 = psiFromAnalog(a1);

  int a2 = analogRead(pressure2Pin);
  int p2 = psiFromAnalog(a2);

  int a3 = analogRead(pressure3Pin);
  int p3 = psiFromAnalog(a3);

  Serial.println("Pressure 1: " + (String) p1 + "psi; analog: " + (String) a1);
  Serial.println("Pressure 2: " + (String) p2 + "psi; analog: " + (String) a2);
  Serial.println("Pressure 3: " + (String) p3 + "psi; analog: " + (String) a3);

  digitalWrite(lockPin, p1 == pressure1Target && p2 == pressure2Target && p3 == pressure3Target ? LOW : HIGH);

  delay(400);
}

int psiFromAnalog(int analogReading) {
  int i = 0;
  for (; i < (sizeof(analogPsi) / sizeof(analogPsi[0])) && analogPsi[i] <= analogReading; i++);
  return i;
}
