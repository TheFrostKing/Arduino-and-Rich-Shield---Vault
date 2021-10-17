#include <DHT11.h>
#include <Display.h>
#include <TM1637Display.h>

//Declare pins
const int BUZZER = 3;
const int RED_LED = 4;
const int GREEN_LED = 5;
const int BLUE_LED = 6;
const int YELLOW_LED = 7;
const int RIGHT_BUTTON = 8;
const int LEFT_BUTTON = 9;
const int LDR = 16;
//states that change
int inputLastState = HIGH;
int confirmLastState = HIGH;
//variables
int mode = 1;
int countPositions;
int inputNumber = 1;
int password = 1234;
int storePin;
int countInputButton;
//bool variables
bool isDoorLocked = true; //the safe starts locked
bool isDoorOpen = false;
bool resetSettingsAfterUnlock = false;
bool resetAfterBreach = false;
int countPassTries;
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
  const int UPPER_TH = 500; // 70
  const int LOWER_TH = 200; //10
  if (ldrValue > UPPER_TH)    //APPLYING HYSTERESIS, using two tresholds to remove oscillation
  {
    digitalWrite(GREEN_LED, HIGH);   //opens the door
    isDoorOpen = true;
    if (isDoorOpen == true && isDoorLocked == true)
    {
      tone(BUZZER, 201, 405);
      Serial.println("BREACH");
      resetAfterBreach = true;
    }
  }
  if (isDoorOpen == false && isDoorLocked == true && resetAfterBreach == true)
  {
    Display.show("----");
    storePin = 0;
    countInputButton = 0;
    countPositions = 0;
  }
  if (ldrValue < LOWER_TH)    //closes the door
  {
    digitalWrite(GREEN_LED, LOW);
    isDoorLocked = true;
  }
  if (isDoorOpen == true && isDoorLocked == false)
  {
    Display.showCharAt(0, 'O');
    Display.showCharAt(1, 'P');
    Display.showCharAt(2, 'E');
    Display.showCharAt(3, 'N');
    Serial.println("door is open");
    digitalWrite(YELLOW_LED, HIGH);
    resetSettingsAfterUnlock = true;
  }
  if (isDoorOpen == false && isDoorLocked == false && resetSettingsAfterUnlock == true)
  {
    Serial.println("safe is closed");
    isDoorLocked = true;
    digitalWrite(YELLOW_LED, LOW);
    Display.show("----");
    storePin = 0;
    countInputButton = 0;
    countPositions = 0;
    resetSettingsAfterUnlock = false;
  }
  if (countInputButton >= 1)
  {
    if (CONFIRM_BUTTON != confirmLastState)
    {
      tone(BUZZER, 20, 405);
      if (CONFIRM_BUTTON == LOW)
      {
        countPositions++;
        countPositions = countPositions % 6;
        Serial.print("WHAT IS THE POSTION OF CONFIRM");
        Serial.println(countPositions);

        if (countPositions == 0)
        {
          countPositions = 1;
        }
        switch (countPositions)
        {
          case 1:
            storePin += inputNumber * 1000;
            Serial.println(storePin);
            break;
          case 2:
            storePin += inputNumber * 100;
            Serial.println(storePin);
            break;
          case 3:
            storePin += inputNumber * 10;
            Serial.println(storePin);
            break;
          case 4:
            storePin += inputNumber * 1;
            Serial.println(storePin);
            break;
        }
        if (storePin != password && countPositions == 5)    // countPositions is the 5th click of confirm button to check if the pin matches the preset one
        {
          if (isDoorOpen == true)                            // if the door is open while the pin is incorrect that means a breach
          {
            Serial.println("Safe is broken into");
            Serial.println("Alarm");
            //put correct pin to stop
          }
          if (isDoorOpen == false)                   // otherwise if the door is not open checks whether the passes match and if they don't we have 2 more tries(3 in total). If all failed - alarm triggers
          {
            countPassTries++;
            if (countPassTries == 3)
            {
              //If the visitor enters a wrong code 3 times in a row then the buzzer beeps and the red LED blinks for 5
              //seconds. A message “ALARM SAFE #” is send to the reception via the serial port. # is the unique safe
              //number and the serial monitor is the reception. The visitor has to wait 5 seconds to enter a new
              //code.
              digitalWrite(RED_LED, HIGH);
              tone(BUZZER, 201, 405);
              delay(1000);
              digitalWrite(RED_LED, LOW);
              noTone(BUZZER);
              delay(1000);
              digitalWrite(RED_LED, HIGH);
              tone(BUZZER, 201, 405);
              delay(1000);
              digitalWrite(RED_LED, LOW);
              noTone(BUZZER);
              delay(1000);
              digitalWrite(RED_LED, HIGH);
              tone(BUZZER, 201, 405);
              delay(1000);
              Serial.println("Alarm");
              Display.show("----");
              storePin = 0;
              countInputButton = 0;
              countPositions = 0;
              countPassTries = 0;
            }
            else
            {
              Serial.println("I am not in");
              Serial.println(isDoorLocked);
              digitalWrite(RED_LED, HIGH);
              delay(1000);
              Display.show("----");
              storePin = 0;
              countInputButton = 0;
              countPositions = 0;
            }
          }
        }
        if (storePin == password && countPositions == 5) // if passwords match the door can be open
        {
          if (isDoorOpen == false && isDoorLocked == true)
          {
            digitalWrite(GREEN_LED, HIGH);
            delay(1000);
            Display.clear();
            isDoorLocked = false;
            Serial.println("door is not open but unlocked");
          }
        }
        inputNumber = 1;
      }
      delay(50);
      confirmLastState = CONFIRM_BUTTON;
    }
  }
  if (INPUT_NUMBERS != inputLastState)
  {
    countInputButton++;
    if (INPUT_NUMBERS == LOW)
    {
      tone(BUZZER, 20, 405);
      inputNumber++;
      inputNumber = inputNumber % 6;
      if (inputNumber == 0)
      {
        inputNumber = 1;
      }
    }
    if (inputNumber == 1)Display.showCharAt(countPositions, '1');
    if (inputNumber == 2)Display.showCharAt(countPositions, '2');
    if (inputNumber == 3)Display.showCharAt(countPositions, '3');
    if (inputNumber == 4)Display.showCharAt(countPositions, '4');
    if (inputNumber == 5)Display.showCharAt(countPositions, '5');
    delay(50);
    inputLastState = INPUT_NUMBERS;
  }
  if (inputNumber == 1)Display.showCharAt(countPositions, '1');
}
