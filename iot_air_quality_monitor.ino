
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "Sifat120" //hotspot/wifi user name
#define WLAN_PASS       "sifat120" //hotspot/wifi password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // 8883 for MQTTS
#define AIO_USERNAME    "Sifat120"
#define AIO_KEY         "eaa2d56701c14bfbb55b18874b7003fd"

/************ Global State (you don't need to change this!) ******************/


WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish HUM = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/HUM");
Adafruit_MQTT_Publish TEMP = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/TEMP");
Adafruit_MQTT_Publish haz = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/haz");
Adafruit_MQTT_Publish HYDRO = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/HYDRO");

/*************************** Sketch Code ************************************/


void MQTT_connect();

#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */
#include <dht.h>

dht DHT;

#define DHT11_PIN 2

int sensorValue;

int air = A0;
int quality;
int d;


void setup(void) {


  Serial.println("Hello!");
  ads.begin();
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

 

}

uint32_t x = 0;

void loop(void) {

  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here
  Adafruit_MQTT_Subscribe *subscription;



  /*******************our code*******************************/


  int16_t adc0, adc1, adc2, adc3;

  adc0 = ads.readADC_SingleEnded(0);
  //  adc1 = ads.readADC_SingleEnded(1);
  //  adc2 = ads.readADC_SingleEnded(2);
  //  adc3 = ads.readADC_SingleEnded(3);
  Serial.print("AIN0: "); Serial.println(adc0);
  //  Serial.print("AIN1: "); Serial.println(adc1);
  //  Serial.print("AIN2: "); Serial.println(adc2);
  //  Serial.print("AIN3: "); Serial.println(adc3);
  Serial.println(" ");
   d = map(adc0, 12300, 11000, 0, 7);
  Serial.print("per: "); Serial.println(d);
  delay(300);

  hum();
  hazz();

  /***************************/






  // Now we can publish haz!
  Serial.print(F("\nSending haz val "));
  
  Serial.print("...");
  if (! haz.publish(quality)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }


 // Now we can publish HUM!
  Serial.print(F("\nSending HUM val "));
  
  Serial.print("...");
  if (! HUM.publish(DHT.humidity)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }


 // Now we can publish TEMP!
  Serial.print(F("\nSending TEMP val "));

  Serial.print("...");
  if (! TEMP.publish(DHT.temperature)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }



  







 // Now we can publish HYDRO!
  Serial.print(F("\nSending HYDRO val "));
;
  Serial.print("...");
  if (! HYDRO.publish(d)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }



}


void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}

void hum()
{
  int chk = DHT.read11(DHT11_PIN);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);
  delay(300);
}
void hazz()
{
  sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1023.0);
  // print out the value you read:
  //Serial.println(voltage);
  Serial.println("AIR");
  Serial.print(sensorValue);
  quality=map(sensorValue,0,1023,0,100);
  delay(300);


}
