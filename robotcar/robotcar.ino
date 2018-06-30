
//    The direction of the car's movement
//  ENA   ENB   IN1   IN2   IN3   IN4   Description
//  HIGH  HIGH  HIGH  LOW   LOW   HIGH  Car is runing forward
//  HIGH  HIGH  LOW   HIGH  HIGH  LOW   Car is runing back
//  HIGH  HIGH  LOW   HIGH  LOW   HIGH  Car is turning left
//  HIGH  HIGH  HIGH  LOW   HIGH  LOW   Car is turning right
//  HIGH  HIGH  LOW   LOW   LOW   LOW   Car is stoped
//  HIGH  HIGH  HIGH  HIGH  HIGH  HIGH  Car is stoped
//  LOW   LOW   N/A   N/A   N/A   N/A   Car is stoped

//define L298n module IO Pin
#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11

void accelerate() {
  for (int i = 100; i <= 255; i++) {
    analogWrite(ENA, i);
    analogWrite(ENB, i);
    delay(10);
  }
}

void decelerate() {
  for (int i = 255; i >= 100; i--) {
    analogWrite(ENA, i);
    analogWrite(ENB, i);
    delay(10);
  }
}

void throttle() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  delay(2000);
}

void hold() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(3000);
  Serial.println("stop");
}

void forward(bool analog) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  if (analog) {
    accelerate();
    decelerate();
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
    accelerate();
    decelerate();
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
    accelerate();
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
    accelerate();
  }
  else {
    throttle();
  }

  Serial.println("right");
}

void light() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  Serial.println("light");
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
}

void loop() {
  hold();
  forward(true);

  hold();
  back(false);

  hold();
  left(true);

  hold();
  right(true);

  light();
}

