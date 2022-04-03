#include <IRremote.h>

/*

Send infrared commands from the Arduino to the iRobot Roomba
by probono

2013-03-17 Initial release

Copyright (c) 2013 by probono
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

IR Remote Control
129 Left 
130 Forward 
131 Right 
132 Spot 
133 Max 
134 Small 
135 Medium 
136 Large / Clean 
137 Stop 
138 Power 
139 Arc Left 
140 Arc Right 
141 Stop  

Scheduling Remote  
142 Download 
143 Seek Dock 

Roomba Discovery Driveon Charger 
240 Reserved 
248 Red Buoy 
244 Green Buoy 
242 Force Field 
252 Red Buoy and Green Buoy 
250 Red Buoy and Force Field 
246 Green Buoy and Force Field 
254 Red Buoy, Green Buoy and Force 

Roomba 500 Drive-on Charger
160 Reserved 
161 Force Field 
164 Green Buoy 
165 Green Buoy and Force Field 
168 Red Buoy 
169 Red Buoy and Force Field 
172 Red Buoy and Green Buoy 
173 Red Buoy, Green Buoy and Force Field

Roomba 500 Virtual Wall
162 Virtual Wall 

Roomba 500 Virtual Wall Lighthouse ###### (FIXME: not yet supported here)
0LLLL0BB 
LLLL = Virtual Wall Lighthouse ID
(assigned automatically by Roomba 
560 and 570 robots) 
1-10: Valid ID 
11: Unbound 
12-15: Reserved 
BB = Which Beam
00 = Fence 
01 = Force Field 
10 = Green Buoy 
11 = Red Buoy

*/

//IRsend irsend; // hardwired to pin 3; use a transistor to drive the IR LED for maximal range
#define LEDPIN 4 //Hardwired to pin 4
#define IR_SEND_PIN 3
void setup() 
{
Serial.begin(9600);
 pinMode(LEDPIN, OUTPUT);
 IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK);
}
long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
 
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
 
  long result = (high<<8) | low;
 
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

void blinkLED() {
  digitalWrite(LEDPIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);                       // wait for a second
  digitalWrite(LEDPIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}


void roomba_send(int code) 
{
//Serial.print("Sending Roomba code ");
//Serial.print(code);
int length = 8;
unsigned int raw[length*2];
unsigned int one_pulse = 3000;
unsigned int one_break = 1000;
unsigned int zero_pulse = one_break;
unsigned int zero_break = one_pulse;

int arrayposition = 0;
// Serial.println("");
for (int counter = length-1; counter >= 0; --counter) {
if(code & (1<<counter)) {
// Serial.print("1");
raw[arrayposition] = one_pulse;
raw[arrayposition+1] = one_break;
} 
else {
// Serial.print("0");
raw[arrayposition] = zero_pulse;
raw[arrayposition+1] = zero_break;
}
arrayposition = arrayposition + 2;
} 
for (int i = 0; i < 3; i++) {
IrSender.sendRaw(raw, 15, 38);
delay(50);
}
//Serial.println("");

/*
Serial.print("Raw timings:");
for (int z=0; z<length*2; z++) {
Serial.print(" ");
Serial.print(raw[z]);
}
Serial.print("\n\n");
*/
}

void loop() 
{
  if (readVcc()<3300){
  blinkLED();
  }
  else{
    Serial.println(readVcc());
    roomba_send(162); 
    
  }
delay(100);
}
