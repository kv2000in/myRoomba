/*
Troubleshooting to get it working.
Polarity - IR LED Diode positive pin to D3 (PWM pin, hardwired in IRremote library, can not change), negative pin via 220 ohm resistor to GND
IRremote library downgraded to version 1.0.0 in order for it to work. latest versions 3.x or 4.x - don't work.
  
  Uses "A Multi-Protocol Infrared Remote Library for the Arduino":
  http://www.arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
*/

#include <IRremote.h>

// Initiate IRremote on Pin 3.
IRsend irsend;

void setup()
{
  irsend.enableIROut(38);//Lib function
}

void loop()
{
  //These values should work for pre i models of roomba
  irsend.mark(1000);
  irsend.space(1000);
  
  //For "i" models (i3, i7 etc) use the following
  //irsend.mark(500);
  //irsend.space(7500);
  
}