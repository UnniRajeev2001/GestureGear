#include <Wire.h>
#include <Mouse.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>
#include <IRremote.h>
#include <Servo.h>
#include <LiquidCrystal.h>

#define FIRE_PIN A0
#define IR_RECEIVER_PIN 11
#define PIEZO_PIN 9
#define TARGET_DELAY 2000

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int buttonPin = 6;
const int ledPin = 7;
int buttonState = 0;
int score = 0;

void setup() {
  lcd.begin(16, 2);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  lcd.print("Press to start");
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Game started!");
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.print(score);
    delay(500);
    while (true) {
      lcd.setCursor(0, 1);
      lcd.print("Score: ");
      lcd.print(score);
      if (digitalRead(buttonPin) == LOW) {
        lcd.clear();
        lcd.print("Game Over!");
        delay(1000);
        lcd.clear();
        lcd.print("Your score: ");
        lcd.print(score);
        score = 0;
        delay(2000);
        lcd.clear();
        lcd.print("Press to start");
        break;
      }
      else {
        score++;
        digitalWrite(ledPin, HIGH);
        delay(500);
        digitalWrite(ledPin, LOW);
        delay(500);
      }
    }
  }
}

Servo servo;
LiquidCrystal lcd(12, 10, 5, 4, 3, 2);

int targetAngles[] = {30, 60, 90, 120, 150};
int currentTarget = 0;
unsigned long lastTargetChange = 0;

IRrecv irrecv(IR_RECEIVER_PIN);
decode_results results;

void setup() {
  pinMode(FIRE_PIN, INPUT);
  servo.attach(6);
  lcd.begin(16, 2);
  irrecv.enableIRIn();
  randomSeed(analogRead(0));
  lcd.print("Press any button");
}

void loop() {
  if (irrecv.decode(&results)) {
    int value = results.value;
    switch(value) {
      case 0xFFA25D:  // Power
        resetGame();
        break;
      case 0xFF629D:  // Up arrow
        moveTarget(1);
        break;
      case 0xFFE21D:  // Down arrow
        moveTarget(-1);
        break;
      case 0xFF22DD:  // Play/pause
        fire();
        break;
    }
    irrecv.resume();
  }

  if (millis() - lastTargetChange > TARGET_DELAY) {
    moveTarget(random(-2, 3));
    lastTargetChange = millis();
  }
}

void resetGame() {
  currentTarget = 0;
  lcd.clear();
  lcd.print("Game Reset");
  delay(1000);
}

void moveTarget(int direction) {
  currentTarget = constrain(currentTarget + direction, 0, 4);
  int angle = targetAngles[currentTarget];
  servo.write(angle);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Target Angle:");
  lcd.setCursor(0, 1);
  lcd.print(angle);
}

void fire() {
  int sensorValue = analogRead(FIRE_PIN);
  if (sensorValue > 500) {
    tone(PIEZO_PIN, 1000, 200);
    lcd.clear();
    lcd.print("Hit!");
    delay(1000);
  } else {
    tone(PIEZO_PIN, 500, 200);
    lcd.clear();
    lcd.print("Missed!");
    delay(1000);
  }
}


#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int buttonPin = 6;
int buttonState = 0;
int lastButtonState = 0;
int score = 0;

void setup() {
  lcd.begin(16, 2);
  pinMode(buttonPin, INPUT);
  lcd.print("Press to start");
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH && lastButtonState == LOW) {
    lcd.clear();
    lcd.print("Game started!");
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.print(score);
    delay(1000);
    lcd.clear();
    lcd.print("Press button to");
    lcd.setCursor(0, 1);
    lcd.print("increase score");
    while (true) {
      buttonState = digitalRead(buttonPin);
      if (buttonState == HIGH && lastButtonState == LOW) {
        score++;
        lcd.clear();
        lcd.print("Score: ");
        lcd.print(score);
        delay(500);
      }
      lastButtonState = buttonState;
    }
  }
  lastButtonState = buttonState;
}


MPU6050 mpu;

SoftwareSerial bluetooth(8, 9); 
// SoftwareSerial for HC-05 (RX, TX)

int16_t ax, ay, az;
int flexPin = A0; 
// Flex sensor connected to analog pin A0 (Left click)

int flexValue = 0; 
// Variable to store the flex sensor value

int clickThreshold = 500; 
// Threshold for flex sensor value to simulate click

bool isClicking = false; 
// Flag to track the click state

unsigned long lastClickTime = 0; 
// Variable to store the last click time

unsigned long debounceDelay = 50; 
// Debounce time in milliseconds

void setup() {
  Wire.begin();
  Serial.begin(9600);
  bluetooth.begin(9600);
  
  mpu.initialize();
  
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while (1);
  }
  
  Mouse.begin();
  pinMode(flexPin, INPUT); 
  // Set flex sensor pin as input
}

void loop() {
  mpu.getAcceleration(&ax, &ay, &az);
  
  // Map the sensor data to cursor movements
  int sensitivity = 10; // Adjust sensitivity as needed
  
  int dx = map(ax, -17000, 17000, -sensitivity, sensitivity); 
  // Map ax to cursor X movement

  int dy = map(ay, -17000, 17000, -sensitivity, sensitivity); 
  // Map ay to cursor Y movement
  
  Mouse.move(dx, dy, 0); 
  // Move the mouse cursor
  
  // Read the flex sensor value
  flexValue = analogRead(flexPin);
  
  // Check if the flex sensor is bent beyond the threshold for left click
  
  if (flexValue > clickThreshold) {
    if (!isClicking && millis() - lastClickTime > debounceDelay) {
      isClicking = true;
      Mouse.press(MOUSE_LEFT); 
      // Simulate left mouse button press
      lastClickTime = millis(); 
      // Update last click time
    }
  } else {
    if (isClicking) {
      isClicking = false;
      Mouse.release(MOUSE_LEFT); 
      // Release the left mouse button
    }
  }
  
  // Send sensor data via Bluetooth
  bluetooth.print("X:");
  bluetooth.print(ax);
  bluetooth.print(" Y:");
  bluetooth.print(ay);
  bluetooth.print(" Z:");
  bluetooth.println(az);
  
  // Print sensor data and flex sensor value to monitor
  Serial.print("X acceleration: ");
  Serial.println(ax);
  Serial.print("Y acceleration: ");
  Serial.println(ay);
  Serial.print("Z acceleration: ");
  Serial.println(az);
  Serial.print("Flex sensor value: ");
  Serial.println(flexValue);
  
  delay(10); 
  // Delay to control the update rate
}
