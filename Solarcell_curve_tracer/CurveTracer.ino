#include "DAC16.h"

DAC16 dac16(SYNCpin, SCLKpin, DINpin);

/*
          I |
            |       < measurement direction
            |  * * * * *     
            |               *
            |                 *
            |                   *
            |                    * 
            |                     *
            |                      *  
            |                          
            -----------------------------
              ^                         U
           mVend                

*/

void CurveTracer::init()
{ startTime_ms = millis();
}

void CurveTracer::run(bool curve)
{ mainsADC = adcMean(mainsPin, 256); // pre fill MA filter 1*)
  tADC = adcMean(Tpin, 256); // pre fill MA filter 
  luxADC = adcMean(luxPin, 256); // pre fill MA filter 
  //lampADC = adcMean(lampPin, 256); // pre fill MA filter 
  if(mainsADC >= 1020) { Serial << "\nPower not connected\n"; while(1); }
//  if(mainsADC >= 1020) USBsupply = true; // autodetection power supply
  FrequencyTimer2::setOnOverflow(timer2ISR); // enable ISR here, ISR disturbs 1*)
  if(curve) MPP(0, 1, dacStart1, dacEnd1, dacStep1);
  MPPcontinuous();
}

bool CurveTracer::MPP(bool stopAfterMpp, bool printCurve, unsigned dacStart, unsigned dacEnd, unsigned dacStep)
{ float mVnext, mW;
  unsigned timeOutCounter=0;
  mW_MPP=0;
  if(printCurve) Serial << F("\n\nmV mA mains T lux 24V DAC mW ");
  Serial << "(start " << dacStart << " step " << dacStep << ")";

  for(unsigned dac=dacStart; dac<=dacEnd; dac+=dacStep) 
  { measure(dac); 
    if(timeOutCounter++ > timeOut || Uoverflow || Ioverflow)
    { mW_MPP=0;//doen1
      dac_MPP = dacStart1; //doen1
      Serial << F("\nTimeout or overflow");
      return 0;
    }
    mW = mA * mV / 1000; 
    if(mW > mW_MPP) 
    { mW_MPP = mW; dac_MPP = dac; mV_MPP = mV; mains_MPP = _mains; T_MPP = _T; lux_MPP = _lux, lamp_MPP = _lamp; 
      Serial << "+"; // if dacStart is to low, to many + are printed
    } 
    else Serial << "-";

    if(stopAfterMpp) if(mW < max(0, mW_MPP - stopAfter_mW)) 
    { Serial << "(stop " << stopAfter_mW << "mW)";
      break;
    }
    if(dac == dacStart) mVnext = mV;     
    if(mV <= mVnext) // direction 80 75 70mV, reduce printed data
    { if(printCurve) Serial << endl << mV, mA, _mains, _T, _lux, _lamp, dac, mW; Serial << " ";   
      mVnext = mV - mVprintInterval; 
    }
    if(mV < mVend) break;
  }
  mA_max = mA;
  return 1;
}

void CurveTracer::MPPcontinuous() 
{ Serial << F("\ni sec mains T lux 24V mA_max mA_maxCorr dac_MPP mV_MPP mW mWcorr ");
  unsigned dacStart = dacStart1;
  for(unsigned i=0; ; i++)
  { unsigned long t = (millis() - startTime_ms)/1000.0;    // doen gaat fout .0    
    if(MPP(1, 0, dacStart, dacEnd1, dacStep1)) // measure coarse, dacStart changes
    { float mA_max1 = mA_max; // mA_max is only measured at first step
      float mA_maxCorr1 = mACorr(mA_max1, lux_MPP, T_MPP);    
      // dacStart = max(0, dac_MPP - dacStart2BackJump); doen1 fout
      dacStart = max(dacStart1, dac_MPP - dacStart2BackJump);
      MPP(1, 0, dacStart, dacEnd2, dacStep2); // measure fine around MPP
      Serial << endl << i, t, mains_MPP, T_MPP, lux_MPP, lamp_MPP, mA_max1, mA_maxCorr1, dac_MPP, mV_MPP, mW_MPP, mWcorr(mW_MPP, lux_MPP, T_MPP); Serial << " "; 
    }
    else dacStart = dacStart1;
  } 
}

void CurveTracer::measure(unsigned dac)
{ dac16.write(dac);
  delay (stepTimeCapacityEffect_ms); 
  mVmA(mVmAsamples);
  if(!USBsupply) _mains = mains();
  _T = T();
  _lux = lux();
  //_lamp = lamp();
  correctForWire();  
  if(Uoverflow) Serial << " U_overflow";
  if(Ioverflow) Serial << " I_overflow";
  if(mainsOverflow) Serial << " mains_overflow";
  if(Toverflow) Serial << " T_overflow";  
  //if(luxOverflow) Serial << " lux_overflow";  
  //if(lampOverflow) Serial << " lamp_overflow";  

  //if(stopWatchTimer.check()) Serial << stopWatch.counter << " " << stopWatch.interval << " " << stopWatch.maxInterval(1) << "\n"; // test
}

void CurveTracer::mVmA(unsigned samples) 
{ uADC = iADC = 0;
  sampleDownCounter = samples;  
  //stopWatch.start();
  while(sampleDownCounter); // take all samples
  //stopWatch.stop();
  uADC /= samples;
  mV = (aU * uADC + bU) * 1000.0; 
  iADC /= samples;
  mA = (aI * iADC + bI) * 1000.0; 
}

float CurveTracer::mains()
{ return (aMains * mainsADC + bMains); 
}

float CurveTracer::T()
{ return aT * tADC + bT; 
}

float CurveTracer::lux()
{ return luxADC; 
}

float CurveTracer::lamp()
{ float aLamp = 0.0244;//doen
  return (aLamp * lampADC); 
}

void CurveTracer::correctForWire()
{ mV = mV + mA * wire_mOhm / 1000;    
}

void CurveTracer::calibrateU()
{ FrequencyTimer2::setOnOverflow(timer2ISR); // enable ISR 
  dac16.write(0); // turn T1 off
  delay(500); 
  mVmA(1);
  Serial << F("Cal U adc: ") << uADC << " " << mV << " mV\n";  
}

void CurveTracer::calibrateI()
{ FrequencyTimer2::setOnOverflow(timer2ISR); // enable ISR 
  dac16.write(65535); // turn T1 on
  delay(500);
  mVmA(1);
  Serial << F("Cal I adc: ") << iADC << " " << mA << " mA\n"; 
}

void CurveTracer::calibrateMains()
{ FrequencyTimer2::setOnOverflow(timer2ISR); // enable ISR 
  delay(500);
  Serial << F("Cal mains adc: ") << mainsADC << " mains: " << mains() << endl; 
}

void CurveTracer::calibrateT()
{ FrequencyTimer2::setOnOverflow(timer2ISR); // enable ISR 
  delay(500);
  Serial << F("Cal T adc: ") << tADC << " T: " << T() << endl; 
}

void CurveTracer::update()
{ if(sampleDownCounter)
  { uADC += adcMean(Uoverflow, Upin, 1);
    iADC += adcMean(Ioverflow, Ipin, 1);   
    sampleDownCounter--;
  }
  // running moving average (RMA) = exponential!
  if(!USBsupply) mainsADC = (mainsADC * (RMAorder-1) + adcMean(mainsOverflow, mainsPin, 1)) / RMAorder; 
  tADC = (tADC * (RMAorder-1) + adcMean(Toverflow, Tpin, 1)) / RMAorder; 
  luxADC = (luxADC * (RMAorder-1) + adcMean(luxOverflow, luxPin, 1)) / RMAorder; 
  //lampADC = (lampADC * (RMAorder-1) + adcMean(lampOverflow, lampPin, 1)) / RMAorder; 
}

float CurveTracer::mWcorr(float mW, float lux, float T) 
{ return mW * (1 - TCmW * (T-T0)) * (1 - mWvsLux * (lux-lux0)/lux0); 
}

/*
float CurveTracer::mWcorr(float mW, float lux, float T) 
{ return mW * (TCmW * (T - T0) + 1) / ((lux-lux0) * mWvsLux/lux0 + 1); 
}
*/

float CurveTracer::mACorr(float mA, float lux, float T)
{ return mA * (1 - TCmA * (T-T0)) * (1 - mAvsLux * (lux-lux0)/lux0);
}

/*
float CurveTracer::mACorr(float mA, float lux) // doen TCmW????
{ return mA / ((lux-lux0) * mWvsLux/lux0 + 1);
}
*/
/*
float CurveTracer::mWcorr(float mW, float mains, float T) 
{ return mW * (TCmW * (T - Tref) + 1) / ((mains-230) * mWmainsCorr/230 + 1); 
}

float CurveTracer::mACorr(float mA, float mains)
{ return mA / ((mains-230) * ImainsCorr/230 + 1); 
}
*/
