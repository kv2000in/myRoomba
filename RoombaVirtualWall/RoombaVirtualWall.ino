/*
Based on information found at:
http://sites.google.com/site/irobotcreate2/createanirbeacon

Uses "A Multi-Protocol Infrared Remote Library for the Arduino":
http://www.arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
*/

#include <IRremote.h>

// Initiate IRremote on Pin 3.
IRsend irsend;

void setup()
{
irsend.enableIROut(38);//Lib function
//pinMode(3,OUTPUT);
}

void loop()
{
//  digitalWrite(3,HIGH);
//  delay(1000);
//  digitalWrite(3,LOW);
//  delay(1000);
irsend.mark(1000);
irsend.space(1000);
}
