/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0 - Henrik Ekblad
 * DESCRIPTION
 * Example sketch showing how to control ir devices
 * An IR LED must be connected to Arduino PWM pin 3.
 * An optional ir receiver can be connected to PWM pin 8. 
 * All receied ir signals will be sent to gateway device stored in VAR_1.
 * When binary light on is clicked - sketch will send volume up ir command
 * When binary light off is clicked - sketch will send volume down ir command
 * http://www.mysensors.org/build/ir
 */

// Enable debug prints
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

#define MY_NODE_ID 99


#include <SPI.h>
#include <MySensor.h>
#include <IRremote.h>

int RECV_PIN = 8;
#define CHILD_ID_IR  3  // childId

IRrecv irrecv(RECV_PIN);

decode_results results;
MyMessage msgIr(CHILD_ID_IR, V_IR_RECEIVE);


void setup()
{
  //Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}

void presentation()  {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("IR Sensor", "1.0");

  // Register a sensors to  Use binary light for test purposes.
  present(CHILD_ID_IR, S_IR);
}


void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    char buffer[10];
    sprintf(buffer, "%08lx", results.value);

    // Send ir result to gw
    send(msgIr.set(buffer));

    irrecv.resume(); // Receive the next value
  }
  delay(100);
} 
/*
void receive(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type==V_IR_SEND) {
     int incomingRelayStatus = message.getInt();
     if (incomingRelayStatus == 1) {
      irsend.send(NEC, 0x1EE17887, 32); // Vol up yamaha ysp-900
     } else {
      irsend.send(NEC, 0x1EE1F807, 32); // Vol down yamaha ysp-900
     }
     // Start receiving ir again...
    irrecv.enableIRIn(); 
  }
}
*/
 
