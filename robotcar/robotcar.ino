
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
#include <Servo.h>

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
#define IR_TWO 16750695

IRrecv irrecv(RECV_PIN);
decode_results irResults;
unsigned long irTimestampMs;

Servo obstacleServo;
int echo = A4;
int trig = A5;
int rightDistance = 0;
int leftDistance = 0;
int middleDistance = 0;
bool servoActive = false;
int maxDistance = 30;

void accelerate() {
  for (int i = 50; i <= 255; i++) {
    analogWrite(ENA, i);
    analogWrite(ENB, i);
    delay(10);
  }
}

void decelerate() {
  for (int i = 255; i >= 0; i--) {
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
    accelerate();
  }
  else {
    throttle();
  }

  Serial.println("forward");

  avoidObstacle();
}

void back(bool analog) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  if (analog) {
    accelerate();
  }
  else {
    throttle();
  }

  Serial.println("back");
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  throttle();

  Serial.println("left");
}

void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  throttle();

  Serial.println("right");
}

void signaling() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  Serial.println("light");
}

int measureDistance() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(20);
  digitalWrite(trig, LOW);

  float distance = pulseIn(echo, HIGH);
  distance = distance / 58;

  return (int) distance;
}

void obstacleAutoDrive() {
  if (servoActive) {
    obstacleServo.attach(3);
    obstacleServo.write(90);
    middleDistance = measureDistance();

    if (middleDistance <= maxDistance) {
      hold();
      delay(500);
      obstacleServo.write(10);
      delay(1000);
      rightDistance = measureDistance();

      delay(500);
      obstacleServo.write(90);
      delay(1000);
      obstacleServo.write(180);
      delay(1000);
      leftDistance = measureDistance();

      delay(500);
      obstacleServo.write(90);
      delay(1000);

      if (rightDistance > leftDistance) {
        right();
        delay(200);
      }
      else if (rightDistance < leftDistance) {
        left();
        delay(200);
      }
      else if ((rightDistance <= maxDistance) || (leftDistance <= maxDistance)) {
        back(false);
        delay(180);
      }
      else {
        forward(false);
      }
    }
    else {
      forward(false);
    }
  }
}

void avoidObstacle() {
  int distance = measureDistance();

  if (distance <= 10) {
    hold();
  }
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
      left();
      break;
    case 'r':
      right();
      break;
    case 'h':
      hold();
      break;
    case 's':
      signaling();
      break;
    case 'o':
      servoActive = !servoActive;
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
        left();
        break;
      case IR_RIGHT:
        right();
        break;
      case IR_OK:
        hold();
        break;
      case IR_ONE:
        signaling();
        break;
      case IR_TWO:
        servoActive = !servoActive;
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

  // servo init
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);

  // start receiving infrared signal
  irrecv.enableIRIn();
}

void loop() {
  controlBluetooth();
  controlIr();
  obstacleAutoDrive();
}

