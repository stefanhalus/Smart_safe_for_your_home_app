void Log(String message)
{
  if (debug == 1)
  {
    Serial.println(message);
  }
}

void LedBlink(int pin, int delayTime)
{
  digitalWrite(pin, HIGH);
  delay(delayTime);
  digitalWrite(pin, LOW);
  delay(delayTime);
}

void LedOn(int pin)
{
  digitalWrite(pin, HIGH);
}

void LedOff(int pin)
{
  digitalWrite(pin, LOW);
}

void BuzzerBeep(int milliseconds)
{
  digitalWrite(buzzerPin, HIGH);
  delay(milliseconds);
  digitalWrite(buzzerPin, LOW);
}

void BuzzerOn()
{
  digitalWrite(buzzerPin, HIGH);
}

void BuzzerOff()
{
  digitalWrite(buzzerPin, LOW);
}

void PowerLedByAlarmState()
{
  //System Ready
  if (!armRequestedByUser)
  {
    LedOn(greenLedPin);
    LedOff(redLedPin);
  }
  else
  {
    //Delay while Arming
    if (!ArmingDelayExceeded())
    {
      LedOn(redLedPin);
      LedBlink(greenLedPin, 50);
    }
    else
    {
      LedOff(greenLedPin);
      if (movementDetected)
      {
        LedBlink(redLedPin, 50);
      }
      else
      {
        LedOn(redLedPin);
      }
    }
  }
}

// Return true when # key is pressed
bool UserIsertedANewCode()
{
  if (!digitalRead(multiplexorPin))
  {
    // Use readKeypad() to get a binary representation for
    // which row and column are pressed
    unsigned int keyData = multiplexor.readKeypad();
    // Use the getRow, and getCol helper functions to find
    // which row and column keyData says are active.
    byte row = multiplexor.getRow(keyData);
    byte col = multiplexor.getCol(keyData);
    char key = keyMap[row][col];
    if (key == '#')
    {
      return true;
    }
    ProcessInsertedKey(key);
    PrintInsertedKey(key);
  }
  return false;
}

bool IsAValidCode()
{
  bool result = codeInserted == code;
  codeInserted = "";
  return result;
}

void ProcessInsertedKey(char key) {
  if (IsANumber(key))
  {
    codeInserted += key;
  }
  if (key == '*')
  {
    codeInserted = "";
  }
}

bool IsANumber(char key)
{
  return key == '1' ||
         key == '2' ||
         key == '3' ||
         key == '4' ||
         key == '5' ||
         key == '6' ||
         key == '7' ||
         key == '8' ||
         key == '9' ||
         key == '0';
}

void PrintInsertedKey(char key)
{
  lcd.print(key);
  if (key == '*')
  {
    lcd.clear();

    PrintSystemStatusOnLcd();
  }
  Log("Key pressed: " + (String)key);
}

void PrintSystemStatusOnLcd()
{
  lcd.clear();
  if (armRequestedByUser)
  {
    lcd.print("System Armed");
  }
  else
  {
    lcd.print("System Ready");
  }
  lcd.setCursor(0, 1);
  lcd.blink();
}

void ShowErrorMessage(String message, int delayTime)
{
  lcd.clear();
  lcd.print(message);
  delay(delayTime);
  PrintSystemStatusOnLcd();
}

void SwitchSystemState()
{
  if (armRequestedByUser)
  {
    Log("System disarmed");
    armRequestedByUser = false;
    movementDetected = false;
    smsHaveToBeSent = false;
    PrintSystemStatusOnLcd();
  }
  else
  {
    Log("System Armed");
    armRequestedByUser = true;
    armedRequestedTime = millis();
    movementDetected = false;
    smsHaveToBeSent = false;
    PrintSystemStatusOnLcd();
  }
}

void CheckFirstMovementDetected()
{
  if (!movementDetected && MovementDetectedNow())
  {
    movementDetected = true;
    movementDetectedTime = millis();
    smsHaveToBeSent = true;
    Log("Movement Detected");
  }
}

bool MovementDetectedNow()
{
  int valPir = analogRead(pirSensorPin);
  if (valPir > 150)
  {
    return true;
  }
  return false;
}

void SendSms()
{
  serialSIM800.write("AT+CMGS=\"0742068839\"\r\n");  //Send new SMS command and message number
  delay(400);
  serialSIM800.write("Alerta de intruziune!  A intrat cineva in locatie si nu a dezarmat corect alarma.");  //Send SMS content
  delay(400);
  serialSIM800.write((char)26);  //Send Ctrl+Z / ESC to denote SMS message is complete
  Log("S-A TRIMIS SMS");
  smsHaveToBeSent = false;
}

bool SystemIsArmed()
{
  if (armRequestedByUser && ArmingDelayExceeded())
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool ArmingDelayExceeded()
{
  return millis() > (armedRequestedTime + armDelayTime);
}

bool DisarmingDelayExceeded()
{
  return millis() > (movementDetectedTime + disarmDelayTime);
}

