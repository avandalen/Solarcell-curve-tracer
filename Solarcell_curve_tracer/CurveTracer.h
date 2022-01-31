#ifndef CURVETRACER_H
#define CURVETRACER_H

class CurveTracer
{
public:
  void init();
  void update();
  void run(bool curve);
  void calibrateU();
  void calibrateI();
  void calibrateMains();
  void calibrateT();

private:
  bool MPP(bool stopAfterMpp, bool printCurve, unsigned dacStart, unsigned dacEnd, unsigned dacStep);
  void MPPcontinuous();
  void measure(unsigned dac);
  void mVmA(unsigned samples);
  float mains();
  float T();
  float lux();
  float lamp();
  void correctForWire();
  float mWcorr(float mW, float lux, float T);
  float mACorr(float mA, float lux, float T);

/*
  float mWcorr(float mW, float mains, float T);
  float mACorr(float mA, float mains);
*/
  volatile unsigned sampleDownCounter; // used in ISR
  volatile unsigned long uADC, iADC; // sum of unsigned numbers
  volatile float mainsADC, tADC, luxADC, lampADC; // movingAverage needs float
  bool Uoverflow, Ioverflow, mainsOverflow, Toverflow, luxOverflow, lampOverflow, USBsupply; // volatile doesn't work
  unsigned long startTime_ms;
  
  float mV, mA, _mains, _T, _lux, _lamp, mW_MPP, mV_MPP, mains_MPP, T_MPP, lux_MPP, lamp_MPP, mA_max;
  unsigned dac_MPP;
};

template<class T> 
Print &operator , (Print &stream, const T arg); 

#endif
