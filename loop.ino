void loop() {
  //Power both Leds
  PowerLedByAlarmState();


  //Read and process the inserted code
  if (UserIsertedANewCode()) {
    if (IsAValidCode()) {
      SwitchSystemState();
    }
    else {
      ShowErrorMessage("InvalidCode", 3000);
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

  // Captează consola SIM și o imprimă pe consola Arduino
//  if (serialSIM800.available()) {
//    Serial.write(serialSIM800.read());
//  }
}
