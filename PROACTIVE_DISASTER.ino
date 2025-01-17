#define BLYNK_TEMPLATE_ID "TMPL3udJizyRr"
#define BLYNK_TEMPLATE_NAME "Proactive Disaster Detection"
#define BLYNK_AUTH_TOKEN "01nk7JK6dqwzBc1rtfVn2lCJDjGUJ9go"
 
// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial
 
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
 WiFiClient client;

char auth[] = BLYNK_AUTH_TOKEN;

String apiKey = "O8EID70VI88EA38B";     //  Enter your Write API key from ThingSpeak
 
const char *ssid =  "pritam";     // replace with your wifi ssid and wpa2 key
const char *pass =  "12345678";

const char* server = "api.thingspeak.com";

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
String long_lat;

String Link;

#include <TinyGPS++.h>
TinyGPSPlus gps;

static const uint32_t GPSBaud = 9600;

float latitude = 13.1718;
float longitude = 77.5362;

#define echoPin1 19
#define trigPin1 18

int vib = 5;
long duration;
int distance;
 
int buzz=4;
int val; 
int dist1;
BlynkTimer timer; 
void GPS()
{
if (gps.charsProcessed() < 10)
  {
    //Serial.println("No GPS detected: check wiring.");
     // Blynk.virtualWrite(V4, "GPS ERROR");         // Value Display widget  on V4 if GPS not detected
  }
}
void displaygpsInfo()
{ 

  if(gps.location.isValid()) 
  {
    
    latitude = (gps.location.lat());      //Storing the Lat. and Lon. 
    longitude = (gps.location.lng()); 
    
    Serial.print("LAT:  ");
    Serial.println(latitude, 6);                // float to x decimal places
    Serial.print("LONG: ");
    Serial.println(longitude, 6); 
    
      }
}
 

void locate()
{
  while (Serial.available() > 0) 
    {
      // sketch displays information every time a new sentence is correctly encoded.
      if (gps.encode(Serial.read()))
        displaygpsInfo();
  }
}

int obstacle(int trigPinx, int echoPinx) {
  digitalWrite(trigPinx, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPinx, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinx, LOW);

  duration = pulseIn(echoPinx, HIGH);

  distance = duration * 0.034 / 2;
  return distance;
}
void ultraso()
{ 
   dist1 = obstacle(trigPin1,echoPin1);
   Serial.println(dist1);
  Serial.print("\t");
       lcd.setCursor(0, 0);
  lcd.print("Fld_D:" + String(distance));
        Blynk.virtualWrite(V2, latitude);  // Virtual Pin 2 for Latitude
    Blynk.virtualWrite(V3, longitude); // Virtual Pin 3 for Longitude

  if (distance < 10) {
    Blynk.virtualWrite(V0, "High");  // Send "High" if flood detected
       digitalWrite(buzz,HIGH);
  lcd.setCursor(6,1);
lcd.print(" FLD:HIGH  :");
      
    long_lat = String(float(latitude))+","+String(float(longitude));
    Blynk.virtualWrite(V0, "High");  // Send "High" if flood detected
   Serial.println(long_lat);
  } else if (distance < 50) {
    Blynk.virtualWrite(V0, "Normal");  // Send "Normal" for a normal water level
  }  
  else{
    digitalWrite(buzz,LOW);
     lcd.setCursor(6,1);
lcd.print(" FLD:LOW   :");
 Blynk.virtualWrite(V0, "Low");  // Send "Low" for low water level
     
  }   
  delay(1000);
 
  }
void vibration()
{
val = digitalRead(vib);
Serial.println(val);
lcd.setCursor(0,1);
lcd.print("Vib:");
lcd.print(val);

  if (val == HIGH) {
    Blynk.virtualWrite(V1, "Detected");
  } else {
    Blynk.virtualWrite(V1, "Not Detected");
  }
} 
void setup() {
  // put your setup code here, to run once:   
Serial.begin(9600);  
pinMode(trigPin1, OUTPUT);
pinMode(echoPin1, INPUT);
pinMode(vib, INPUT);
  pinMode(buzz,OUTPUT); 
   lcd.begin();
  lcd.backlight();
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  timer.setInterval(2000L, locate); 
  timer.setInterval(1000L, ultraso); 
    timer.setInterval(2000L, vibration);
  timer.setInterval(9000L, sendDataTS);
}

void loop() {
  // put your main code here, to run repeatedly:
 Blynk.run();
timer.run();
}
void sendDataTS(void)
{
     if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(val);
                             postStr +="&field2=";
                             postStr += String(dist1);
                             postStr +="&field3=";
                             postStr += String(latitude); 
                             postStr +="&field4=";
                             postStr += String(longitude);                   
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
                           
                               delay(1000);
                        }
          client.stop();
  } 
