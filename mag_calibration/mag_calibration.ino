/*
Basic_I2C.ino
Brian R Taylor
brian.taylor@bolderflight.com

Copyright (c) 2017 Bolder Flight Systems

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or 
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "MPU9250.h"

// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire,0x68);
int status;

float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values 

float mxb_max = -10000.0, mxb_min = 10000.0, myb_max = -10000.0, myb_min = 10000.0, mzb_max = -10000.0, mzb_min = 10000.0;

float axb, ayb, azb, axs, ays, azs;
float mxb, myb, mzb, mxs, mys, mzs;

float loopCount = 5, sampCount = 1000; // 50 samp/sec -> 20 sec
float biasX = 0, biasY = 0, biasZ = 0;
float chordX = 0, chordY = 0, chordZ = 0, chordAvg = 0;
float scaleX, scaleY, scaleZ;

void setup() {
  // serial to display data
  Serial.begin(115200);

  while(!Serial) {}

  // start communication with IMU 
  status = IMU.begin();
  if (status < 0) {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    while(1) {}
  }
}

void loop() {

  for(int loopCounter = 0; loopCounter < loopCount; loopCounter++)
  {
    mxb_max = -10000.0, mxb_min = 10000.0, myb_max = -10000.0, myb_min = 10000.0, mzb_max = -10000.0, mzb_min = 10000.0;
    for (int sampCounter = 0; sampCounter < sampCount ; sampCounter++)
    {
       // read the sensor
       IMU.readSensor();
      
       my = IMU.getMagY_uT();
       mx = IMU.getMagX_uT();
       mz = IMU.getMagZ_uT();
      
       // Mag calibration snippet
       if (mx > mxb_max)
         mxb_max = mx;
       if (mx < mxb_min)
         mxb_min = mx;
      
       if (my > myb_max)
         myb_max = my;
       if (my < myb_min)
         myb_min = my;  
      
       if (mz > mzb_max)
         mzb_max = mz;
       if (mz < mzb_min)
         mzb_min = mz;  
       Serial.println(String(mxb_min) + "\t" + String(mxb_max) + "\t" + String(myb_min) + "\t" + String(myb_max) + "\t" + String(mzb_min) + "\t" + String(mzb_max));
       delay(20);
    }

    biasX = ((float)loopCounter * biasX + (mxb_max + mxb_min)/2.0)/((float)loopCounter+1.0);
    biasY = ((float)loopCounter * biasY + (myb_max + myb_min)/2.0)/((float)loopCounter+1.0);
    biasZ = ((float)loopCounter * biasZ + (mzb_max + mzb_min)/2.0)/((float)loopCounter+1.0);

    chordX = ((float)loopCounter * chordX + (mxb_max - mxb_min)/2.0)/((float)loopCounter+1.0);
    chordY = ((float)loopCounter * chordY + (myb_max - myb_min)/2.0)/((float)loopCounter+1.0);
    chordZ = ((float)loopCounter * chordZ + (mzb_max - mzb_min)/2.0)/((float)loopCounter+1.0);
    
    Serial.println("Finished trial " + String(loopCounter+1)); 
    delay(1000);
  }

  chordAvg = (chordX + chordY + chordZ)/3.0;
  scaleX = chordAvg / chordX;
  scaleY = chordAvg / chordY;
  scaleZ = chordAvg / chordZ;

  Serial.println("Calibration complete. Results:");
  Serial.println("Biases:\n x = " + String(biasX, 4) + "\t y = " + String(biasY, 4) + "\t z = " + String(biasZ, 4));
  Serial.println("Scales:\n x = " + String(scaleX, 4) + "\t y = " + String(scaleY, 4) + "\t z = " + String(scaleZ, 4));

  // Acceleration calibration snippet

  //Serial.println(String(ax) + "\t" + String(ay) + "\t" + String(az));
  delay(100);

  while(1){}

}

