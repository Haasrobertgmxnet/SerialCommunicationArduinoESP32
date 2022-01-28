#include <Arduino.h>
#include "WiFi.h"
//#include "SoftwareSerial.h"

// #undef SEND_SMS

#include "gsmwork.h"

const int ledPin= PIN_LED;
const int ledPin2= PIN_LED2;
const int rxPin= PIN_RX;
const int txPin= PIN_TX;

// Set serial for AT commands (to SIM800 module)
#define SerialAT  Serial1

String state= "Undefined";

void setup() {
  // Save power
  WiFi.mode( WIFI_MODE_NULL );
  btStop();
  setCpuFrequencyMhz(80);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, rxPin, txPin);
  pinMode(ledPin,OUTPUT);
  pinMode(ledPin2,OUTPUT);
  digitalWrite(ledPin, HIGH);
  delay(300);
  digitalWrite(ledPin, LOW);
  delay(300);
  digitalWrite(ledPin, HIGH);
  delay(300);
  digitalWrite(ledPin, LOW);
  delay(300);
  digitalWrite(ledPin, HIGH);
  delay(300);
  digitalWrite(ledPin, LOW);
  delay(300);
  while (!Serial);
  state= "Ready!";
  Serial.println(state);
  //do work
  delay(500);
  
  #ifdef SEND_SMS_AT
  Serial1.print("AT\r");                   //Set the module to SMS mode
  String moduleState= Serial1.readStringUntil('\r');
  Serial1.print("AT+CSQ\r");                   //Set the module to SMS mode
  String signalState= Serial1.readStringUntil('\r');
  Serial1.print("AT+CBC\r");                   //Set the module to SMS mode
  String batteryState= Serial1.readStringUntil('\r');
  state= String("GSM state: " + moduleState+signalState+batteryState);
  digitalWrite(ledPin2, LOW);
  delay(300);
  digitalWrite(ledPin2, HIGH);
  delay(300);
  digitalWrite(ledPin2, LOW);
  delay(300);
  digitalWrite(ledPin, HIGH);
  delay(300);
  digitalWrite(ledPin, LOW);
  delay(1000);
  Serial.println(state);
  Serial.flush();
  #endif

  while(Serial.available()> 0){
    String reading= Serial.readStringUntil('\r');
    if(reading.substring(0,18) != String("Bodenfeuchte-Wert:")) continue;
    Serial.println("Serial: " + reading);
    #ifdef SEND_SMS
    prepareGSM();
    sendSMS(reading);
    #endif
    #ifdef SEND_SMS_AT
    delay(500);
    Serial1.print("AT+CMGF=1\r");                   //Set the module to SMS mode
    delay(100);
    Serial1.print("AT+CMGS=\"+4917680181926\"\r");  //Your phone number don't forget to include your country code, example +212123456789"
    delay(500);
    Serial1.print(reading);       //This is the text to send to the phone number, don't make it too long or you have to modify the SoftwareSerial buffer
    delay(500);
    Serial1.print((char)26);// (required according to the datasheet)
    delay(500);
    Serial1.println();
    delay(500);
    #endif
    digitalWrite(ledPin2, HIGH);
    delay(300);
    digitalWrite(ledPin2, LOW);
    delay(300);
    digitalWrite(ledPin2, HIGH);
    delay(300);
    digitalWrite(ledPin2, LOW);
    delay(300);
    digitalWrite(ledPin2, HIGH);
    delay(300);
    digitalWrite(ledPin2, LOW);
    delay(300);
  }
 
  delay(1000);
  state="Data sent!";
  delay(1000);
  Serial.println(state);
  Serial.flush();

  Serial.flush();
  digitalWrite(ledPin2, HIGH);
  delay(300);
  digitalWrite(ledPin2, LOW);
  delay(300);  
}

void loop() {
  // left empty
}