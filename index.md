This project uses an inertial measurement unit (accelerometer, gyroscope, and magnetometer) to control a 4 degree of freedom robot arm. The yaw, pitch, and roll of the IMU controls the angular position of the servo motors of the robot arm. Since there is only one IMU, keyword spotting algorithm is used to switch between two servos—shoulder and elbow. A [demonstration video](https://youtu.be/j8ZxJlxczAA) is included to show how the control system works in real life, this is only an early version of the project as this project is still ongoing.

Hopefully, the video has provided an intuition into what the project does. Basically, the orientation of the board (discussed in the Materials section) controls the orientation of the robot arm. In the video, it can be observed that changing the orientation of the board also changes the position of the servos in the arm. The video also shows that when the user utters the keywords "one" or "two", the servo being controlled changes.

To provide a better discussion, the terms used will also be discussed, albeit shortly. First off are the angles used such as yaw, pitch, and roll. Basically, these terms are used to describe the motion of the robot arm. From the figure below, the angles are basically just rotations in different axes, roll is rotation in the x-axis, pitch in the y-axis, and yaw in the z-axis. A more intuitive discussion is just using logic in describing them. Roll can be described as the rotation a person does, when they are on fire and they are trying to extinguish that fire by "rolling" on the ground. Obviously, a person on fire would roll side to side, and not forward. Pitch can be described as the motion a person does with "pitching" a ball. For yaw, since the remaining two are discussed, it just leaves the rotation for z-axis.

![image](https://user-images.githubusercontent.com/94373003/179172847-37362b06-c61e-4b38-af5f-744a9e4e9993.png)


Photo from [1]


For the naming convention of the robot arm joints. Typically, the joints are named as the body parts of humans. From the photo below, the servo at the base is named "waist", the next two are the "shoulder" and "elbow", while the last one is the "wrist". Their names are quite intuitive, as they perform functions similar to their counterparts (but not completely, as shoulders can yaw, pitch, and roll).

![image](https://user-images.githubusercontent.com/94373003/178113167-9c787558-f8b9-433b-8120-205c922ef88f.png)

Next is keyword spotting, which is a Machine Learning algorithm that looks for certain "keywords" in a stream of live audio. Upon detection of certain keywords—in this project, are "one" and "two"—an action will be performed depending on the algorithm it is integrated into. In this project, uterring "one" shifts the control to the shoulder servo, while uttering "two" shifts to the elbow servo.

Now that the terms are discussed, next is how the project works (simplified). The 9-DOF Inertial Measurement Unit computes the orientation of the board by fusing the outputs of three sensors—accelerometer, gyroscope, and magnetometer. All sensors in the IMU are 3-axes, measuring linear acceleration, angular velocity, and magnetic field respectively. The algorithm used in integrating their outputs is discussed in the Methods section. For now, we treat the IMU as black box. After the IMU fuses all the outputs of the three sensors—the yaw, pitch, and roll angle is obtained. These are then passed onto the servos, which means that the angle of the IMU will then be the angle of the servos. As stated before, the project is restricted into using one IMU, thus controlling two servos with only one pitch angle is impossible. Thus, a keyword spotting algorithm (basically speech recognition) is implemented to control the shoulder and the elbow servos.

Since the focus of this project is on the controls, the mechanical construction of the robot arm is only for demonstration purposes and no scientific methods were performed in constructing it.

WORK IN PROGRESS

# Materials
---
### Arduino Nano 33 BLE Sense

Arduino Nano 33 BLE Sense (Arduino 33) is an AI-enabled board—where AI-enabled means that it is compatible with existing AI software tools such as TensorFlow Lite for Microcontroller and Edge Impulse, etc.—operating at 3.3 V. It has multiple built-in sensors such as 9-axis inertial sensor, humidity and temperature sensor, barometric sensor, microphone, gesture sensor, proximity sensor, and light color and intensity sensor. It also has a Bluetooth Low Energy (BLE) chipset and can also be used as a Bluetooth host and client.

### 5V Servo Motors

SG90 Servo Motor is a servo motor with torque of 2.5kg/cm. It can rotate at a maximum of 180 degrees, at speed of 0.1s/60 degrees.

---
# Methods
---
### Hardware
---


### Controls
---
#### Keyword Spotting
---
Keyword spotting is a Tiny Machine 
#### IMU
---
# References

[1] https://erc-bpgc.github.io/blog/blog/robotic_arms/
