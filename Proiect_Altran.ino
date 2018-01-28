//#include <Wire.h> // Include the I2C library (required)
#include <SparkFunSX1509.h> // Include SX1509 library
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

//Pins used
int redLedPin = 13;
int greenLedPin = 12;
int buzzerPin = 11;
int pirSensorPin = A0;
// ARDUINO pin 8 connected to SX1509 interrupt
#define multiplexorPin 8

//initialize LCD
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

//SIM800L Pins
#define SIM800_TX_PIN 10
#define SIM800_RX_PIN 9
//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN, SIM800_RX_PIN);
String phoneNumber = "0742068839";

//Matrix Keyboard Multiplexor
const byte SX1509_ADDRESS = 0x3E;  // SX1509 I2C address (00)
SX1509 multiplexor; // Create an SX1509 object
#define KEY_ROWS 4
#define KEY_COLS 4
// Handy array we'll use to map row/column pairs to 
// character values:
char keyMap[KEY_ROWS][KEY_COLS] = {
  {'1', '2', '3', 'C'},
  {'4', '5', '6', '#'},
  {'7', '8', '9', '#'},
  {'*', '0', '#', '#'}};


String masterCode = "1234";
String codeInserted = "";
bool armRequestedByUser = false;
unsigned long armedRequestedTime;
bool movementDetected = false;
unsigned long movementDetectedTime;
bool smsHaveToBeSent = false;
bool changeCodeKeyPressed = false;

unsigned long armDelayTime = 20000;
unsigned long disarmDelayTime = 20000;

// Communication with UI
String stringFromUI = "";
String passwordsFromUIArray[20];
String namesFromUIArray[20];
int stringFromUILinesCount = 0;

// Do we want debugging on serial out? 1 for yes, 0 for no
int debug = 0;


