#include <LiquidCrystal.h>

// Pin definitions
const int pressurePin = A0;   // Analog pin for pressure sensor
const int buttonPin   = 8;    // Digital pin for CPR button
const int pressGood   = 10;   // Green LED (Good pressure)
const int pressBad    = 9;    // Red LED (Bad pressure)

// Variables
int pressureRead;
int buttonState = 0;
int lastButtonState = 0;
int cprCount = 0;
int cprCycle = 0;
int BPM = 0;

float bpmCount;
float totalTime;

unsigned long timeBegin, timeEnd;

// LCD configuration (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 2, 3, 4, 5);

void setup() {
  // Pin modes
  pinMode(buttonPin, INPUT);
  pinMode(pressGood, OUTPUT);
  pinMode(pressBad, OUTPUT);

  // Initialize Serial & LCD
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.home();
  lcd.print("Begin CPR");
}

void loop() {
  pressureRead = analogRead(pressurePin);
  buttonState = digitalRead(buttonPin);

  // Check button press
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      cprCount++;
      lcd.clear();
      lcd.print("Count: ");
      lcd.print(cprCount);

      // Record start time on first compression
      if (cprCount == 1) {
        timeBegin = millis();
      }
    }
  }

  // LED feedback (good vs bad pressure)
  if (pressureRead < 500) {
    digitalWrite(pressGood, HIGH);
    digitalWrite(pressBad, LOW);
  } else {
    digitalWrite(pressBad, HIGH);
    digitalWrite(pressGood, LOW);
  }

  // After 30 compressions → calculate BPM
  if (cprCount == 30) {
    cprCount = 0;
    timeEnd = millis();
    totalTime = (timeEnd - timeBegin) / 1000.0;
    bpmCount = 30.0 / totalTime;
    BPM = bpmCount * 60;

    lcd.clear();
    lcd.print("Rate: ");
    lcd.print(BPM);
    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    lcd.print(totalTime);

    cprCycle++;

    // Reset after 5 cycles
    if (cprCycle == 5) {
      cprCycle = 0;
    }
  }

  // Real-time feedback (rate check)
  if (cprCount > 5 && cprCount <= 30) {
    float rateTime = (millis() - timeBegin) / 1000.0;
    int rate = (cprCount / rateTime) * 60;
    lcd.setCursor(12, 0);

    if (rate >= 100 && rate <= 120) {
      lcd.print("Good");
    } else {
      lcd.print("----");
    }
  }

  delay(25);
  lastButtonState = buttonState;
}
