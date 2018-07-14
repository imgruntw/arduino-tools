#include <IRremote.h>
#include <Servo.h>

// line tracking IO
#define LT_R digitalRead(10)
#define LT_M digitalRead(4)
#define LT_L digitalRead(2)

// define L298n module IO Pin
// ENA   ENB   IN1   IN2   IN3   IN4   Description
// HIGH  HIGH  HIGH  LOW   LOW   HIGH  Car is runing forward
// HIGH  HIGH  LOW   HIGH  HIGH  LOW   Car is runing back
// HIGH  HIGH  LOW   HIGH  LOW   HIGH  Car is turning left
// HIGH  HIGH  HIGH  LOW   HIGH  LOW   Car is turning right
// HIGH  HIGH  LOW   LOW   LOW   LOW   Car is stoped
// HIGH  HIGH  HIGH  HIGH  HIGH  HIGH  Car is stoped
// LOW   LOW   N/A   N/A   N/A   N/A   Car is stoped
const byte ENA = 5;
const byte ENB = 6;
const byte IN1 = 7;
const byte IN2 = 8;
const byte IN3 = 9;
const byte IN4 = 11;

// infrared signal codes
const byte RECV_PIN = 12;
const long IR_UP = 16736925;
const long IR_DOWN = 16754775;
const long IR_LEFT = 16720605;
const long IR_RIGHT = 16761405;
const long IR_OK = 16712445;
const long IR_ONE = 16738455;
const long IR_TWO = 16750695;

// infrared signal
IRrecv irrecv(RECV_PIN);
decode_results irResults;

// servo and obstacle detection
const int ECHO = A4;
const int TRIG = A5;
const int MIN_OBSTACLE_DISTANCE = 20;
int rightDistance = 0;
int leftDistance = 0;
int middleDistance = 0;
bool servoActive = false;
Servo obstacleServo;

// control
const char FRW = 'f';
const char BCK = 'b';
const char LFT = 'l';
const char RGH = 'r';
const char HLD = 'h';
const char LED = 's';

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
  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(LT_R, INPUT);
  pinMode(LT_M, INPUT);
  pinMode(LT_L, INPUT);

  // start receiving infrared signal
  irrecv.enableIRIn();

  // servo activate
  if (servoActive) {
    obstacleServo.attach(3);
    obstacleServo.write(90);
  }
}

void loop() {
  char bluetoothCommand = readBluetooth();
  char irCommand = readIr();
  bool forwardCommand = FRW == bluetoothCommand || FRW == irCommand;
  bool backCommand = BCK == bluetoothCommand || BCK == irCommand;
  bool leftCommand = LFT == bluetoothCommand || LFT == irCommand;
  bool rightCommand = RGH == bluetoothCommand || RGH == irCommand;
  bool holdCommand = HLD == bluetoothCommand || HLD == irCommand;
  bool ledCommand = LED == bluetoothCommand || LED == irCommand;
  bool obstacle = measureDistance() <= MIN_OBSTACLE_DISTANCE;

  if (forwardCommand && !obstacle) {
    forward();
  }
  else if (backCommand) {
    back();
  }
  else if (leftCommand) {
    left();
  }
  else if (rightCommand) {
    right();
  }
  else if (holdCommand) {
    hold();
  }
  else if (ledCommand) {
    toggleLed();
  }

  if (obstacle) {
    hold();
  }

  if (LT_M || LT_R || LT_L) {
    hold();
  }
}

char readBluetooth() {
  // bluetooth serial port
  return Serial.read();
}

char readIr() {
  char result;

  if (irrecv.decode(&irResults)) {
    unsigned long irValue = irResults.value;
    irrecv.resume();

    switch (irValue) {
      case IR_UP:
        return FRW;
      case IR_DOWN:
        return BCK;
      case IR_LEFT:
        return LFT;
      case IR_RIGHT:
        return RGH;
      case IR_OK:
        return HLD;
      case IR_ONE:
        return LED;
      default:
        Serial.println(irValue);
        break;
    }
  }

  return result;
}

void slowAccelerate() {
  for (int i = 50; i <= 255; i++) {
    analogWrite(ENA, i);
    analogWrite(ENB, i);
    delay(10);
  }
}

void fastAccelerate() {
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
}

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  slowAccelerate();
}

void back() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  slowAccelerate();
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  fastAccelerate();
  delay(250);
  hold();
}

void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  fastAccelerate();
  delay(250);
  hold();
}

void toggleLed() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

int measureDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(20);
  digitalWrite(TRIG, LOW);

  float distance = pulseIn(ECHO, HIGH);
  distance = distance / 58;

  return (int) distance;
}

