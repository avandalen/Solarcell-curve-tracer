/* 
Albert.h 
Arduino library Albert
Version 17-12-2015
Version 29-4-2016 #if defined(__arm__)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses.

Version 17-12-2015 analogReadfast
*/

#ifndef ALBERT_H
#define ALBERT_H

#if defined(__arm__)
  #define Serial SerialUSB
#endif

#define arrayLenght(array) sizeof(array)/sizeof(array[0]) // compile-time calculation

int availableRAM(); 
void openDrain(byte pin, bool value);
void blinkLed(byte pin, int n=3);
void maxLoops(const unsigned long loops);
int inline analogReadFast(byte ADCpin, byte prescalerBits=4);

//---------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned inline adcMean(const byte adcPin, const int samples) // inline library functions must be in header
{ unsigned long adcVal = 0;
  for(int i=0; i<samples; i++) adcVal += analogReadFast(adcPin);
  adcVal /= samples;
  return adcVal;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned inline adcMean(bool &overflow, const byte adcPin, const int samples) // inline library functions must be in header
{ unsigned long adcVal = 0;
  for(int i=0; i<samples; i++)
  { unsigned temp = analogReadFast(adcPin);
    adcVal += temp;
    overflow = ((temp >= 1023) | (temp <= 0)); 
  }
  adcVal /= samples;
  return adcVal;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------

#if defined(__arm__) 
int inline analogReadFast(byte ADCpin, byte prescalerBits) // inline library functions must be in header
{ ADC->CTRLA.bit.ENABLE = 0;                     // Disable ADC
  while( ADC->STATUS.bit.SYNCBUSY == 1 );        // Wait for synchronization
  ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV64 |   // Divide Clock by 64
                   ADC_CTRLB_RESSEL_10BIT; 
  ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_1 |   // 1 sample 
                     ADC_AVGCTRL_ADJRES(0x00ul); // Adjusting result by 0
  ADC->SAMPCTRL.reg = 0x00;                      // Sampling Time Length = 0

  ADC->CTRLA.bit.ENABLE = 1;                     // Enable ADC
  while( ADC->STATUS.bit.SYNCBUSY == 1 );        // Wait for synchronization
  return analogRead(ADCpin);
}
#else
int inline analogReadFast(byte ADCpin, byte prescalerBits) // inline library functions must be in header
{ byte ADCSRAoriginal = ADCSRA; 
  ADCSRA = (ADCSRA & B11111000) | prescalerBits; 
  int adc = analogRead(ADCpin);  
  ADCSRA = ADCSRAoriginal;
  return adc;
}
#endif

//---------------------------------------------------------------------------------------------------------------------------------------------------------

template<class T> 
inline Print &operator ,(Print &stream, const T arg) 
{ stream.print(" ");
  stream.print(arg); 
  return stream; 
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------

class SimpleSoftPWM // only for LEDs
{ public:
    bool getLevel(byte value); // 0 ... 255

  private:
    unsigned long start_us, periodTime_us=19890; // minimum frequency to prevent LED flickering = 50Hz
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------

class HeartBeat
{ public:
    HeartBeat(byte pin);
    void poll(); 
    void blinkCount(int _blinkCounts); 
  
  private:
    void heartBeat();
    void blink();
    SimpleSoftPWM heartBeatPWM;
  
    unsigned long last_ms;
    int Uled; 
    int blinkCounts;
    byte pin;
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------

#endif


