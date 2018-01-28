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
  bool result = false;
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
      result = true;
    }
    ProcessInsertedKey(key);
    PrintInsertedKey(key);
  }
  return result;
}

char ReadKey()
{
  char key = '-';
  if (!digitalRead(multiplexorPin))
  {
    // Use readKeypad() to get a binary representation for
    // which row and column are pressed
    unsigned int keyData = multiplexor.readKeypad();
    // Use the getRow, and getCol helper functions to find
    // which row and column keyData says are active.
    byte row = multiplexor.getRow(keyData);
    byte col = multiplexor.getCol(keyData);
    key = keyMap[row][col];
  }
  return key;
}

bool IsAValidCode()
{
  bool result = false;
  if (codeInserted == masterCode)
  {
    result = true;
  }
  if (IsAUserCode())
  {
    result = true;
  }
  
  codeInserted = "";
  return result;
}

bool IsAUserCode()
{
  bool result = false;
  for (int i = 0; i < stringFromUILinesCount; i++)
  {
    if (codeInserted == passwordsFromUIArray[i])
    {
      result = true; 
      ShowMessage("Hello " + namesFromUIArray[i], 2000);
    }
  }
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
  if (key == 'C' && codeInserted == "")
  {
    changeCodeKeyPressed = true;
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

void PrintMessageOnLcd(String message)
{
  lcd.clear();
  lcd.print(message);
  lcd.setCursor(0, 1);
  lcd.blink();
}

void ShowMessage(String message, int delayTime)
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
    Serial.println("System Ready");
    armRequestedByUser = false;
    movementDetected = false;
    smsHaveToBeSent = false;
    PrintSystemStatusOnLcd();
  }
  else
  {
    Serial.println("System Armed");
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
  bool result = false;
  if (valPir > 150)
  {
    result = true;
  }
  return result;
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
  bool result = false;
  if (armRequestedByUser && ArmingDelayExceeded())
  {
    result = true;
  }
  else
  {
    result = false;
  }

  return result;
}

bool ArmingDelayExceeded()
{
  return millis() > (armedRequestedTime + armDelayTime);
}

bool DisarmingDelayExceeded()
{
  return millis() > (movementDetectedTime + disarmDelayTime);
}

bool UserAllowedToChangeCode()
{
  changeCodeKeyPressed = false;
  bool result = false;
  if (codeInserted == "" && !armRequestedByUser)
  {
    result = true;
  }
  return result;
}

bool CodeChanged()
{
  bool result = false;
  if (CodeChangeConfirmed())
  {
    PrintMessageOnLcd("Old code ?");
    while(true)
    {
      if (UserIsertedANewCode()) 
      {
        if (IsAValidCode()) 
        {
          PrintMessageOnLcd("New code");
          String firstNewCode = GetNewCode();
          PrintMessageOnLcd("Confirm new code");
          String secondNewCode = GetNewCode();
          if(firstNewCode == secondNewCode)
          {
            masterCode = firstNewCode;
            Log("code changed to " + (String)masterCode);
            result = true;
            break;
          }
          else
          {
            break;
          }
        }
        else 
        {
          break;
        }
      }  
    }
    
  }
  
  return result;
}

bool CodeChangeConfirmed()
{
  bool result = false;
  PrintMessageOnLcd("Change code ?");
  while(true)
  {
    if(ReadKey() == '#')
    {
      result = true;
      break;
    }
    if(ReadKey() == '*')
    {
      result = false;
      break;
    }
  }
  return result;
}

String GetNewCode()
{
  String result = "";
  while(true)
  {
    if (UserIsertedANewCode()) 
    {
      result = codeInserted;
      codeInserted = "";
      break;
    }
  }
  return result;
}

void ParseUsersString()
{
  stringFromUILinesCount = GetNumberOfLines();
  Log("lines : " + (String)stringFromUILinesCount);
  String line = "";
  int j = 0;
  int stringLength = stringFromUI.length();
  for(int i = 0; i < stringLength; i++)
  {
    if(stringFromUI.charAt(i) == '|')
    {
      SetNameAndPassword(line, j);
      line = "";
      j++;
    }
    else
    {
      line += stringFromUI.charAt(i);  
    }
  }
  //return result;
}

int GetNumberOfLines()
{
  int result = 0; 
  for(int i = 0; i < stringFromUI.length(); i++)
  {
    if(stringFromUI.charAt(i) == '|')
    {
      result++;
    }
  }
  return result;
}

void SetNameAndPassword(String line, int j)
{
  String stringArray[3];
  String temp = "";
  int k = 0;
  char separator = ',';
  for(int i = 0; i< line.length(); i++)
  {
    if(line.charAt(i) == separator)
    {
      stringArray[k] = temp;
      temp = "";
      k++;
    }
    else
    {
      temp += line.charAt(i);  
    }
  }
  
  namesFromUIArray[j] = stringArray[1];
  passwordsFromUIArray[j] = stringArray[2];
}

