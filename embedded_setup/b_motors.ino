// Variables
float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values 
double refCommand = 0.0; // Target angle relative to start position (set by determineAngleShift())
float angleShift = 0.0;
int pwmLeft, pwmRight;

double yaw, yawAbs;
float yawSmooth, yawAbsSmooth;
float decayRate = 0.9; // IIR moving average decay rate

double kp = 4.0, ki = 10.0, kd = 0.25; // Ziegler-Nichols kc = 4, Tc = 0.7 s -> kp 2.4, ki 11.43, kd 0.35
double omega_c = 100*2*pi;  // Cutoff frequency was set to 100 Hz
double e_k = 0, e_k_1 = 0, d_k = 0, d_k_1 = 0, integ_k = 0, integ_k_1 = 0, filt = 0, filt_1 = 0;
double speedCommand, speedLeft, speedRight;

double fwdScale = 0.4, turnScale = 0.85;

unsigned long startTime = micros(), dutyTime, execTime, stateTime; // timing variables
unsigned long turnLimit = 5000, straightLimit = 2000; // For the purposes of testing time triggers

// C4V left, C5V right

// Functions

void sensorRead()   // Gather the latest data from the sensor
{
  extern MPU9250 IMU;
  IMU.readSensor();

  ax = IMU.getAccelX_mss();
  ay = IMU.getAccelY_mss();
  az = IMU.getAccelZ_mss();
  mx = IMU.getMagX_uT();
  my = IMU.getMagY_uT();
  mz = IMU.getMagZ_uT();  
}

void motorUpdate()    // Compute current yaw and generate feedback command
{
  execTime = micros();
  sensorRead();
  yaw = computeYaw(angleShift);
  yawSmooth = (1 - decayRate)*yaw + decayRate*yawSmooth;

  speedCommand = generateCommand(yawSmooth);
  
  if (speedCommand > 1){ speedCommand = 1; } 
  else if (speedCommand < -1) { speedCommand = -1; } // PWM bounds

  //Serial.println(String(speedCommand, 4));
  
  dutyTime = micros() - startTime;
}

void turn()   // Correct the heading to the reference angle (refCommand)
{
  Serial.println("Executed turn");
  motorUpdate();
  if (dutyTime >= 20000) //one PWM duty cycle have passed
  {
    speedLeft = speedCommand * 750.0 * -1.0 * turnScale;
    speedRight = speedCommand * 750.0 *  turnScale;
    pwmLeft = (int)speedLeft + 2250;
    pwmRight = (int)speedRight + 2250;
    FTM0_C4V = pwmLeft;
    FTM0_C5V = pwmRight;
    Serial.println("turn "+ String(speedCommand, 4) + "\t pwm left " + String(pwmLeft) + "\t pwm right " + String(pwmRight));
    startTime = micros();
  }
}

void forward()    // Forward motion in feedback loop
{
  Serial.println("Executed forward");
	
/*	feedback loop
	motorUpdate(); 
  if (dutyTime >= 20000) //one PWM duty cycle have passed
  {
    speedLeft = (0.45 - speedCommand * fwdScale)*750.0;
    speedRight = (0.45 + speedCommand * fwdScale)*750.0;
    pwmLeft = (int)speedLeft + 2250;
    pwmRight = (int)speedRight + 2250;
    FTM0_C4V = pwmLeft;
    FTM0_C5V = pwmRight;
    Serial.println("fwd "+ String(speedCommand, 4) + "\t pwm left " + String(pwmLeft) + "\t pwm right " + String(pwmRight));
    startTime = micros();
  }

*/

  FTM0_C4V = 2620; // discrepancy is to fix small error
  FTM0_C5V = 2600;
}

void reverse()    // Backward motion in feedback loop
{
/* feedback loop
  motorUpdate();
  if (dutyTime >= 20000) //one PWM duty cycle have passed
  {
    speedLeft = (-0.45 - speedCommand * fwdScale)*750.0;
    speedRight = (-0.45 + speedCommand * fwdScale)*750.0;
    pwmLeft = (int)speedLeft + 2250;
    pwmRight = (int)speedRight + 2250;
    FTM0_C4V = pwmLeft;
    FTM0_C5V = pwmRight;

    startTime = micros();
  }
*/

  FTM0_C4V = 1880;
  FTM0_C5V = 1900;
}
void motorStop()   // Stop vehicle immediately
{
	FTM0_C4V = 2250;
	FTM0_C5V = 2250;
}

void determineAngleShift(void)    // Determine angle to reset frame of reference
{
  Serial.println("Changing angle shift");
  
  for (int i = 0; i < 100; i++)
  {
    sensorRead();  
    yawAbs = computeYaw(0.0);
    yawAbsSmooth = (1 - decayRate)*yawAbs + decayRate*yawAbsSmooth;
    
    delay(10);
  }
  angleShift = -1.0*yawAbsSmooth;
  Serial.println("Angle set to " + String(angleShift));
}

double computeYaw(float angle)  // Compute the yaw relative to the given angle (0 Absolute, angleShift for vehicle reference frame)
{

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

float generateCommand(double current)   // PID controller to generate the speed control command
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
  // Serial.println(String(e_k, 4));
  return 2.5*atan2(pidOut, 1)/pi; // for support at lower speeds
}



