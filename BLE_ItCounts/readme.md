Health Measurement: It counts
==========

Overview


--------

Our project aims to produce an integrated wearable solution to health maintenance, encompassing measurements related to everyday sedentary posture and counting movements when exercising. This is captured in three distinct modes within our device: Spine curvature measurement,  and the number of steps when jogging/walking.

  

- Spine curvature measurement
  

- Counting number of push-ups (and probably sit-ups)
  

- Number of steps when jogging/walking



Operation
--------




The first of the modes operates using a strain gauge that has a resistance depending on its curvature. By attaching the device on the back of a person, the device can warn the user through an LED when his/her spine has a curvature exceeding a certain threshold, thus not sitting properly. 

A limitation of this design would be the short length of the strain gauge not giving high precision to data collected. This could be easily mitigated with a longer gauge.

Said strain gauge also helps measure the number of steps by attachment to the back of the knee joint, measuring the number of cycles of strain/relaxation. 



Finally, a light sensor (light dependent resistor) is used to detect push-ups with cycles of light intensity measured at the chest.



Software
--------



BLE provides the interfacing between the sensors and the user’s phone, sending real time data for display or analysis. This is done using the mbed platform provided by ARM and the yotta/gcc compiler in C++. 

(Screenshots!)



Analysis and display of data is performed by an application written in Javascript on Evothings. 

No extra software is required on the user end apart from bluetooth detection and connection applications (which should be readily accessible on different platforms, say [nRF Master Control Panel](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en_GB) for Android and [LightBlue](https://itunes.apple.com/gb/app/lightblue-bluetooth-low-energy/id557428110?mt=8) for iOS).




Extensions
--------



Collected data can be used for countless purposes that would prove to be too lengthy for this document. Further improvements could be to integrate our project with the Internet (for example, with social networks) to promote this device as a component of the IoT.


