
// the direction of the car's movement
// ENA   ENB   IN1   IN2   IN3   IN4   Description
// HIGH  HIGH  HIGH  LOW   LOW   HIGH  Car is runing forward
// HIGH  HIGH  LOW   HIGH  HIGH  LOW   Car is runing back
// HIGH  HIGH  LOW   HIGH  LOW   HIGH  Car is turning left
// HIGH  HIGH  HIGH  LOW   HIGH  LOW   Car is turning right
// HIGH  HIGH  LOW   LOW   LOW   LOW   Car is stoped
// HIGH  HIGH  HIGH  HIGH  HIGH  HIGH  Car is stoped
// LOW   LOW   N/A   N/A   N/A   N/A   Car is stoped

#include <IRremote.h>

// define L298n module IO Pin
#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11

// infrared signal codes
#define RECV_PIN 12
#define IR_UP 16736925
#define IR_DOWN 16754775
#define IR_LEFT 16720605
#define IR_RIGHT 16761405
#define IR_OK 16712445
#define IR_ONE 16738455

IRrecv irrecv(RECV_PIN);
decode_results irResults;
unsigned long irTimestampMs;

void accelerate(int maxSpeed) {
  for (int i = 100; i <= maxSpeed; i++) {
    analogWrite(ENA, i);
    analogWrite(ENB, i);
    delay(10);
  }
}

void decelerate(int maxSpeed) {
  for (int i = maxSpeed; i >= 100; i--) {
    analogWrite(ENA, i);
    analogWrite(ENB, i);
    delay(10);
  }
}

void throttle() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
}

void hold() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  Serial.println("stop");
}

void forward(bool analog) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  if (analog) {
    accelerate(255);
  }
  else {
    throttle();
  }

  Serial.println("forward");
}

void back(bool analog) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  if (analog) {
    accelerate(255);
  }
  else {
    throttle();
  }

  Serial.println("back");
}

void left(bool analog) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  if (analog) {
    accelerate(150);
  }
  else {
    throttle();
  }

  Serial.println("left");
}

void right(bool analog) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  if (analog) {
    accelerate(150);
  }
  else {
    throttle();
  }

  Serial.println("right");
}

void signaling() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  Serial.println("light");
}

void controlBluetooth() {
  // bluetooth serial port
  char bluetooth = Serial.read();

  switch (bluetooth) {
    case 'f':
      forward(true);
      break;
    case 'b':
      back(true);
      break;
    case 'l':
      left(true);
      break;
    case 'r':
      right(true);
      break;
    case 'h':
      hold();
      break;
    case 's':
      signaling();
      break;
    default:
      break;
  }
}

void controlIr() {
  if (irrecv.decode(&irResults)) {
    irTimestampMs = millis();

    unsigned long irValue = irResults.value;
    Serial.println(irValue);
    irrecv.resume();

    switch (irValue) {
      case IR_UP:
        forward(true);
        break;
      case IR_DOWN:
        back(true);
        break;
      case IR_LEFT:
        left(true);
        break;
      case IR_RIGHT:
        right(true);
        break;
      case IR_OK:
        hold();
        break;
      case IR_ONE:
        signaling();
        break;
      default:
        break;
    }
  }
  else {
    unsigned long pastMs = millis() - irTimestampMs;

    if (pastMs > 5000) {
      hold();
      irTimestampMs = millis();
    }
  }
}

void setup() {
  // open serial and set the baudrate
  Serial.begin(9600);

  // before using io pin, pin mode must be set first
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // start receiving infrared signal
  irrecv.enableIRIn();
}

void loop() {
  controlBluetooth();
  controlIr();
}

