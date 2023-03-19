//Arduino Nano 33 BLE peripheral & server
#include <math.h>
#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

//arm length values
#define ARM_LENGTH_1 85
#define ARM_LENGTH_2 105

//variable for x and y position of the end effector (technically yz or xz plane to be used since 3d but xy is used for simplicity)
float x = 0.0;
float y = 0.0;

//variable for angle of shoulder and elbow servo
float theta1 = 0.0;
float theta2 = 0.0;

//variable to simplify computation of theta1 and theta2
float K1 = 0.0; //for theta2

//structs for motor and imu control
struct Euler {
  float phi, theta, psi; //roll, pitch, yaw
};
struct Accelerometer {
  float x, y, z;
  Euler eul;
};
struct Magnetometer {
  float x, y, z;
  Euler eul;
};
struct Gyroscope {
  float x, y, z;
  Euler eul;
};
struct Fusion {
  Euler eul;
};

//instantatiate struct variables
Accelerometer acce;
Magnetometer magne;
Gyroscope gyro;
Fusion fused;

//variables used in sensor fusion
float dt;
unsigned long millis_old;
float weights = 0.05;

//buffer to be used for averaging/filtering
float phiBuffer;
float thetaBuffer;
float psiBuffer;

//BLE Service
BLEService servoAngleService("0a88bb9a-9790-11ed-a8fc-0242ac120002");
BLEFloatCharacteristic waistAngleCharacteristic("0a88bb9b-9790-11ed-a8fc-0242ac120002", BLERead | BLENotify);
BLEFloatCharacteristic shoulderAngleCharacteristic("0a88bb9c-9790-11ed-a8fc-0242ac120002", BLERead | BLENotify);
BLEFloatCharacteristic elbowAngleCharacteristic("0a88bb9d-9790-11ed-a8fc-0242ac120002", BLERead | BLENotify);
BLEFloatCharacteristic wristAngleCharacteristic("0a88bb9e-9790-11ed-a8fc-0242ac120002", BLERead | BLENotify);

//map for float
float fmap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}//fmap

//function for converting accelerometer data to x and y
void pitchToXY(float theta, float* x, float* y) {
  const float maxX = 170.0; //manual calibration via experiment.
  const float maxY = 160.0;
  
  //estimate for now, from experimentation and obervation later
  const float maxTheta = -40.0 * M_PI / 180;
  const float minTheta = 40.0 * M_PI / 180; //for negative theta
  const float middleTheta = (maxTheta+minTheta)/2;
  //set dead region
  const int deadRegion = 5 * M_PI / 180; //rads


  //ignore dead region readings

    //scale pitch to z value if theta is positive
    //rotate hands upwards
if (theta < middleTheta) {
    if(theta<maxTheta){
        theta=maxTheta;
    }
    //reset y
    *y=0.0;
    //map function rewritten for floats
    *x = fmap(theta, -deadRegion, maxTheta, 55, maxX); //scale decreasing theta reading to increasing x value.
    
  }
  else if (theta > middleTheta) {
    if(theta>minTheta){
        theta=minTheta;
    }
    //reset x
    *x=0.0;
    //theta already negative, no need to put negatives on other parts of equation
     *y = fmap(theta, deadRegion, minTheta, 55, maxY); //scale decreasing theta reading to increasing y value.
    }
}//pitchToXY

//some function for sending data to BLE


void setup() {
  Serial.begin(115200);
  Serial.println("Serial monitor initiated");
  //while(!Serial){};

  //blocking code until BLE begins
  if (!BLE.begin()) {
    while (1);
  }
  //*for imu and motor
  IMU.begin();
  Serial.begin(115200);
  millis_old = millis();

  //*calibrate IMU (constants from DIY calibration from: https://github.com/FemmeVerbeek/Arduino_LSM9DS1/blob/master/Getting%20Started.md
  IMU.setAccelFS(3);
  IMU.setAccelODR(5);
  IMU.setAccelOffset(-0.009047, -0.028952, -0.007588);
  IMU.setAccelSlope (0.999976, 0.988901, 0.998409);

  IMU.setGyroFS(2);
  IMU.setGyroODR(5);
  IMU.setGyroOffset (-0.144623, 0.880035, 0.643738);
  IMU.setGyroSlope (1.000000, 1.000000, 1.000000);

  //*magnetic field strength near equator: 30.5 microT
  IMU.setMagnetFS(0);
  IMU.setMagnetODR(8);
  IMU.setMagnetOffset(-4.344482, 9.343872, 11.042480);
  IMU.setMagnetSlope (2.061886, 2.008697, 2.280857);

  IMU.readMagneticField(magne.x, magne.y, magne.z);
  gyro.eul.phi = -3.0, gyro.eul.theta = 3.0;
  fused.eul.phi = 0., fused.eul.theta = 0.;

  gyro.eul.psi = atan2(magne.y, magne.x) * 180 / M_PI;
  fused.eul.psi = atan2(magne.y, magne.x) * 180 / M_PI;

  //setup BLE
  BLE.setLocalName("SmartGlove");
  BLE.setAdvertisedService(servoAngleService);
  servoAngleService.addCharacteristic(shoulderAngleCharacteristic);
  servoAngleService.addCharacteristic(elbowAngleCharacteristic);
  BLE.addService(servoAngleService);

  //advertise BLE to central devices
  BLE.advertise();
}


/**
   @brief      Arduino main function. Runs the inferencing loop.
*/
void loop(){
  //BLE code
  //connect a central device
  BLEDevice central = BLE.central();

  //turn on LED if connected
  if (central) {
    digitalWrite(LED_BUILTIN, HIGH);

    //write data to be read by ESP32 when connnected to central device
    while (central.connected()) {
      //*for imu and motor
      IMU.readAcceleration(acce.x, acce.y, acce.z);
      IMU.readGyroscope(gyro.x, gyro.y, gyro.z);
      IMU.readMagneticField(magne.x, magne.y, magne.z);
    
      //accelerometer trigo
      acce.eul.phi = -atan2(acce.y, acce.z) * 180 / M_PI; //negative for convention NED
      acce.eul.theta = atan2(acce.x, acce.z) * 180 / M_PI;
    
      //integrate angular velocity
      dt = (millis() - millis_old) / 1000.;
      millis_old = millis();
    
      gyro.eul.phi = gyro.eul.phi + gyro.x * dt;
      gyro.eul.theta = gyro.eul.theta + gyro.y * dt;
      gyro.eul.psi = gyro.eul.psi - gyro.z * dt;
    
      magne.eul.psi = atan2(magne.y, magne.x) * 180 / M_PI; //xy only but IMU seems to compensate it already
    
      fused.eul.phi = (fused.eul.phi + gyro.x * dt) * (1 - weights) + acce.eul.phi * weights;
      fused.eul.theta = (fused.eul.theta + gyro.y * dt) * (1 - weights) + acce.eul.theta * weights;
      fused.eul.psi = (fused.eul.psi - gyro.z * dt) * (1 - weights) + magne.eul.psi * weights;
    
      //delay 4 samples then average 
      for (byte i = 0; i < 4; i++) {
        phiBuffer += fused.eul.phi;
        thetaBuffer += fused.eul.theta;
        psiBuffer += fused.eul.psi;
      }
      phiBuffer /= 4;
      thetaBuffer /= -4;
      psiBuffer /= -4;
      //convert pitch to xy and z position of the end effector
      pitchToXY(thetaBuffer*M_PI/180, &x, &y);
      y=abs(y);
      //solve inverse kinematics constants
      K1=((x*x) + (y*y) - ARM_LENGTH_1*ARM_LENGTH_1 - ARM_LENGTH_2*ARM_LENGTH_2) / (2 * ARM_LENGTH_1*ARM_LENGTH_2); 
     
      //code for inverse kinematic, derived using geometry
      theta2 = acos(K1);
      theta1 = atan2(y, x) - atan2(ARM_LENGTH_2*sin(theta2),ARM_LENGTH_1+ARM_LENGTH_2*cos(theta2));
      //convert theta2 and theta1 to degrees
      theta2*=180/M_PI;
      theta1*=180/M_PI;
      
      waistAngleCharacteristic.writeValue(phiBuffer);
      shoulderAngleCharacteristic.writeValue(theta1);
      elbowAngleCharacteristic.writeValue(theta2);
      wristAngleCharacteristic.writeValue(psiBuffer);
      Serial.print("theta:");
      Serial.print(thetaBuffer);
      Serial.print(",");
      Serial.print("Waist Angle:");
      Serial.print(phiBuffer);
      Serial.print(",");
      Serial.print("Shoulder Angle:");
      Serial.print(theta1);
      Serial.print(",");
      Serial.print("Elbow Angle:");
      Serial.print(theta2);
      Serial.print(",");
      Serial.print("Wrist Angle:");
      Serial.println(psiBuffer);
      delay(1);
    }
  }
  //turn off LED if disconnected
  digitalWrite(LED_BUILTIN, LOW);
}
