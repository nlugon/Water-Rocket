/*
  Rocket SD card datalogger with BME280 barometer
  MPU6050 accelerometer originally used for data acquisition
  MCU : Arduino Nano

  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 10 
  WARNING SD CARD CONNECTED TO 3V

  I2C : SDA-SDI-A4, SCL-SCK-A5
  Piezo Buzzer : Pin 8
  Servo Motor : Pin 6
*/

#include <SPI.h>
#include <SD.h>

// #include <Adafruit_MPU6050.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include <Servo.h>

#include <SimpleKalmanFilter.h>

#define SEALEVELPRESSURE_HPA (1013.25)

void startTone();

/*
 SimpleKalmanFilter(e_mea, e_est, q);
 e_mea: Measurement Uncertainty 
 e_est: Estimation Uncertainty 
 q: Process Noise
 */
SimpleKalmanFilter pressureKalmanFilter(0.01, 1, 0.01);



// file name to use for writing
const char filename[] = "april18.txt";

//int notes[] = { 440, 523, 587, 659 };

int notes[] = {659, 523, 784, 392};
const int chipSelect = 10;

// Adafruit_MPU6050 mpu;
Adafruit_BME280 bme;

// Sensor status
unsigned sd_status;
// unsigned mpu_status;
unsigned bme_status;

// Home altitude 
float ground_altitude;
// Current altitude 
float current_filtered_altitude;
float previous_filtered_altitude;
// Max altitude
float max_altitude;


// Apogee detection
bool apogee = 0;

// Log timing
unsigned long startMillis;  // some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 200;  // the value is a number of milliseconds
const unsigned long period_buzzer = 1000;

int counter; // for buzzer after parachute deployment 

Servo servomotor;

void setup() {

  counter = 0;
  servomotor.attach(6);
  servomotor.write(5); 

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  while (!Serial) {
    delay(10); // wait for serial port to connect. Needed for native USB port only
  }

  // INITIALIZE SD CARD *******************************************
  Serial.print("Initializing SD card... ");
  // see if the card is present and can be initialized:
  sd_status = SD.begin(chipSelect);
  if (!sd_status) {
    Serial.println("Card failed, or not present");
  }

  else {
    Serial.println("Card initialized.");
    // try to open the file for writing
    File dataFile = SD.open(filename, FILE_WRITE);
    if (!dataFile) {
      Serial.print("error opening ");
      Serial.println(filename);
    }
    else {
    // add some new lines to start
    dataFile.println();
    dataFile.println("Time(ms) Altitude(m) FilteredAlt (m)");

    }
    dataFile.close();

  }




  // INITIALIZE MPU 6050 ***********************************************
  // Serial.print("Initializing MPU 6050... ");
  // mpu_status = mpu.begin();
  // if (!mpu_status) {
  //   Serial.println("Failed to find MPU6050 chip");
  // }
  // else {
  //   mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  //   mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  //   mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  //   Serial.println("Success");
  //   delay(100);
  // }


  // INITIALIZE BME280 ***********************************************
  Serial.print("Initializing BME 280... ");
  bme_status = bme.begin();
  if (!bme_status) {
    Serial.println("Failed to find BME280 chip");
  }
  else {
    Serial.println("Success");
  }

  //Pre-launch checks :
  startTone();
  delay(5000);
  servomotor.write(175);
  delay(2000);
  servomotor.write(5);
  delay(1000);

  // Get unfiltered ground altitude
  ground_altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  current_filtered_altitude = pressureKalmanFilter.updateEstimate(bme.readAltitude(SEALEVELPRESSURE_HPA));
  previous_filtered_altitude = current_filtered_altitude;
  max_altitude = current_filtered_altitude;
  Serial.print("Ground Altitude : ");
  Serial.println(ground_altitude);
  Serial.print("Ground filtered altitude : ");
  Serial.println(current_filtered_altitude);
  

  startMillis = millis();  //initial start time
}

void loop() {

  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  //test whether the period has elapsed. 

  if (currentMillis - startMillis >= period) { 

      


      // SD CARD READER : make a string for assembling the data to log:
      String dataString = "";
      dataString += String(currentMillis);
      dataString += " ";


      if (bme_status){
        dataString += String(bme.readAltitude(SEALEVELPRESSURE_HPA)); // Approx Altitude in m
        dataString += " ";
        // dataString += String(bme.readPressure() / 100.0F); // Pressure in hPa
        // dataString += " ";
        current_filtered_altitude = pressureKalmanFilter.updateEstimate(bme.readAltitude(SEALEVELPRESSURE_HPA));
        dataString += String(current_filtered_altitude); // Kalman Altitude in m
        dataString += " ";

        Serial.print("Ground Altitude : ");
        Serial.println(ground_altitude);
        Serial.print("Current Altitude : ");
        Serial.println(current_filtered_altitude);
        Serial.print("Max Altitude : ");
        Serial.println(max_altitude);



        if (max_altitude < current_filtered_altitude){
              max_altitude = current_filtered_altitude;     
            }

        if (current_filtered_altitude - ground_altitude > 3 && max_altitude - current_filtered_altitude > 0.4) {
// current_filtered_altitude - ground_altitude > 3
              // DEPLOYY THE PARACHUTE
              apogee = 1;
              servomotor.write(175);
              delay(50);            
        }

        if (apogee == 1){
            tone(8, notes[0]);
            delay(100);
            noTone(8);
            delay(2000);
        
        }



        //previous_filtered_altitude = current_filtered_altitude;


      }

      // Add info for when apogee detected
      dataString += String(apogee);
      dataString += " ";      


      // MPU6050 : Get new sensor events with the readings 
      // if (mpu_status){
      //   sensors_event_t a, g, temp;
      //   mpu.getEvent(&a, &g, &temp);

      //   dataString += String(a.acceleration.x); // X-axis Acceleration in m/s2
      //   dataString += " ";
      //   dataString += String(a.acceleration.y);
      //   dataString += " ";
      //   dataString += String(a.acceleration.z);
      //   dataString += " ";
      //   // dataString += String(g.gyro.x);
      //   // dataString += " ";
      //   // dataString += String(g.gyro.y);
      //   // dataString += " ";
      //   // dataString += String(g.gyro.z);
      //   // dataString += " ";
      // }


      if (sd_status){
        // if the file is available, write to it:
        File dataFile = SD.open(filename, FILE_WRITE);
        if (dataFile) {
          dataFile.println(dataString);
          // print to the serial port too:
          Serial.println(dataString);
        }
        // if the file isn't open, pop up an error:
        else {
          Serial.print("error opening ");
          Serial.println(filename);
        }
        dataFile.close();
      }


      startMillis = currentMillis;
  }

}

// void startTone() {
//   // PIEZO : play music
//   tone(8, notes[0]);
//   delay(200);
//   tone(8, notes[1]);
//   delay(200);
//   tone(8, notes[2]);
//   delay(400);
//   noTone(8);
// }

void startTone() {
  // PIEZO : play music upon startup 
  tone(8, notes[0]);
  delay(200);
  noTone(8);
  delay(50);

  tone(8, notes[0]);
  delay(200);
  noTone(8);
  delay(100);

  tone(8, notes[0]);
  delay(200);
  noTone(8);
  delay(200);

  tone(8, notes[1]);
  delay(200);
  noTone(8);
  delay(50);

  tone(8, notes[0]);
  delay(200);
  noTone(8);
  delay(100);

  tone(8, notes[2]);
  delay(300);
  noTone(8);
  delay(400);

  tone(8, notes[3]);
  delay(200);
  noTone(8);
  delay(100);
}
