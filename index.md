This project uses machine learning algorithm (keyword spotting) and inertial measurement unit (accelerometer, gyroscope, and magnetometer) to control a 4 degree of freedom robot arm. The yaw, pitch, and roll of the IMU controls the angular position of the servo motors of the robot arm. Since there is only one IMU, keyword spotting algorithm is used to switch between two servos—shoulder and elbow. A [demonstration video](https://youtu.be/j8ZxJlxczAA) is included to show how the control system works in real life, this is only an early version of the project as this project is still ongoing.

Hopefully, the video has provided an intuition into what the project does. Basically, the orientation of the board (discussed in the Materials section) controls the orientation of the robot arm. In the video, it can be observed that changing the orientation of the board also changes the position of the servos in the arm. The video also shows that when the user utters the keywords "one" or "two", the servo being controlled changes.

Discuss robot arm naming conventions

![image](https://user-images.githubusercontent.com/94373003/178113167-9c787558-f8b9-433b-8120-205c922ef88f.png)

Discuss IMU, yaw, pitch, and roll

Since the focus of this project is on the controls, the mechanical construction of the robot arm is only for demonstration purposes and no scientific methods were performed in constructing it.


# Materials
---
### Arduino Nano 33 BLE Sense

### 5V Servo Motors


---
# Methods
---
### Hardware
---
### Controls
---
#### Keyword Spotting
---
#### IMU
---
# References
