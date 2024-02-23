#include <Wire.h>
#include <MPU6050.h>
#include <BleMouse.h>

BleMouse bleMouse("Universal Gesture Controlled Mouse", "Espressif", 100);

MPU6050 mpu;

int16_t gx, gy, gz; // Gyro variables
int vx, vy;         // Pointer displacement

const int thumbPin = 25;
const int indexPin = 33;
int valThumb, valIndex; // Flex sensor output values
int sense = 250;        // Pointer sensitivity value
int buffThumb = 1900;
int buffIndex = 2100;

bool flagThumb = false;
bool flagIndex = false;
bool flagDrag = false;

int rate = 20; // Refresh rate

void setup() {
  Wire.begin();
  mpu.initialize();
  bleMouse.begin();
  Serial.begin(115200); // Initialize serial communication
}

void loop() {
  moveMouse();
  clickMouse();
  printValues();
  delay(rate);
}

void moveMouse() {
  if (!flagThumb && !flagIndex) {
    mpu.getRotation(&gx, &gy, &gz);

    // Convert gyro output values to pointer displacement
    vx = -(gx + 470) / sense;
    vy = -(gz + 20) / sense;

    bleMouse.move(vx, vy);
  } else if (flagDrag) {
    mpu.getRotation(&gx, &gy, &gz);
    vx = -(gx + 470) / sense;
    vy = -(gz + 20) / sense;
    bleMouse.press(MOUSE_LEFT);
    valThumb = analogRead(thumbPin);
    if (valThumb < buffThumb) {
      flagDrag = false;
      bleMouse.release(MOUSE_LEFT);
    }
    bleMouse.move(vx, vy);
  }
}

void clickMouse() {
  valThumb = analogRead(thumbPin);
  valIndex = analogRead(indexPin);
  
  clickThumb();
  clickIndex();
}

void clickThumb() {
  if (!flagThumb) {
    if (valThumb > buffThumb) {
      flagDrag = true;
      flagThumb = true;
      bleMouse.press(MOUSE_LEFT);
      bleMouse.release(MOUSE_LEFT);
    }
  } else {
    if (valThumb < buffThumb) {
      flagThumb = false;
    }
  }
}

void clickIndex() {
  if (!flagIndex) {
    if (valIndex > buffIndex) {
      flagIndex = true;
      bleMouse.press(MOUSE_RIGHT);
      bleMouse.release(MOUSE_RIGHT);
    }
  } else {
    if (valIndex < buffIndex) {
      flagIndex = false;
    }
  }
}

void printValues() {
  Serial.print("\nGyroscope values - gx=");
  Serial.print(gx);
  Serial.print(" gy=");
  Serial.print(gy);
  Serial.print(" gz=");
  Serial.print(gz);
  Serial.print("\n");

  Serial.print("X and Y coordinate values - vx=");
  Serial.print(vx);
  Serial.print(" vy=");
  Serial.print(vy);
  Serial.print("\n");

  Serial.print("Flex Sensor Bend Values - valThumb=");
  Serial.print(valThumb);
  Serial.print(" valIndex=");
  Serial.print(valIndex);
  Serial.print("\n");
}
