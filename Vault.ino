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
int inputNumber;
int password = 1234;
int storePin=0;
int countInputButton;

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
  const int INPUT_NUMBERS = digitalRead(LEFT_BUTTON);
  const int CONFIRM_BUTTON = digitalRead(RIGHT_BUTTON);
  if (mode == 1)
  {
    if (countInputButton >= 1)
    {
      if (CONFIRM_BUTTON != confirmLastState)
      {
        if (CONFIRM_BUTTON == LOW)
        {
          countPositions++;
          countPositions = countPositions % 5;
          if (countPositions == 0)
          {
            countPositions = 1;
          }
          inputNumber = 0;
        }
        delay(150);
        confirmLastState = CONFIRM_BUTTON;
      }
    }
  }
  if (INPUT_NUMBERS != inputLastState)
  {
    countInputButton++;
    if (INPUT_NUMBERS == LOW)
    {
      inputNumber++;
      inputNumber = inputNumber % 5;
      if (inputNumber == 0)
      {
        inputNumber = 1;
      }
    }
     Serial.println(storePin);
    if (inputNumber == 1)Display.showCharAt(countPositions, '1');
    if (inputNumber == 2)Display.showCharAt(countPositions, '2');
    if (inputNumber == 3)Display.showCharAt(countPositions, '3');
    if (inputNumber == 4)Display.showCharAt(countPositions, '4');
    delay(150);
    inputLastState = INPUT_NUMBERS;
  }
}
