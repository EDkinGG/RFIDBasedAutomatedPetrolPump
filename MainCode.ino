#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SPI.h>
#include <RFID.h>
#define RELAY 2
#define SS_PIN A1
#define RST_PIN A0
RFID rfid(SS_PIN, RST_PIN);
String rfidCard;
const byte ROWS = 4;
const byte COLS = 4;
char data[10];
String password = "11298";
byte count = 0;
int amount;
char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
//Keypad Connections to Arduino
byte rowPins[ROWS] = { 10, 9, 8, 7 };
byte colPins[COLS] = { 6, 5, 4, 3 };
// Create keypad object
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins,
                             colPins, ROWS, COLS);
// Create LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2);
//LCD Display Connection
// lcd sda = A4
// lcd SCL = A5
// vcc = vcc
// gnd = gnd
void setup() {
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
  // Setup LCD with backlight and initialize
  lcd.backlight();
  lcd.begin();
  Serial.begin(9600);
  Serial.println("Starting the RFID Reader...");
  SPI.begin();
  rfid.init();
}

void(* resetFunc) (void) = 0;


void loop() {
  lcd.clear();
  lcd.setCursor(2, 0);  //lcd.setCursor(col, row);
  lcd.print("Smart Petrol");
  lcd.setCursor(1, 1);
  lcd.print("Filling Station");
  delay(100);

  if (rfid.isCard()) {
    Serial.println("OK");

    if (rfid.readCardSerial()) {
      Serial.println("OK");
      rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
      Serial.println(rfidCard);
      lcd.clear();
      lcd.print(rfidCard);
      //rfid.halt();

      if (rfidCard == "195 129 161 8") {
        Serial.println("RFID Reader: Card accepted!");
        lcd.clear();
        lcd.print("Card accepted!");
        delay(1500);
        lcd.clear();
        lcd.print("Enter Password:");
        lcd.setCursor(0, 1);

        String enpass = "";

        while (1) {
          char customKey = customKeypad.getKey();
          if (customKey) {
            lcd.print(customKey);
            Serial.println(customKey);
            if (customKey == '*') {
              break;
            } else {
              enpass += customKey;
            }
          }
        }

        if (password == enpass) {
          lcd.clear();
          lcd.print("    Password   ");
          lcd.setCursor(0, 1);
          lcd.print("    Accepted!   ");
          delay(1500);
          lcd.clear();
          lcd.print("Enter amount:");
          lcd.setCursor(0, 1);

          while (1) {
            //-----------Start of Keypad base code
            char customKey = customKeypad.getKey();
            if (customKey) {
              lcd.print(customKey);
              Serial.println(customKey);
              data[count] = customKey;
              count++;
            }
            if (customKey == 'C' && count > 0) {
              lcd.setCursor(13, 0);
              lcd.print(" ");
              lcd.setCursor(13, 0);
              count = 0;
            } else if (customKey == '*') {
              count--;
              amount = num();
              Serial.println(amount);
              lcd.clear();
              lcd.print("Amount = ");
              lcd.print(amount);
              delay(1500);
              int div = 50;
              Serial.println(div);
              int time = amount * div;
              Serial.println(time);       //time calculation in ms
              digitalWrite(RELAY, HIGH);  //for motor or lcd to represent pump is on
              lcd.clear();
              lcd.print("Filing.");
              int a = 0;
              while (a < time) {  //delay time with an animation effect
                delay(334);
                lcd.print(".");
                delay(334);
                lcd.print(".");
                delay(334);
                lcd.setCursor(7, 0);
                lcd.print(" ");
                lcd.setCursor(7, 0);
                a = a + 1002;
              }
              //LOW
              digitalWrite(RELAY, LOW);  //for motor or lcd to represent pump is stopped
              lcd.clear();
              lcd.print(" Done Filling");
              lcd.setCursor(3, 1);
              lcd.print("Thank You!");
              delay(3000);
              break;
            }

            //-----------END of Keypad base code
          }
          Serial.println("AAAA");
        } else {
          Serial.println("Wrong Password!");
          lcd.clear();
          lcd.print("Wrong Password!");
          delay(3000);
        }
      } else {
        Serial.println("RFID Reader: Card Denied!");
        lcd.clear();
        lcd.print(" Card Denied!!");
        delay(3000);
        lcd.clear();
      }
    }
    Serial.println("halt er agge");
    resetFunc();  //call reset
    rfid.halt();
    Serial.println("AhjAA");
  }
}

int num()  //converts data-array into number
{
  int n = 0;
  int i = 0;
  while (i < count) {
    n = n * 10 + data[i] - '0';
    i++;
  }
  count = 0;
  return n;
}