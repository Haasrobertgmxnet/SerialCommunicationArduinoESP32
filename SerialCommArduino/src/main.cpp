#include <Arduino.h>
#include "SoftwareSerial.h"

//#define PIN_RESET 7
// #define PIN_TX 10
// #define PIN_RX 11
// #define PIN_PWR 4

const int sensorPin= SIG_PIN;
const int controlPin= PWR_PIN;
const int espPin= PWR_PIN_ESP32;
const int rxPin= RX_PIN;
const int txPin= TX_PIN;

ISR(WDT_vect){
  //DON'T FORGET THIS!  Needed for the watch dog timer.  This is called after a watch dog timer timeout - this is the interrupt function called after waking up
}// watchdog interrupt

SoftwareSerial mySerial(txPin, rxPin);

String state= "Undefined";

inline String wait_for_msg(String msg, int k= 0){
  unsigned len= msg.length();
  while(mySerial.available()==0);
  String state;
  do{
    Serial.print("... " );
    state= mySerial.readStringUntil('\r');
  }while (state.substring(0,len)!=msg);
  Serial.println("\n\nwait_for_msg: " + state);
  return state;
}

void setup()
{
  pinMode(controlPin, OUTPUT);

  //Save Power by writing all Digital IO LOW - note that pins just need to be tied one way or another, do not damage devices!
  // for (int i = 0; i < 16; i++) {
  //   if(i == sensorPin) continue;
  //   if(i == rxPin) continue;
  //   if(i == txPin) continue;
  //   pinMode(i, OUTPUT);
  // }

  //SETUP WATCHDOG TIMER
  WDTCSR = (24);//change enable and WDE - also resets
  WDTCSR = (33);//prescalers only - get rid of the WDE and WDCE bit
  WDTCSR |= (1<<6);//enable interrupt mode

  //ENABLE SLEEP - this enables the sleep mode
  SMCR |= (1 << 2); //power down mode
  SMCR |= 1;//enable sleep

  digitalWrite(controlPin, LOW);

  Serial.begin(9600);
  mySerial.begin(9600);

  while (!Serial);
  Serial.println("Initializing...");
  
}

void loop()
{
  mySerial.flush();
  // supply ESP32 with power
  digitalWrite(controlPin, HIGH);
  
  // wait for messages from ESP32
  wait_for_msg("Ready!",1);
  wait_for_msg("GSM state: ",2);

  // read moisture
  ADCSRA |= (1 << 7); // Enable ADC
  auto reading = analogRead(sensorPin); // Read from sensor pin 2
  ADCSRA &= ~(1 << 7); // Disable ADC
  float val= static_cast<float>(reading);
  String smsText= "Bodenfeuchte-Wert: " + String(val);
  mySerial.println(smsText);
  mySerial.flush();
  Serial.println(smsText);
  
  delay(500);
  wait_for_msg("Data sent!",3);
  delay(500);
  Serial.println("End!");
  digitalWrite(controlPin, LOW);
  delay(4000);
  for(int i=0;i<3;++i){
    //BOD DISABLE - this must be called right before the __asm__ sleep instruction
    MCUCR |= (3 << 5); //set both BODS and BODSE at the same time
    MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); //then set the BODS bit and clear the BODSE bit at the same time
    __asm__  __volatile__("sleep");//in line assembler to go to sleep
  }
}

