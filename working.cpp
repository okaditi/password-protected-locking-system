#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>

#define Password_Length 5

Servo myservo;
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
#define BUZZER 8

char Data[Password_Length];
char Master[Password_Length] = "1111";
byte data_count = 0;
int failed_attempts = 0;
bool door = false;
char customKey;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {0, 1, 2, 3};
byte colPins[COLS] = {4, 5, 6, 7};

Keypad customKeypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup()
{
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  myservo.attach(9, 2000, 2400);
  ServoClose();
  lcd.begin(16, 2);
  lcd.clear();
}

void loop()
{
  customKey = customKeypad.getKey();

  if (door == true)
  {
    if (customKey == '#')
    {
      lcd.clear();
      lcd.print("DOOR CLOSING");
      delay(1000);
      ServoClose();
      door = false;
      lcd.clear();
    }
  }
  else
  {
    Open();
  }
}

void Open()
{
  lcd.setCursor(0, 0);
  lcd.print(" ENTER PASSWORD");

  customKey = customKeypad.getKey();
  if (customKey)
  {
    Data[data_count] = customKey;
    lcd.setCursor(data_count, 1);
    lcd.print("*");
    data_count++;
  }

  if (data_count == Password_Length - 1)
  {
    if (!strcmp(Data, Master))
    {
      failed_attempts = 0;
      lcd.clear();
      ServoOpen();
      lcd.print("  DOOR IS OPEN ");
      door = true;
      delay(1000);
      lcd.clear();
    }
    else
    {
      failed_attempts++;
      lcd.clear();
      lcd.print(" WRONG PASSWORD ");
      delay(2000);
      lcd.clear();

      if (failed_attempts >= 3)
      {
        lcd.print(" SYSTEM LOCKED! ");
        for (int i = 0; i < 5; i++)
        {
          digitalWrite(BUZZER, HIGH);
          delay(500);
          digitalWrite(BUZZER, LOW);
          delay(500);
        }
        failed_attempts = 0;
      }
      else
      {
        digitalWrite(BUZZER, HIGH);
        delay(500);
        digitalWrite(BUZZER, LOW);
      }
    }
    clearData();
  }
}

void ServoClose()
{
  for (int pos = 90; pos >= 0; pos -= 10)
  {
    myservo.write(pos);
    delay(50);
  }
}

void ServoOpen()
{
  for (int pos = 0; pos <= 90; pos += 10)
  {
    myservo.write(pos);
    delay(50);
  }
}

void clearData()
{
  while (data_count != 0)
  {
    Data[data_count--] = 0;
  }
}