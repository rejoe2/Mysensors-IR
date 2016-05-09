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
#define USE_DUMP //should print out lots of info to serial (ElectricRCAircraftGuy feature?)

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

#include <SPI.h>
#include <MySensor.h>
//#include <IRLib.h> //ElectricRCAircraftGuy version
#include <irmp.h> // aus https://www.mikrocontroller.net/articles/IRMP

int RECV_PIN = 8;

#define CHILD_ID_IR  3  // childId

IRsend irsend;
IRrecv My_Receiver(RECV_PIN);
IRdecode My_Decoder;
//decode_results results;
//unsigned int Buffer[RAWBUF];
MyMessage msgIr(CHILD_ID_IR, V_IR_RECEIVE);

void setup()
{
  // My_Decoder.useDoubleBuffer(Buffer); //uncomment to use; requires the "extraBuffer" to be uncommented above
  //Try different values here for Mark_Excess. 50us is a good starting guess. See detailed notes above for more info.
  My_Receiver.Mark_Excess = 50; //us; mark/space correction factor
  //Optional: set LED to blink while IR codes are being received
  // My_Receiver.blink13(true); //blinks whichever LED is connected to LED_BUILTIN on your board, usually pin 13
  //                            //-see here for info on LED_BUILTIN: https://www.arduino.cc/en/Reference/Constants
  My_Receiver.setBlinkLED(13, true); //same as above, except you can change the LED pin number if you like
  My_Receiver.enableIRIn(); // Start the receiver

}

void presentation()  {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("IR Sensor", "1.0");

  // Register the sensor als IR Node
  present(CHILD_ID_IR, S_IR);
}

void loop()
{
  if (My_Receiver.getResults(&My_Decoder)) //if IR data is ready to be decoded
  {
    //1) decode it
    My_Decoder.decode();
    //filter out zeros and NEC repeats
    const char rec_value = My_Decoder.value;
    if (rec_value != 0xffffffff && rec_value != 0x0) {

      Serial.println("decoding");

      //2) print results
      //FOR BASIC OUTPUT ONLY:
      Serial.print(Pnames(My_Decoder.decode_type));
      Serial.print(", ");
      Serial.print(My_Decoder.value, HEX);
      Serial.print(", ");
      Serial.println(My_Decoder.bits);
      // Serial.println();
      //FOR EXTENSIVE OUTPUT:
      //My_Decoder.dumpResults();
      char buffer[24];
//      uint8_t IrType; //don't know how to send type as char; may not be necessary
//      IrType = My_Decoder.decode_type;
      uint8_t IrBits = My_Decoder.bits;   
      sprintf(buffer, "%i, 0x%08lX, %i", My_Decoder.decode_type, My_Decoder.value, IrBits);
//      buffer = buffer+(", ");
//      buffer = buffer+("%08lx", My_Decoder.value);
//      buffer = buffer+(", ");
//      buffer = buffer+(My_Decoder.bits);
      //sprintf(buffer, "%08lx", My_Decoder.value);
      // Send ir result to gw
      send(msgIr.set(buffer));
      Serial.println(buffer);
    }
    //3) resume receiver (ONLY CALL THIS FUNCTION IF SINGLE-BUFFERED); comment this line out if double-buffered
    /*for single buffer use; do NOT resume until AFTER done calling all decoder
      functions that use the last data acquired, such as decode and dumpResults; if using a double
      buffer, don't use resume() at all unless you called My_Receiver.detachInterrupt() previously
      to completely stop receiving, and now want to resume IR receiving.*/
    My_Receiver.resume();

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

//splitting the received send command needs to be completed
//also transfer from numeric representation to char for protocol type
//could be obsolete, if protocol is sent as text    
/*
    uint8_t arg0;
    arg0 = (uint8_t *) {irData}[0];
    int arg1;
    arg1 = {irData}[1];
    uint8_t arg2;
    arg2 = (uint8_t *) {irData}[2];
    Serial.println(arg0);
    Serial.println(arg1, HEX);
    Serial.println(arg2);

    //irsend.send(arg0, arg1, arg2);  
 */
  }

  // Start receiving ir again...
  My_Receiver.resume();

  //  irrecv.enableIRIn();
}
/* codesniplet from Aircraft.. IRserial_remote example
    void loop() {
  if (Serial.available ()>0) {
    protocol = Serial.parseInt (); parseDelimiter();
    code     = parseHex ();        parseDelimiter();
    bits     = Serial.parseInt (); parseDelimiter();
    Serial.print("Prot:");  Serial.print(protocol);
    Serial.print(" Code:"); Serial.print(code,HEX);
    Serial.print(" Bits:"); Serial.println(bits);
    My_Sender.send(IRTYPES(protocol), code, bits);
  }
  }*/


