//================this is the base code for Team 2 K65-SAE CANSAT PROJECT=================================
//author: N.V.SANG and N.T.PHONG
//unique sensors : BME280, VEML7700, MPU6050, RAIN SENSORS, GPS NEO 7M tranmission LORA E32
//NOTE: The gps delay is not over 1000ms to receive location signal from sattlites, the baudrate of SoftwareSerial can be 115200
//teams will send a str with 4 times loop to end all the data sensors XD
//===================================================================================================


//Initialization of Sensors and Serial Communication:
#include <SoftwareSerial.h>                     
#include "LoRa_E32.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_VEML7700.h>              
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <TinyGPS++.h>           

//Sensor and Communication Setup:
#define ENCRYPT 0x78
#define BAND 419E6
#define rain_sensor A0
#define SEALEVELPRESSURE_HPA (1013.25) 

SoftwareSerial ss(8, 9);              //TX , RX GPS 
SoftwareSerial mySerial(2, 3);        //TX, RX LORA


MPU6050 mpu6050(Wire);
Adafruit_BME280 bme;
Adafruit_VEML7700 veml;
TinyGPSPlus gps;
String LoRaMessage = "";
char device_id[12] = "MYPC";
unsigned int i=0;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  ss.begin(9600);
  Wire.begin();
  veml.begin();
  mpu6050.begin();

  mpu6050.calcGyroOffsets(true);      //Calibrate Gyroscope
  pinMode(rain_sensor, INPUT);
  while (!Serial);

  Serial.println(F("LoRa Sender"));

  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");    //Initialize BME280 Sensor
    while (1);
  }

}

//Fetching GPS Location
void getLocation(float lat, float lng){
  while (ss.available() > 0){
     if (gps.encode(ss.read())){
      displayInfo(lat,lng);
     }
   }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
  delay(0);
}
void displayInfo(float lat, float lng)
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(lat, 6);
    Serial.print(F(","));
    Serial.print(lng, 6);
  }
  else
  {
    lat = 0;
    lng = 0;
    Serial.print(F("INVALID"));
  }
  Serial.println();  
}

void loop() {
  //Reading Sensor Data in Loop
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float humidity = bme.readHumidity();
  long timer = 0;
  double lux = veml.readLux(VEML_LUX_NORMAL);
  int rainfall = map(analogRead(rain_sensor), 780, 0, 0, 100);
  float Latitude = gps.location.lat();
  float Longitude = gps.location.lng();
  float accX = mpu6050.getAccX();
  float accY = mpu6050.getAccY();
  float accZ = mpu6050.getAccZ();
  float angleX = mpu6050.getGyroAngleX();
  float angleY = mpu6050.getGyroAngleY();
  float angleZ = mpu6050.getGyroAngleZ();
  if (rainfall >= 100) {
    rainfall = 100;
  }
  if (rainfall <= 0) {
    rainfall = 0;
  }
  //Printing Sensor Data
  Serial.print(F("Device ID: "));
  Serial.println(device_id);

  getLocation(Latitude,Longitude);


  Serial.print(F("Temperature = "));
  Serial.print(temperature);
  Serial.println(F(" *C"));

  Serial.print(F("Pressure = "));
  Serial.print(pressure);
  Serial.println(F(" hPa"));

  Serial.print(F("Approx. Altitude = "));
  Serial.print(altitude);
  Serial.println(F(" m"));

  Serial.print(F("Humidity = "));
  Serial.print(humidity);
  Serial.println(F(" %"));

  Serial.print(F("Rainfall = "));
  Serial.print(rainfall);
  Serial.println(F(" %"));

  Serial.print(F("Light = "));
  Serial.print(lux);
  Serial.println(F(" lx"));

  mpu6050.update();


    Serial.print("accX : "); Serial.print(accX);
    Serial.print("\taccY : "); Serial.print(accY);
    Serial.print("\taccZ : "); Serial.println(accZ);

    Serial.print("gyroX : "); Serial.print(mpu6050.getGyroX());
    Serial.print("\tgyroY : "); Serial.print(mpu6050.getGyroY());
    Serial.print("\tgyroZ : "); Serial.println(mpu6050.getGyroZ());

    Serial.print("accAngleX : "); Serial.print(mpu6050.getAccAngleX());
    Serial.print("\taccAngleY : "); Serial.println(mpu6050.getAccAngleY());

    Serial.print("gyroAngleX : "); Serial.print(mpu6050.getGyroAngleX());
    Serial.print("\tgyroAngleY : "); Serial.print(mpu6050.getGyroAngleY());
    Serial.print("\tgyroAngleZ : "); Serial.println(mpu6050.getGyroAngleZ());

    Serial.print("angleX : "); Serial.print(angleX);
    Serial.print("\tangleY : "); Serial.print(angleY);
    Serial.print("\tangleZ : "); Serial.println(angleZ);
    Serial.println("=======================================================\n");


//Building LoRa Message and Sending Data
if (i==0) {
    LoRaMessage = String(i) + "a" + String(temperature) + "b" + String(pressure)
               + "c" + String(altitude) + "d" + String(humidity);
                  Serial.println(i);

  } else if(i == 1){
    LoRaMessage = String(i) + "a" + String(accX) + "b" + String(accY)
                            + "c" + String(accZ) + "d" + String(rainfall) ;
                  Serial.println(i);

  } else if(i == 2){
    LoRaMessage = String(i) + "a" + String(angleX) + "b" + String(angleY)
                            + "c" + String(angleZ) + "d" + String(lux) ;
                  Serial.println(i);

  } else if(i == 3){
    LoRaMessage = String(i) + "a" + String(Latitude) + "b" + String(Longitude);
                  Serial.println(i);

  }

  //send packet
  mySerial.print(LoRaMessage);
  delay(2000);
  i++;
  if(i==4) {i=0;}
}
