// pins
const byte Upin = 0;
const byte Ipin = 1;
const byte Tpin = 2;
const byte mainsPin = 3;
const byte luxPin = 4;
const byte lampPin = 5;
const byte SYNCpin = 2;
const byte SCLKpin = 3; 
const byte DINpin = 4;

// ISR constants
const unsigned long timer2ISRInterval_us = 1000;
const unsigned long timer2Setting = timer2ISRInterval_us*2; // weird FrequencyTimer2 

// curve constants
const unsigned mVprintInterval = 5; // just for printing, not for MPPT
const unsigned mVend = 150;
const unsigned dacStep1 = 128;
const unsigned dacStart1 = 45000; // 1 course
const unsigned dacEnd1 = 65535;
//const unsigned dacStep2 = 4; // fine accurate
const unsigned dacStep2 = 32; //32 fine
const unsigned dacStart2BackJump = 150; 
const unsigned dacEnd2 = 65535;
const unsigned stopAfter_mW = 15;
const unsigned timeOut = 60;

// calibration constants
const float aU = 0.00074029; 
const float bU = 0.00018932;  
const float aI = 0.00630197; 
const float bI = -0.15170678; 
const float aMains = -0.03947368; 
const float bMains = 247.52631579; 
const float aT = 0.09137056; 
const float bT = 5.17766497;  

// measurements constants
const unsigned wire_mOhm = 2;
const unsigned stepTimeCapacityEffect_ms = 2; // second curve is wrong with 0ms
const unsigned RMAorder = 8192; // take 2^n //doen1 ?? andere naam, voor T en lux, te grootte tijdcontante 20s lux erg traag?
const unsigned mVmAsamples = 100; // = 100=100ms = 10 * period 1/100Hz 
//const float mWmainsCorr = 2.8; // why not 2 ?
//const float ImainsCorr = 2.6; 

// correction constants 
// PVCool2-2 T lux corr 8-8-2013 R17 3900.xls
const unsigned T0 = 25;
const float TCmW = -0.0036; // pvcdrom -0.004 ... -0.005
const float TCmA = 0.0006; // pvcdrom 0.0006
const unsigned lux0 = 900; // adjust 24V to 900 
const float mWvsLux = 0.61;
const float mAvsLux = 0.59; 


