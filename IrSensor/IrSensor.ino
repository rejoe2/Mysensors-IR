/**
   The MySensors Arduino library handles the wireless radio link and protocol
   between your home built sensors/actuators and HA controller of choice.
   The sensors forms a self healing radio network with optional repeaters. Each
   repeater and gateway builds a routing tables in EEPROM which keeps track of the
   network topology allowing messages to be routed to nodes.

   Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
   Copyright (C) 2013-2015 Sensnology AB
   Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors

   Documentation: http://www.mysensors.org
   Support Forum: http://forum.mysensors.org

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.

 *******************************

   REVISION HISTORY
   Version 1.0 - Henrik EKblad

   DESCRIPTION
   Example sketch showing how to control ir devices
   An IR LED must be connected to Arduino PWM pin 3.
   An optional ir receiver can be connected to PWM pin 8.
   All receied ir signals will be sent to gateway device stored in VAR_1.
   When binary light on is clicked - sketch will send volume up ir command
   When binary light off is clicked - sketch will send volume down ir command
   http://www.mysensors.org/build/ir
*/

// Enable debug prints
//#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

#include <SPI.h>
#include <MySensor.h>
#include <IRLib.h>

int RECV_PIN = 8;

#define CHILD_ID_IR  3  // childId

IRsend irsend;
IRrecv irrecv(RECV_PIN);
IRdecode decoder;
//decode_results results;
unsigned int Buffer[RAWBUF];
MyMessage msgIr(CHILD_ID_IR, V_IR_RECEIVE);

void setup()
{
  irrecv.enableIRIn(); // Start the ir receiver
  decoder.UseExtnBuf(Buffer);

}

void presentation()  {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("IR Sensor", "1.0");

  // Register the sensor als IR Node
  present(CHILD_ID_IR, S_IR);
}

void loop()
{
  if (irrecv.GetResults(&decoder)) {
    irrecv.resume();
    decoder.decode();
    decoder.DumpResults();

    char buffer[10];
    sprintf(buffer, "%08lx", decoder.value);
    // Send ir result to gw
    send(msgIr.set(buffer));
  }
}



void receive(const MyMessage &message) {
  const char *irData;
  // We will try to send a complete send command from controller side, e.g. "NEC, 0x1EE17887, 32"
  if (message.type == V_IR_SEND) {
    irData = message.getString();
#ifdef MY_DEBUG
    Serial.println(F("Received IR send command..."));
    //Serial.print(F("Code: 0x")); //we will only need this in case we cannot send the complete command set
    Serial.println(irData);
#endif
    char arg0 = irData[0];
    int arg1 = irData[1];
    uint8_t arg2 = irData[2];
    irsend.send(arg0, arg1, arg2);
  }

  // Start receiving ir again...
  irrecv.enableIRIn();
}

// Dumps out the decode_results structure.
// Call this after IRrecv::decode()
// void * to work around compiler issue
//void dump(void *v) {
//  decode_results *results = (decode_results *)v
/*void dump(decode_results *results) {
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  }
  else if (results->decode_type == NEC) {
    Serial.print("Decoded NEC: ");
  }
  else if (results->decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  }
  else if (results->decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  }
  else if (results->decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  }
  else if (results->decode_type == PANASONIC) {
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->panasonicAddress,HEX);
    Serial.print(" Value: ");
  }
  else if (results->decode_type == JVC) {
     Serial.print("Decoded JVC: ");
  }
  Serial.print(results->value, HEX);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): ");

  for (int i = 0; i < count; i++) {
    if ((i % 2) == 1) {
      Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
    }
    else {
      Serial.print(-(int)results->rawbuf[i]*USECPERTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println("");
  }
*/

