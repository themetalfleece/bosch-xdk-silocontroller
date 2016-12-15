const int silo1ledPin = 9;
const int silo2ledPin = 7;
const int silo3ledPin = 19;
const int silo4ledPin = 8;
int flowPins[] = {10, 4, 5, 6};
volatile boolean flowActive = true;
enum direction {left, right} flowDirection;
const int processAPin = 2;
const int processBPin = 3;
volatile int currentLedIndex = -1;
volatile int previousLed = 0;
const int delayTimes = 8;
int typeAsequence[] = {silo1ledPin, flowPins[0], flowPins[1], flowPins[2], flowPins[3], silo4ledPin};
int typeBsequence[] = {silo2ledPin, flowPins[3], flowPins[2], flowPins[1], flowPins[0], silo3ledPin};
const int reversePin = 14;
int reverse = 0;
const int availabilityLedPin = 11;

void setup() {
  Serial.begin(9600);
  pinMode(silo1ledPin, OUTPUT);
  pinMode(silo2ledPin, OUTPUT);
  pinMode(silo3ledPin, OUTPUT);
  pinMode(silo4ledPin, OUTPUT);
  pinMode(flowPins[0], OUTPUT);
  pinMode(flowPins[1], OUTPUT);
  pinMode(flowPins[2], OUTPUT);
  pinMode(flowPins[3], OUTPUT);
  pinMode(processAPin, INPUT_PULLUP);
  pinMode(processBPin, INPUT_PULLUP);
  pinMode(reversePin, INPUT);
  reverse = digitalRead(reversePin);
  if (reverse) {
    for (int i = 0; i < 6 / 2; i++)
    {
      int temp = typeAsequence[i];
      typeAsequence[i] = typeAsequence[6 - 1 - i ];
      typeAsequence[6 - 1 - i] = temp;
    }
    for (int i = 0; i < 6 / 2; i++)
    {
      int temp = typeBsequence[i];
      typeBsequence[i] = typeBsequence[6 - 1 - i ];
      typeBsequence[6 - 1 - i] = temp;
    }
  }
  pinMode(availabilityLedPin, OUTPUT);
  digitalWrite(availabilityLedPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(processAPin), processAisr, LOW);
  attachInterrupt(digitalPinToInterrupt(processBPin), processBisr, LOW);
}

void processAisr() {
  if (flowDirection == left) {
    previousLed = 0;
    currentLedIndex = -1;
  }
  flowDirection = right;
  flow();
}

void processBisr() {
  if (flowDirection == right) {
    previousLed = 0;
    currentLedIndex = -1;
  }
  flowDirection = left;
  flow();
}


void loop() {
  // TODO write and then sleep awaiting interrupt
  digitalWrite(availabilityLedPin, HIGH);
}

void flow() {
  digitalWrite(availabilityLedPin, LOW);
  int i;
  currentLedIndex = (currentLedIndex + 1) % 6;
  if (flowDirection == right) {
    digitalWrite(typeAsequence[previousLed], LOW);
    digitalWrite(typeAsequence[currentLedIndex], HIGH);

    delayMicrosecondsIntervalTimes(delayTimes);
    digitalWrite(typeAsequence[currentLedIndex], LOW);
  }
  else {
    digitalWrite(typeBsequence[previousLed], LOW);
    digitalWrite(typeBsequence[currentLedIndex], HIGH);

    delayMicrosecondsIntervalTimes(delayTimes);
    digitalWrite(typeBsequence[currentLedIndex], LOW);
  }
  previousLed = currentLedIndex;
}

void delayMicrosecondsIntervalTimes(int times) {
  for (int i = 0; i < times; i++)
    delayMicroseconds(16380);
}

