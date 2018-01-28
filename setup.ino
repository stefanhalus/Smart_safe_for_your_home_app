void setup(){
  //Start serial
  Serial.begin(9600);
  
  //Red and green LED
  pinMode(redLedPin,OUTPUT);
  pinMode(greenLedPin,OUTPUT);

  //PIR sensor
  pinMode(pirSensorPin, INPUT);
  
  //buzzer
  pinMode(buzzerPin, OUTPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  PrintSystemStatusOnLcd();
  
  // Set the ARDUINO pin as an input, to monitor the multiplexor
  pinMode(multiplexorPin, INPUT_PULLUP);

  //Matrix Keypad - multiplexor
  multiplexor.begin(SX1509_ADDRESS);
  // To initialize the keypad engine, you at least need
  // to tell it how many rows and columns are in the matrix.
  // io.keypad(KEY_ROWS, KEY_COLS);
  // You can customize the keypad behavior further, by
  // defining scan time, debounce time, and sleep time:
  // Sleep time range: 128 ms - 8192 ms (powers of 2) 0=OFF
  unsigned int sleepTime = 0;
  // Scan time range: 1-128 ms, powers of 2
  byte scanTime = 64; // Scan time per row, in ms
  // Debounce time range: 0.5 - 64 ms (powers of 2)
  byte debounceTime = 8; // Debounce time
  multiplexor.keypad(KEY_ROWS, KEY_COLS, sleepTime, scanTime, debounceTime);

  //Being serial communication witj Arduino and SIM800
  serialSIM800.begin(115200);
  delay(2000);
  Serial.println("System Ready");

  Log("Initializez modulul sim  sa fie sms");
  serialSIM800.write("AT+CMGF=1\r\n");  //Set SMS format to ASCII
}   

