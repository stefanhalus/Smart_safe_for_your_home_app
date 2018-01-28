void loop() {
  //Power both Leds
  PowerLedByAlarmState();


  //Read and process the inserted code
  if (UserIsertedANewCode()) 
  {
    if (IsAValidCode()) 
    {
      SwitchSystemState();
    }
    else 
    {
      ShowMessage("InvalidCode", 3000);
    }
  }

  //Change code
  if (changeCodeKeyPressed && UserAllowedToChangeCode())
  {
    if (CodeChanged())
    {
      ShowMessage("Success", 3000);
    }
    else
    {
      ShowMessage("Fail", 3000);
    }
  }
  
  // System Armed
  // If there is a signal from Pir sensor, wait for the code
  // If the disarm delay time exceeds make noise and send SMS
  if (SystemIsArmed())
  {
    CheckFirstMovementDetected();

    if (movementDetected && DisarmingDelayExceeded())
    {
      BuzzerBeep(50);
      if (smsHaveToBeSent)
      {
        SendSms();
      }
    }
  }
  
  if (Serial.available() > 0)
  {
    char charReadFromSerial = Serial.read();
    //Log((String)charReadFromSerial);
    if (charReadFromSerial == '#')
    {
      delay(500);
      ParseUsersString();
    }
    else if (charReadFromSerial == '*')
    {
      stringFromUI = "";
    }
    else
    {
      stringFromUI += (String)charReadFromSerial;    
    }  
  }
}
