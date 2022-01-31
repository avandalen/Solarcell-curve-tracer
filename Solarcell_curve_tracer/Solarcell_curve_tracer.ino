//Version 

#include <Streaming.h>
#include "FrequencyTimer2.h"
#include "Albert.h"
#include "CurveTracer.h"
#include "Definitions.h"

CurveTracer curveTracer; 

// JUST FOR TESTS
#include <StopWatch.h>
#include <Metro.h>
Stopwatch stopWatch(micros); 
Metro stopWatchTimer(200);

void setup() 
{ Serial.begin(9600);
  curveTracer.init(); 
  FrequencyTimer2::setPeriod(timer2Setting); // static member functions without object
  //Serial << F("Available RAM: ") << availableRAM(); 
}

void loop()
{ int mode = 3;   
  switch(mode)
  { case 1: curveTracer.run(0); break;
    case 2: curveTracer.run(1); break; // with curve first
    case 3: curveTracer.calibrateU(); break;
    case 4: curveTracer.calibrateI(); break;
    case 5: curveTracer.calibrateMains(); break;
    case 6: curveTracer.calibrateT(); break;
  }  
}

void timer2ISR() 
{ //stopWatch.start();
  curveTracer.update(); // 700us 
  //stopWatch.stop(); 
}
