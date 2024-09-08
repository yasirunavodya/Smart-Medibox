#include <WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>

#define DHT_PIN 15
#define BUZZER 12
#define LDR1 34
#define LDR2 35
#define MOTOR 18


Servo motor;
WiFiClient espClient; 
PubSubClient mqttClient(espClient); 
WiFiUDP ntpUDP; 
NTPClient timeClient(ntpUDP); 
DHTesp dhtSensor; 

float minAngle=30.0;// Minimum angle of the shader
float controlFac=0.75;// Controlling factor 
float customMinAngle;
float customControlFac;
bool custom = false;
bool isScheduledON = false;
unsigned long scheduledOnTime;

char tempAr[6];// store temperature 
char humidAr[6];//store humidity 
char lightAr[6];//store light intensity

void setup() {
  Serial.begin(115200);  
  setupWifi();
  setupMqtt();
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

  timeClient.begin();
  timeClient.setTimeOffset(5.5*3600);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  
  pinMode(LDR1, INPUT);
  pinMode(LDR2, INPUT);

  motor.attach(MOTOR, 500, 2400);// servo motor to pin
}

void loop() {
  if (!mqttClient.connected()){
    connectToBroker();
  }
  mqttClient.loop();

  updateTemperatureAndHumidity();
  Serial.println(tempAr);
  Serial.println(humidAr);
  mqttClient.publish("TEMPERATURE_MED_YASIRU",tempAr);// Publish temperature 
  mqttClient.publish("HUMIDITY_MED_YASIRU",humidAr);

  checkSchedule();

  updateLightIntensity();// Read light intensity from LDR  
  mqttClient.publish("LIGHT_INT_MED_YASIRU",lightAr);// Publish light intensity 

  delay(1000);
}

void buzzerOn(bool on){
  if (on){
    tone(BUZZER, 256);
  }
  else{
    noTone(BUZZER);
  }
}

void setupMqtt(){
  mqttClient.setServer("test.mosquitto.org",1883);
  mqttClient.setCallback(receiveCallback);
}

void setupWifi(){
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println("Wokwi-GUEST");
  WiFi.begin("Wokwi-GUEST","");

  while (WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("Wifi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void receiveCallback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char payloadCharAr[length];

  for (int i=0;i<length;i++){
    Serial.print((char)payload[i]);
    payloadCharAr[i]=(char)payload[i];
  }

  Serial.println(":");

  if (strcmp(topic,"MAIN-ON-OFF")==0){  
      buzzerOn(payloadCharAr[0]=='1');
  }
  else if (strcmp(topic,"ADMIN-SCH-ON")==0){
      if (payloadCharAr[0]=='N'){
        isScheduledON = false;
      }else{
        isScheduledON = true;
        scheduledOnTime = atol(payloadCharAr);
      }
  }
  
  if (strcmp(topic,"MINIMUM-ANG-YASIRU")==0){
    customMinAngle = atof(payloadCharAr);
    minAngle = customMinAngle; // Update global variable
    Serial.println(minAngle);        
}
if (strcmp(topic,"CONTROL-FAC-YASIRU")==0){
    customControlFac = atof(payloadCharAr);
    controlFac = customControlFac; // Update global variable
    Serial.println(controlFac);              
}


  if (strcmp(topic,"DROP-DOWN-YASIRU")==0){
      
      if (payloadCharAr[0]=='A'){
        minAngle = 20;
        controlFac = 0.5;
        Serial.println(minAngle);
        Serial.println(controlFac);
      }else if(payloadCharAr[0]=='B'){
        minAngle = 50;
        controlFac = 0.7;
        Serial.println(minAngle);
        Serial.println(controlFac);
      }else if(payloadCharAr[0]=='C'){
        minAngle = 10;
        controlFac = 0.8;
        Serial.println(minAngle);
        Serial.println(controlFac);
      }
      else{
        custom = true;
      }   
  }
}

void connectToBroker(){
  while (!mqttClient.connected()){
    Serial.println("Attempting MQTT connetion...");
    if (mqttClient.connect("ESP-9813247900")){
      Serial.println("Connected");
      mqttClient.subscribe("MAIN-ON-OFF");
      mqttClient.subscribe("ADMIN-SCH-ON");
      mqttClient.subscribe("DROP-DOWN-YASIRU");
      mqttClient.subscribe("MINIMUM-ANG-YASIRU");
      mqttClient.subscribe("CONTROL-FAC-YASIRU");
    }
    else{
      Serial.println("failed");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}

void updateTemperatureAndHumidity(){
  TempAndHumidity data=dhtSensor.getTempAndHumidity();
  String(data.temperature,2).toCharArray(tempAr, 6);
  String(data.humidity,2).toCharArray(humidAr, 6);
}

void updateLightIntensity() {
  const float analogMinValue = 0.0;   // Minimum analogRead 
  const float analogMaxValue = 1023.0; // Maximum analogRead 
  const float intensityMin = 0.0;  // Minimum intensity 
  const float intensityMax = 1.0;  // Maximum intensity 

  float rightLDR = analogRead(LDR1);
  float leftLDR = analogRead(LDR2);
  float intensity = 0 ;

  if(rightLDR > leftLDR){
    if(rightLDR <= 1023){
      intensity = (rightLDR-analogMinValue)/(analogMaxValue-analogMinValue);
      Serial.println("rightLDR "+String(rightLDR)+"  "+String(intensity));
      mqttClient.publish("MAX-LIGHT-INTENSITY-YASIRU","RIGHT LDR");
      String(intensity, 2).toCharArray(lightAr, 6);
      AdjustServoMotor(intensity,0.5);// Adjust sliding window using light intensity

    }
    else{
      intensity = 1;
      String(intensity, 2).toCharArray(lightAr, 6);
      Serial.println("rightLDR "+String(rightLDR)+"  "+String(intensity));
      mqttClient.publish("MAX-LIGHT-INTENSITY-YASIRU","RIGHT LDR");
      AdjustServoMotor(intensity,0.5);// Adjust sliding window using light intensity

    
    }
} 
  else{
    if(leftLDR <= 1023){
      intensity = (leftLDR-analogMinValue)/(analogMaxValue-analogMinValue);
      Serial.println("leftLDR "+String(leftLDR)+"  "+String(intensity));
      mqttClient.publish("MAX-LIGHT-INTENSITY-YASIRU","LEFT LDR");
      String(intensity, 2).toCharArray(lightAr, 6);
      AdjustServoMotor(intensity,1.5);
    }
    else{
      intensity = 1;
      String(intensity, 2).toCharArray(lightAr, 6);
      Serial.println("leftLDR "+String(leftLDR)+"  "+String(intensity));
      mqttClient.publish("MAX-LIGHT-INTENSITY-YASIRU","LEFT LDR");
      AdjustServoMotor(intensity,1.5);// Adjust sliding window using light intensity


    }
  }
    
}

unsigned long getTime(){
  timeClient.update();
  return timeClient.getEpochTime();// Get current time from NTP server
}

void checkSchedule(){
  if (isScheduledON){
    unsigned long currentTime = getTime();
    Serial.println("currentTime "+String(currentTime));
    Serial.println("scheduledOnTime "+String(scheduledOnTime));
    if (currentTime > scheduledOnTime){
      buzzerOn(true);
      isScheduledON = false;
      mqttClient.publish("ADMIN-MAIN-ON-OFF-ESP","1");
      mqttClient.publish("SCH-ESP-ON","0");
      Serial.println("Scheduled ON");
    }
  }  
}
void AdjustServoMotor(double lightintensity,double D){
  double angle;

  angle = minAngle*D +(180.0-minAngle)*lightintensity*controlFac;// Calculate the angle using light intensity

  
  Serial.println(angle);
  motor.write(angle);// Set the angle of the servo motor  
}