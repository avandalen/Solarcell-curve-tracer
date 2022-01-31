/* 
Albert.cpp 
Arduino library Albert
Version 17-12-2015
Version 29-4-2016 #if defined(__arm__)
Version 8-2-2017 Added analogReadFast code for SAMD21

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses.

Version 17-12-2015 analogReadfast, heartbeat
*/

#include <Arduino.h>
#include "Albert.h"
#include <avdweb_VirtualDelay.h>

// written by David A. Mellis. Based on code by Rob Faludi http://www.faludi.com
int availableRAM() 
{ int size = 70000; 
  byte *buf;
  while ((buf = (byte *) malloc(--size)) == NULL);
  free(buf);
  return size;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------

void openDrain(byte pin, bool value)
{ if(value) pinMode(pin, INPUT);
  else pinMode(pin, OUTPUT); 
  digitalWrite(pin, LOW);  
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------

void blinkLed(byte pin, int n)
{ pinMode(pin, OUTPUT); 
  for(byte i=0; i<n; i++)
  { digitalWrite(pin, HIGH);       
    delay(300);                 
    digitalWrite(pin, LOW);   
    delay(300);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------

void maxLoops(const unsigned long loops)
{ static unsigned long i=0;
  if(++i > loops) while(1);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------

bool SimpleSoftPWM::getLevel(byte value) // 0 ... 255, only for LEDs
{ unsigned long now_us = micros();
  if((now_us - start_us) >= periodTime_us) start_us = now_us;
  return((now_us - start_us) < value * 78); // 19890/255=78
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------

HeartBeat::HeartBeat(byte pin):
pin(pin)
{ pinMode(pin, OUTPUT);
}

void HeartBeat::blinkCount(int _blinkCounts) 
{ if(_blinkCounts > blinkCounts) blinkCounts = _blinkCounts; // high number overrides low number
}

void HeartBeat::poll() // 9us
{ if(blinkCounts==0) heartBeat();
  else blink(); 
}



//---------------------------------------------------------------------------------------------------------------------------------------------------------
