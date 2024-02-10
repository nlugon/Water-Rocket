# Water Rocket
Built and launched a water rocket as part of the ENG-510 Space Propulsion course at EPFL, in teams of 5 students (was in charge of Recovery Subsystem). Single stage launcher with propulsion system based on pressurized air and water. To ensure safe recovery after launch, a parachute is deployed after apogee detection thanks to onboard avionics.

[Project report](https://drive.google.com/file/d/1PPxtXIhh1XiugEDdZYH2G9OJz606AQhE/view) and [launch video (on Youtube)](https://youtu.be/lImlgpacwuk?si=nWyseksTFAoCjJkB)



<div align="center">
  <img width="326" alt="Rocket" src="https://github.com/nlugon/Water-Rocket/assets/112929907/b1b176cc-ecb1-41d2-9b7f-bc4b59117497">
  <img width="400" alt="Launch" src="https://github.com/nlugon/Water-Rocket/assets/112929907/3cc18ffc-9f48-4cbc-ae5f-fb8604ae67cd">
</div>

## Avionics

Onboard avianoics based on Arduino Nano MCU, with BME280 barometer for altitude estimation and onboard SD card for data logging. A servo motor is used to trigger the parachute mechanism. A piezo buzzer is also included for pre-launch checks. All avionics are powered by a 9V alkaline battery. Total avionics+power supply mass of 92 g.

Protocols used : BME280 barometer interfaced with through I2C. SD card reader interfaced with through SPI.

<div align="center">
  <img width="300" alt="Avionics" src="https://github.com/nlugon/Water-Rocket/assets/112929907/25e85869-1160-46ca-8abf-f8329b320b81">
</div>

## Code
Altitude measurements are filtered using a Kalman filter to give a smooth altitude estimation. The Kalman filter was validated by flying the avionics onboard a drone and tuning the filter over multiple flights. Parachute is deployed when the rocket loses altitude by 40cm, provided that the rocket already exceeded 3m beyond its ground recorded altitude to make sure the rocket actually launched. Pre-flight checks are validated by emitting different sound tones from the piezo buzzer.


<div align="center">
  <img src="https://github.com/nlugon/Water-Rocket/assets/112929907/336f5a02-960f-414b-95bf-9f094c101ec3" alt="First Image Description" width="350" />
  <img src="https://github.com/nlugon/Water-Rocket/assets/112929907/df0c18bc-8720-45cd-a7cb-0714009d1e99" alt="Second Image Description" width="552"  />
</div>

(Plot on the right : time in milliseconds on the x axis, and altitude in meters on the y axis. Curve in red corresponds to altitude measurements directly obtained from the BME280 sensor. Curve in blue corresponds to the filtered altitude measurements using a Kalman filter. The use of the Kalman filter provides a more accurate and robust detection for rocket apogee.)


## Parachute Recovery
<div align="center">
  <img width="300" alt="Avionics" src="https://github.com/nlugon/Water-Rocket/assets/112929907/104f81b6-14d8-4399-9799-e7ae7bf01b95">
</div>





