long temp,humi;
char temperature[10],humidity[10];

#include <Wire.h> 
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#include <FirebaseESP8266.h>  
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define FIREBASE_HOST "automated-plant-watering-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "jCrL2kMjlX20wRb8RnlX5ETglhnTusRZQULzvuCO"

#define WIFI_SSID "nodemcu1"
#define WIFI_PASSWORD "nodemcu1"

const int sensor_pin=A0;
const int Relaypin=16;

FirebaseData firebaseData;
FirebaseJson json; 

void setup() {
  pinMode(sensor_pin,INPUT);
  pinMode(Relaypin,OUTPUT);
  lcd.init();
  lcd.backlight();
  dht.begin();
  
  Serial.begin(9600);

  wifiConnect();

  Serial.println("Connecting Firebase.....");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Serial.println("Firebase OK.");

}
void wifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}
void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  lcd.setCursor(0,0);
  lcd.print("   Temperature  ");
  lcd.print(t);
  lcd.setCursor(0,1);
  lcd.print("    Humididty   ");
  lcd.print(h);
  delay(2000);
  lcd.clear();
  float moisture_percentage;

  moisture_percentage = ( 100.00 - ( (analogRead(sensor_pin)/1023.00) * 100.00 ) );

  lcd.setCursor(0,0);
  lcd.print("Soil Moisture:");
  lcd.print(moisture_percentage);
  lcd.print("%");
  
   Firebase.setFloat(firebaseData,"Project/Moisture",moisture_percentage);
String stat;
   if (Firebase.getString(firebaseData,"Project/Motor")){
     stat=firebaseData.stringData(); 
   }
 if(moisture_percentage<=50 || stat[2]== '0')
   {
    lcd.setCursor(0,1);
    digitalWrite(Relaypin,LOW);
    lcd.print("motor on");
   }
  else
  {
  lcd.setCursor(0,1);
  digitalWrite(Relaypin,HIGH);
  lcd.print("motor off");
  }
  
}
