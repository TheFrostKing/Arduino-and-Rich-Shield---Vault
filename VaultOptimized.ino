#include <DHT11.h>
#include <Display.h>
#include <TM1637Display.h>

//DECLARING PINS AND CONST
const int BUZZER = 3;
const int RED_LED = 4;
const int GREEN_LED = 5;
const int BLUE_LED = 6;
const int YELLOW_LED = 7;
const int RIGHT_BUTTON = 8;
const int LEFT_BUTTON = 9;
const int LDR = 16;
const int UPPER_TH = 300; // THIS VALUES MAY NEED TO BE CHANGED DEPENDING ON THE LIGHT CONDITIONS IN THE BUILDING
const int LOWER_TH = 200; //THIS VALUES MAY NEED TO BE CHANGED DEPENDING ON THE LIGHT CONDITIONS IN THE BUILDING
const String SERIAL_NUMBER = "3121";

//STATES THAT CHANGE
int inputLastState = HIGH;
int confirmLastState = HIGH;

//VARIABLES
int countPositions;
int inputNumber = 1;
int password = 1234;
int countInputButton;
int storePin;
int countFailedLogins;

//BOOL VARIABLES
bool isDoorLocked = true; //the safe starts locked
bool isDoorOpen = false;
bool resetSettingsAfterUnlock = false; // I use this bool to close the door after the safe was unlocked with correct pin. After I close the door the dock locks which makes the safe very safe :)

void setup()
{
  Serial.begin(9600);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(LDR, INPUT);
  Display.show("----");
}

void loop()
{
  digitalWrite(RED_LED, LOW);
  const int INPUT_NUMBERS = digitalRead(LEFT_BUTTON);
  const int CONFIRM_BUTTON = digitalRead(RIGHT_BUTTON);
  int ldrValue = analogRead(LDR);

  if (ldrValue > UPPER_TH)    //applying hysteresis, using two tresholds to remove oscillation
  {
    isDoorOpen = true;
  }
  if (ldrValue < LOWER_TH)
  {
    digitalWrite(GREEN_LED, LOW);
    isDoorOpen = false;
  }
  if (isDoorOpen == true && countPositions != 4 )
  {
    Display.clear();
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER, 65, 2000);
    delay(1000);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);
    delay(1000);
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER, 65, 2000);
    delay(1000);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);
    delay(1000);
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER, 65, 2000);
    delay(1000);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);
    Display.show("----");
    countPositions = 0;
  }
  if (INPUT_NUMBERS != inputLastState)
  {
    if (INPUT_NUMBERS == LOW)
    {
      countInputButton++;
      tone(BUZZER, 20, 405);
      inputNumber++;
      inputNumber = inputNumber % 6;
      if (inputNumber == 0)
      {
        inputNumber = 1;
      }
    }
    delay(70);
    inputLastState = INPUT_NUMBERS;
  }
  if (CONFIRM_BUTTON != confirmLastState)
  {
    if (confirmLastState == LOW)
    {
      tone(BUZZER, 20, 405);
      if (countPositions == 0)
      {
        isDoorOpen = false;
        storePin = 0;
        storePin += inputNumber * 1000;
      }
      switch (countPositions)
      {
        case 0:
          isDoorOpen = false;
          storePin = 0;
          storePin += inputNumber * 1000;
          break;
        case 1:
          storePin += inputNumber * 100;
          break;
        case 2:
          storePin += inputNumber * 10;
          break;
        case 3:
          storePin += inputNumber * 1;
          break;
      }
      Serial.println(storePin);
      inputNumber = 1;
      countPositions++;
      countInputButton = 0;
    }
    delay(70);
    confirmLastState = CONFIRM_BUTTON;
  }
  Display.showCharAt(countPositions, '0' + inputNumber);
  if (countPositions == 4)
  {
    if (storePin == password)
    {
      digitalWrite(GREEN_LED, HIGH);
      if (isDoorLocked == true)
      {
        isDoorLocked = false;
      }
      if (isDoorOpen == true && isDoorLocked == false)
      {
        Display.showCharAt(0, 'O');
        Display.showCharAt(1, 'P');
        Display.showCharAt(2, 'E');
        Display.showCharAt(3, 'N');
        Serial.println("door is open");
        delay(150);
        resetSettingsAfterUnlock = true;
      }
      if (isDoorOpen == false && isDoorLocked == false && resetSettingsAfterUnlock == true)
      {
        Serial.println("safe is closed");
        tone(BUZZER, 100, 2000);
        delay(100);
        noTone(BUZZER);
        isDoorLocked = true;
        digitalWrite(YELLOW_LED, LOW);
        Display.show("----");
        countPositions = 0;
        resetSettingsAfterUnlock = false;
      }
    }
    else if (storePin != password)
    {
      countFailedLogins++;
      if (countFailedLogins == 3)
      {
        Display.clear();
        Serial.println("ALARM SAFE" + SERIAL_NUMBER);
        digitalWrite(RED_LED, HIGH);
        tone(BUZZER, 65, 2000);
        delay(1000);
        Serial.println("ALARM SAFE" + SERIAL_NUMBER);
        digitalWrite(RED_LED, LOW);
        noTone(BUZZER);
        delay(1000);
        Serial.println("ALARM SAFE" + SERIAL_NUMBER);
        digitalWrite(RED_LED, HIGH);
        tone(BUZZER, 65, 2000);
        delay(1000);
        Serial.println("ALARM SAFE" + SERIAL_NUMBER);
        digitalWrite(RED_LED, LOW);
        noTone(BUZZER);
        delay(1000);
        Serial.println("ALARM SAFE" + SERIAL_NUMBER);
        digitalWrite(RED_LED, HIGH);
        tone(BUZZER, 65, 2000);
        delay(1000);
        Serial.println("ALARM SAFE" + SERIAL_NUMBER);
        digitalWrite(RED_LED, LOW);
        noTone(BUZZER);
        Display.show("----");
        countFailedLogins = 0;
        countPositions = 0;
      }
      else
      {
        digitalWrite(RED_LED, HIGH);
        tone(BUZZER, 20, 405);
        delay(500);
        Display.show("----");
        countPositions = 0;
      }
    }
  }
}
