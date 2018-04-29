#include "MPU9250.h"

// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire,0x68);
int status;

const float pi = 3.14;

float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values 
//float ax_r, ay_r, mx_r, my_r;
float mxb_max = -10000.0, mxb_min = 10000.0, myb_max = -10000.0, myb_min = 10000.0, mzb_max = -10000.0, mzb_min = 10000.0;
float magBias[3] = {9.673, 14.4515, -15.33}, magScale[3] = {0.9537, 1.0218, 1.028}; // Location specific

float axb, ayb, azb, axs, ays, azs;
float mxb, myb, mzb, mxs, mys, mzs;

int counter = 0;
int upto = 500;
float angleShift = 0.0;

float yawSmooth, yawAbsSmooth;
float decayRate = 0.9;

unsigned long startTime, elapsedTime, execTime, turnTime;

double yaw, yawAbs;
double refCommand = 0;//2.0*pi/4.0; // heading variable to be targeted

double kp = 7.0, ki = 12.0, kd = 0.25; // Ziegler-Nichols kc = 4, Tc = 0.7 s -> kp 2.4, ki 11.43, kd 0.35
double omega_c = 100*2*pi;  // Cutoff frequency was set to 10 Hz
double e_k = 0, e_k_1 = 0, d_k = 0, d_k_1 = 0, integ_k = 0, integ_k_1 = 0, filt = 0, filt_1 = 0;
double speedCommand;
int state = 1; // 1 keep straight, 2 turn


void setup() {
  // serial to display data
  Serial.begin(115200);

  // PWM channel setup
  PORTD_PCR4 |= (1U << 10); // mux to FTM0_C4 for pin 6 - left
  PORTD_PCR5 |= (1U << 10); // mux to FTM0_C5 for pin 20 - right

  FTM0_SC |= _BV(3); //Status control register , This selects system clock
  FTM0_SC &= ~(_BV(4));

  FTM0_MODE |= (1U << 2); //Mode register
  FTM0_MODE |= _BV(1); // Enables FTM, this must come after the line above since this bit is write protected.

  FTM0_SC |= 0x4; //set bit 0 1, USING 16 BIT RESOLUTION
  FTM0_C4SC |= (1U << 3) | (1U << 5); // channel 4
  FTM0_C5SC |= (1U << 3) | (1U << 5); // channel 5

  FTM0_CNTIN = 0; //Initial value is 0 for PWM counter, also edge align mode MUST use CNTIN by design
  FTM0_MOD = 29999;//counts up to MOD then counts back up from 0, this is the period of cycle

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

  // Accelerometer calibration
  int accStatus = IMU.calibrateAccel(); // This doesn't work
  Serial.println("Acc calib stat: " + String(accStatus));
  axb = IMU.getAccelBiasX_mss();
  ayb = IMU.getAccelBiasY_mss();
  azb = IMU.getAccelBiasZ_mss();

  axs = IMU.getAccelScaleFactorX();
  ays = IMU.getAccelScaleFactorY();
  azs = IMU.getAccelScaleFactorZ();  
  Serial.println("axb:\t" + String(axb) + "\t axs:\t" + String(ays) + "\t ayb:\t" + String(ayb) + "\t ays:\t" + String(ays) + "\t azb:\t" + String(azb) + "\t azs:\t" + String(azs) );

  // Magnetometer calibration
  IMU.setMagCalX(magBias[0], magScale[0]);
  IMU.setMagCalY(magBias[1], magScale[1]);
  IMU.setMagCalZ(magBias[2], magScale[2]);
  
  delay(100);
  
  // Sanity check
  mxb = IMU.getMagBiasX_uT();
  myb = IMU.getMagBiasY_uT();
  mzb = IMU.getMagBiasZ_uT();

  mxs = IMU.getMagScaleFactorX();
  mys = IMU.getMagScaleFactorY();
  mzs = IMU.getMagScaleFactorZ();  

  Serial.println("Magnetometer Calibration Sanity Check:");
  Serial.println("mxb:\t" + String(mxb) + "\t mxs:\t" + String(mxs) + "\t ayb:\t" + String(myb) + "\t ays:\t" + String(mys) + "\t azb:\t" + String(mzb) + "\t azs:\t" + String(mzs) );

  // Initial angle shift determination
  determineAngleShift();
  
  startTime = micros();
  turnTime = millis();

}

void loop() {

  execTime = micros();
  
  // read the sensor
  IMU.readSensor();

  ax = IMU.getAccelX_mss();
  ay = IMU.getAccelY_mss();
  az = IMU.getAccelZ_mss();
  mx = IMU.getMagX_uT();
  my = IMU.getMagY_uT();
  mz = IMU.getMagZ_uT();

  yaw =  computeYaw(ax, ay, az, mx, my, mz, angleShift);
  yawAbs = computeYaw(ax, ay, az, mx, my, mz, 0.0);

  // filtering
  yawSmooth = (1 - decayRate)*yaw + decayRate*yawSmooth;
  yawAbsSmooth = (1 - decayRate)*yawAbs + decayRate*yawAbsSmooth;
//  Serial.println(String(yawSmooth) + "\t\t" + String(yawAbsSmooth));

  speedCommand = generateCommand(yawSmooth);
  elapsedTime = micros() - startTime;
  
  if (elapsedTime >= 20000) //40ms interval for pwm
  {
    
    if (speedCommand > 1){ speedCommand = 1; } 
    else if (speedCommand < -1) { speedCommand = -1; } // prevent PWM overflow issues

    Serial.println(String(speedCommand));

    switch(state)
    {
      case 1:
        if (speedCommand > 1) 
        {
          setLeftVel(0.5-0.25*speedCommand);
          setRightVel(0.5);
        } else 
        {
          setLeftVel(0.5);
          setRightVel(0.5+0.25*speedCommand);
        }
      break;
      case 2:
        setLeftVel(-1.0*speedCommand);
        setRightVel(speedCommand);
      break;
    }
    startTime = micros();  
  }

  //Serial.println("Elapsed: " + String(micros() - execTime));

  if(millis() - turnTime >= 3000) // change state
  {
    setLeftVel(0);
    setRightVel(0);
    if (state == 1) // it was going straight
    {
      refCommand = pi/2.0; // left 90 deg turn
      state = 2;
    } else if (state == 2)
    {
      refCommand = 0;
      state = 1;
    }

    // Reset PID states
    e_k = 0;
    e_k_1 = 0;
    d_k = 0; 
    d_k_1 = 0; 
    integ_k = 0; 
    integ_k_1 = 0; 
    filt = 0; 
    filt_1 = 0;
    
    determineAngleShift();
    turnTime = millis();
  }
}

double computeYaw(float ax, float ay, float az, float mx, float my, float mz, float angle){

  // Rotations
  float mx_r = mx*cos(angle) - my*sin(angle);
  float my_r = my*cos(angle) + mx*sin(angle);
  float ax_r = ax*cos(angle) - ay*sin(angle);
  float ay_r = ay*cos(angle) + ax*sin(angle);
  
  double pitch = atan2 (ay_r ,( sqrt ((ax_r * ax_r) + (az * az))));
  double roll = atan2(-ax_r ,( sqrt((ay_r * ay_r) + (az * az))));
  float Yh = (my_r * cos(roll)) - (mz * sin(roll));
  float Xh = (mx_r * cos(pitch))+(my_r * sin(roll)*sin(pitch)) + (mz * cos(roll) * sin(pitch));

  return atan2(Yh, Xh);
}

void setLeftVel(float vel)
{
  float dir = vel * 750;
  int pwmVal = (int)dir + 2250;  
  FTM0_C4V = pwmVal;
}

void setRightVel(float vel)
{
  float dir = vel * 750;
  int pwmVal = (int)dir + 2250;
  FTM0_C5V = pwmVal;
}

float generateCommand(double current)
{
  double error = refCommand - current; // right turns are negative - a command of pi/2 generates pos
  double T = (micros() - execTime)/(1000000.0); // Sampling period
  
  e_k = error / (pi / 2); // 90 degree turns generate the most error - hence the highest possible turn at once
  
  // computations for PID
  d_k = -d_k_1 + (e_k - e_k_1)*(2/T);
  filt = ((2 - T*omega_c)/(2 + T*omega_c))*filt_1 + T*omega_c*(d_k + d_k_1)/(2+T*omega_c);
  integ_k = integ_k_1 + (e_k + e_k_1)*(T/2);

  double pidOut = e_k * kp + kd * filt + ki * integ_k;
  //Serial.println(String(T) + "\t" + String(pidOut) + "\t" + String(e_k) + "\t" + String(filt)  + "\t" + String(integ_k));
  // updates
  e_k_1 = e_k;
  d_k_1 = d_k;
  filt_1 = filt;

  if (pidOut > -1 && pidOut < 1) // Integral anti wind-up condition
    integ_k_1 = integ_k;

  return 2.5*atan2(pidOut, 1)/pi; // for support at lower speeds
}

void determineAngleShift(void)
{
  Serial.println("Changing angle shift");
  delay(500);
  
  for (int i = 0; i < 100; i++)
  {
    IMU.readSensor();

    ax = IMU.getAccelX_mss();
    ay = IMU.getAccelY_mss();
    az = IMU.getAccelZ_mss();
    mx = IMU.getMagX_uT();
    my = IMU.getMagY_uT();
    mz = IMU.getMagZ_uT();
    yawAbs = computeYaw(ax, ay, az, mx, my, mz, 0.0);
    yawAbsSmooth = (1 - decayRate)*yawAbs + decayRate*yawAbsSmooth;
    
    delay(10);
  }
  angleShift = -1.0*yawAbsSmooth;
  Serial.println("Angle set to " + String(angleShift));
}

