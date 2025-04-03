#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Password.h>

#define buzzer 11  // Buzzer pin

Servo servo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

Password password = Password("1111");  // Set password to "1111"

byte maxPasswordLength = 4;
byte currentPasswordLength = 0;
byte a = 5;  // Position for '*' in LCD

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

bool doorIsLocked = true;  // Track door state

void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  servo.attach(10);
  servo.write(90);  // Start in locked position

  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("WELCOME TO");
  lcd.setCursor(0, 1);
  lcd.print("DOOR LOCK SYSTEM");
  delay(3000);
  lcd.clear();
}

void loop() {
  lcd.setCursor(1, 0);
  lcd.print("ENTER PASSWORD");

  char key = keypad.getKey();
  if (key != NO_KEY) {
    Serial.print("Key Pressed: ");
    Serial.println(key);
    delay(60);

    processNumberKey(key);
  }
}

void processNumberKey(char key) {
  lcd.setCursor(a, 1);
  lcd.print("");  // Show '' instead of numbers
  a++;

  if (a == 9) {  // Adjusted for 4-digit password (positions 5 to 8)
    a = 5;
  }

  currentPasswordLength++;
  password.append(key);  // Append pressed key to password input

  if (currentPasswordLength == maxPasswordLength) {
    if (password.evaluate()) {
      beepBuzzer(3, 400, 500);  // ✅ Beep 3 times for correct password
      if (doorIsLocked) {
        dooropen();
      } else {
        doorlocked();
      }
    } else {
      wrongPassword();
    }
  }
}

void dooropen() {
  servo.attach(10);
  Serial.println("Correct Password! Unlocking door...");
  servo.write(0);  // Move to 0° (Unlocked)
  delay(500);
  servo.detach();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DOOR OPENED");
  delay(2000);
  lcd.clear();
  a = 5;
  resetPassword();

  doorIsLocked = false;  // Update door state
}

void doorlocked() {
  servo.attach(10);
  Serial.println("Correct Password! Locking door...");
  servo.write(90);  // Move to 90° (Locked)
  delay(500);
  servo.detach();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DOOR LOCKED");
  delay(2000);
  lcd.clear();
  a = 5;
  resetPassword();

  doorIsLocked = true;  // Update door state
}

void wrongPassword() {
  beepBuzzer(7, 200, 300);  // ❌ Beep 7 times for incorrect password
  Serial.println("Wrong Password! Try again.");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WRONG PASSWORD!");
  lcd.setCursor(0, 1);
  lcd.print("TRY AGAIN");
  delay(2000);
  lcd.clear();

  a = 5;
  resetPassword();
}

void resetPassword() {
  password.reset();
  currentPasswordLength = 0;
}

void beepBuzzer(int times, int onTime, int offTime) {
  for (int i = 0; i < times; i++) {
    tone(buzzer, 4000);  // 4kHz frequency for louder sound
    delay(onTime);
    noTone(buzzer);
    delay(offTime);
  }
}