/************************************************************
BLE CC41A Bluetooth 
This is a clone of the HM10 BLE board

In the Serial Monitor ensure that 'Both NL and CR' is selected
Select a Baud Rate of 9600
********************************************************/
#include <SoftwareSerial.h>

SoftwareSerial bluetooth(2, 3); // RX, TX

void setup()
{
  // Start the hardware serial port
  Serial.begin(9600);
  pinMode(13, OUTPUT);   // onboard LED
  digitalWrite(13, LOW); // switch OFF LED
  bluetooth.begin(9600);
}

void loop()
{
  bluetooth.listen();
  // while there is data coming in, read it
  // and send to the hardware serial port:
  while (bluetooth.available() > 0) {
    char inByte = bluetooth.read();
    Serial.write(inByte);
    if (inByte=='1') digitalWrite(13,HIGH);  // if received character 1 from BLE, set PIN 13 high
    if (inByte=='0') digitalWrite(13,LOW);   // if received character 0 from BLE, set PIN 13 low
  }

  // Read user input if available.
  if (Serial.available()){
    delay(10); // The DELAY!
    bluetooth.write(Serial.read());
  }
}
