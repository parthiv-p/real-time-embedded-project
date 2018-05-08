// Variables

float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values 

float GyroMeasError = PI * (0.0f / 180.0f);   // gyroscope measurement error in rads/s (start at 40 deg/s)
float GyroMeasDrift = PI * (500.0f / 180.0f); // gyroscope measurement drift in rad/s/s (start at 0.0 deg/s/s)

float beta = sqrt(3.0f / 4.0f) * GyroMeasError;   // compute beta
float zeta = sqrt(3.0f / 4.0f) * GyroMeasDrift;   // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value

float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};    // vector to hold quaternion
float deltat;

float yaw;

double kp = 7, ki = 10.0, kd = 0.45; // Ziegler-Nichols kc = 4, Tc = 0.7 s -> kp 2.4, ki 11.43, kd 0.35
double omega_c = 100*2*PI;  // Cutoff frequency was set to 100 Hz
double e_k = 0, e_k_1 = 0, d_k = 0, d_k_1 = 0, integ_k = 0, integ_k_1 = 0, filt = 0, filt_1 = 0;
double speedCommand, speedLeft, speedRight;
double refCommand = 0.0; // Target angle for turns
int pwmLeft, pwmRight;

double fwdScale = 0.4, turnScale = 0.5, base = 0.6;

unsigned long startTime = micros(), dutyTime, execTime, turnTime, lastUpdate = 0; // timing variables
unsigned long settleTime = 1000, straightLimit = 2000; // For the purposes of testing time triggers

// C4V left, C5V right

// Functions

void sensorRead()   // Gather the latest data from the sensor
{
  extern MPU9250 IMU;
  IMU.readSensor();

  ax = IMU.getAccelX_mss();
  ay = IMU.getAccelY_mss();
  az = IMU.getAccelZ_mss();
  gx = IMU.getGyroX_rads();
  gy = IMU.getGyroY_rads();
  gz = IMU.getGyroZ_rads();
  mx = IMU.getMagX_uT();
  my = IMU.getMagY_uT();
  mz = IMU.getMagZ_uT();

  execTime = micros(); 
  deltat = ((execTime - lastUpdate)/1000000.0f);
  lastUpdate = execTime;

  MadgwickQuaternionUpdate(ax, ay, az, gx, gy, gz,  my,  mx, mz);
  yaw   = atan2(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]); 
}

void update_command()    // Compute current yaw and generate feedback command
{
  sensorRead();
  speedCommand = PID_control(yaw);
  
  if (speedCommand > 1){ speedCommand = 1; } 
  else if (speedCommand < -1) { speedCommand = -1; } // PWM bounds

  //Serial.println(String(speedCommand, 4));
  
  dutyTime = micros() - startTime;
}

void correct_heading()   // Correct the heading to the reference angle (refCommand)
{
  turnTime = millis();
  while(millis() - turnTime <= settleTime)
  {
    update_command();
    if (dutyTime >= 20000) //one PWM duty cycle have passed
    {
      speedLeft = speedCommand * 750.0 * -1.0 * turnScale;
      speedRight = speedCommand * 750.0 *  turnScale;
      pwmLeft = (int)speedLeft + 2250;
      pwmRight = (int)speedRight + 2250;
      FTM0_C4V = pwmLeft;
      FTM0_C5V = pwmRight;
      //Serial.println("turn "+ String(speedCommand, 4) + "\t pwm left " + String(pwmLeft) + "\t pwm right " + String(pwmRight));
      startTime = micros();
    }
  }
  motorStop();
  reset_quaternion();
  refCommand = 0;
}

void forward()    // Forward motion in feedback loop
{	
	update_command(); 
  if (dutyTime >= 20000) //one PWM duty cycle have passed
  {
    speedLeft = (base - speedCommand * fwdScale)*750.0;
    speedRight = (base + speedCommand * fwdScale)*750.0;
    pwmLeft = (int)speedLeft + 2250;
    pwmRight = (int)speedRight + 2250;
    FTM0_C4V = pwmLeft;
    FTM0_C5V = pwmRight;
    //Serial.println("yaw:  " + String(yaw) + "\t fwd "+ String(speedCommand, 4) + "\t pwm left " + String(pwmLeft) + "\t pwm right " + String(pwmRight));
    startTime = micros();
  }
  
}

void reverse()    // Backward motion in feedback loop
{
  update_command();
  if (dutyTime >= 20000) //one PWM duty cycle have passed
  {
    speedLeft = (-1.0*base - speedCommand * fwdScale)*750.0;
    speedRight = (-1.0*base + speedCommand * fwdScale)*750.0;
    pwmLeft = (int)speedLeft + 2250;
    pwmRight = (int)speedRight + 2250;
    FTM0_C4V = pwmLeft;
    FTM0_C5V = pwmRight;

    startTime = micros();
  }

}

void right(){
   FTM0_C4V = 2000;
   FTM0_C5V = 2500;
}

void left(){
   FTM0_C4V = 2500;
   FTM0_C5V = 2000;
}

void motorStop()   // Stop vehicle immediately
{
	FTM0_C4V = 2250;
	FTM0_C5V = 2250;
  delay(20);
  resetPID();
}

float PID_control(double current)   // PID controller to generate the speed control command
{
  double e_k = refCommand - current; // right turns are negative - a command of pi/2 generates pos
  double T = deltat; // Sampling period
  // Serial.println(String(e_k, 4));
  
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
    
  return 2.5*atan2(pidOut, 1)/PI; // for support at lower speeds
}

void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
    float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
    float norm;
    float hx, hy, _2bx, _2bz;
    float s1, s2, s3, s4;
    float qDot1, qDot2, qDot3, qDot4;

    // Auxiliary variables to avoid repeated arithmetic
    float _2q1mx;
    float _2q1my;
    float _2q1mz;
    float _2q2mx;
    float _4bx;
    float _4bz;
    float _2q1 = 2.0f * q1;
    float _2q2 = 2.0f * q2;
    float _2q3 = 2.0f * q3;
    float _2q4 = 2.0f * q4;
    float _2q1q3 = 2.0f * q1 * q3;
    float _2q3q4 = 2.0f * q3 * q4;
    float q1q1 = q1 * q1;
    float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    float q1q4 = q1 * q4;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q2q4 = q2 * q4;
    float q3q3 = q3 * q3;
    float q3q4 = q3 * q4;
    float q4q4 = q4 * q4;

    // Normalise accelerometer measurement
    norm = sqrtf(ax * ax + ay * ay + az * az);
    if (norm == 0.0f) return; // handle NaN
    norm = 1.0f/norm;
    ax *= norm;
    ay *= norm;
    az *= norm;

    // Normalise magnetometer measurement
    norm = sqrtf(mx * mx + my * my + mz * mz);
    if (norm == 0.0f) return; // handle NaN
    norm = 1.0f/norm;
    mx *= norm;
    my *= norm;
    mz *= norm;

    // Reference direction of Earth's magnetic field
    _2q1mx = 2.0f * q1 * mx;
    _2q1my = 2.0f * q1 * my;
    _2q1mz = 2.0f * q1 * mz;
    _2q2mx = 2.0f * q2 * mx;
    hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 + _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
    hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 + my * q3q3 + _2q3 * mz * q4 - my * q4q4;
    _2bx = sqrtf(hx * hx + hy * hy);
    _2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 + _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
    _4bx = 2.0f * _2bx;
    _4bz = 2.0f * _2bz;

    // Gradient decent algorithm corrective step
    s1 = -_2q3 * (2.0f * q2q4 - _2q1q3 - ax) + _2q2 * (2.0f * q1q2 + _2q3q4 - ay) - _2bz * q3 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q4 + _2bz * q2) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s2 = _2q4 * (2.0f * q2q4 - _2q1q3 - ax) + _2q1 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q2 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + _2bz * q4 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q3 + _2bz * q1) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q4 - _4bz * q2) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s3 = -_2q1 * (2.0f * q2q4 - _2q1q3 - ax) + _2q4 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q3 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + (-_4bx * q3 - _2bz * q1) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q2 + _2bz * q4) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q1 - _4bz * q3) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s4 = _2q2 * (2.0f * q2q4 - _2q1q3 - ax) + _2q3 * (2.0f * q1q2 + _2q3q4 - ay) + (-_4bx * q4 + _2bz * q2) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    norm = sqrtf(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4);    // normalise step magnitude
    norm = 1.0f/norm;
    s1 *= norm;
    s2 *= norm;
    s3 *= norm;
    s4 *= norm;

    // Compute rate of change of quaternion
    qDot1 = 0.5f * (-q2 * gx - q3 * gy - q4 * gz) - beta * s1;
    qDot2 = 0.5f * (q1 * gx + q3 * gz - q4 * gy) - beta * s2;
    qDot3 = 0.5f * (q1 * gy - q2 * gz + q4 * gx) - beta * s3;
    qDot4 = 0.5f * (q1 * gz + q2 * gy - q3 * gx) - beta * s4;

    // Integrate to yield quaternion
    q1 += qDot1 * deltat;
    q2 += qDot2 * deltat;
    q3 += qDot3 * deltat;
    q4 += qDot4 * deltat;
    norm = sqrtf(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);    // normalise quaternion
    norm = 1.0f/norm;
    q[0] = q1 * norm;
    q[1] = q2 * norm;
    q[2] = q3 * norm;
    q[3] = q4 * norm;

}

void resetPID()
{
    e_k = 0;
    e_k_1 = 0;
    d_k = 0; 
    d_k_1 = 0; 
    integ_k = 0; 
    integ_k_1 = 0; 
    filt = 0; 
    filt_1 = 0;
}

void reset_quaternion()
{
  q[0] = 1.0f;
  q[1] = 0.0f;
  q[2] = 0.0f;
  q[3] = 0.0f;
}

