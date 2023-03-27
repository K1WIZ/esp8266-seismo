/* First attempt at a seismometer sensor using MPU6050 sensor and ESP8266
 *  
 *  Written by: John Rogers
 *  Rev: 1
 *  03-26-23
 *  
 *  Website: https://wizworks.net
 *  
 *  Permission is granted for free use both commercial and personal so long as this notice remains.
 *  
 */


#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


Adafruit_MPU6050 mpu;

//Ip address of your machine
const char* mqtt_server = "ip.of.mqtt.host";
const char* ssid =  "Your SSID";                                    // replace with your wifi ssid and wpa2 key
const char* pass =  "Your Password";

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); /* will pause Zero, Leonardo, etc until serial console opens */
  }

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  /* join I2C bus (I2Cdev library doesn't do this automatically) */
  Wire.begin();

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);
  Serial.println("");
  delay(100);
}

void setup_wifi() {
    Serial.println("Connecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("seismoClient", "mqtt.user", "mqtt.passwd")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values for testing over serial connection*/
  Serial.print("AccelX:");
  Serial.print(a.acceleration.x);
  Serial.print(",");
  Serial.print("AccelY:");
  Serial.print(a.acceleration.y);
  Serial.print(",");
  Serial.print("AccelZ:");
  Serial.print(a.acceleration.z);
  Serial.println("");

  
  client.publish("seismo/Acc", (String(a.acceleration.x) + "," + String(a.acceleration.y) + "," + String(a.acceleration.z)).c_str(), true);

  delay(10);
}
