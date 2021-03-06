// Libraries
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"

// DHT 11 sensor
#define DHTPIN D2
#define DHTTYPE DHT22 

// Lamp pin
const int lamp_pin = 13;

// WiFi parameters
#define WLAN_SSID       "Ubisoft-Guests"
#define WLAN_PASS       "cz23Vjjf9k2@,j@,wWjRzzW5"

// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "svezina"
#define AIO_KEY         "e97f3bf2042141d19203a3036ca34852"

// DHT sensor
DHT dht(DHTPIN, DHTTYPE, 15);

// Functions
void connect();

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;
const char MQTT_CLIENTID[] PROGMEM  = AIO_KEY __DATE__ __TIME__;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWORD);

/****************************** Feeds ***************************************/

const char LAMP_FEED[] PROGMEM = AIO_USERNAME "/feeds/lamp";
Adafruit_MQTT_Subscribe lamp = Adafruit_MQTT_Subscribe(&mqtt, LAMP_FEED);

// Setup feeds for temperature & humidity
const char TEMPERATURE_FEED[] PROGMEM = AIO_USERNAME "/feeds/temperature";
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, TEMPERATURE_FEED);

const char HUMIDITY_FEED[] PROGMEM = AIO_USERNAME "/feeds/humidity";
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, HUMIDITY_FEED);

/*************************** Sketch Code ************************************/

void setup() {

  // Init sensor
  dht.begin();

  // Set lamp pin to output
  //pinMode(lamp_pin, OUTPUT);

  Serial.begin(115200);
  Serial.println(F("Adafruit IO Example"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  delay(10);
  Serial.print(F("Connecting to "));
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();

  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());

  // listen for events on the lamp feed
  //mqtt.subscribe(&lamp);

  // connect to adafruit io
  connect();

}

void loop() {

  //Adafruit_MQTT_Subscribe *subscription;

  // ping adafruit io a few times to make sure we remain connected
  if(! mqtt.ping(3)) {
    // reconnect to adafruit io
    if(! mqtt.connected())
      connect();
  }

  // Grab the current state of the sensor
  float humidity_data = (float)dht.readHumidity();
  float temperature_data = (float)dht.readTemperature();

  // Publish data
  if (! temperature.publish(temperature_data))
    Serial.println(F("Failed to publish temperature"));
  else
    Serial.println(F("Temperature published!"));

  if (! humidity.publish(humidity_data))
    Serial.println(F("Failed to publish humidity"));
  else
    Serial.println(F("Humidity published!"));

  // Repeat every 10 seconds
  delay(1000);

  // this is our 'wait for incoming subscription packets' busy subloop
  /*while (subscription = mqtt.readSubscription(1000)) {
    // Grab the current state of the sensor
    int humidity_data = (int)dht.readHumidity();
    int temperature_data = (int)dht.readTemperature();

    // Publish data
    if (! temperature.publish(temperature_data))
      Serial.println(F("Failed to publish temperature"));
    else
      Serial.println(F("Temperature published!"));

    if (! humidity.publish(humidity_data))
      Serial.println(F("Failed to publish humidity"));
    else
      Serial.println(F("Humidity published!"));

    // we only care about the lamp events
    if (subscription == &lamp) {

      // convert mqtt ascii payload to int
      char *value = (char *)lamp.lastread;
      Serial.print(F("Received: "));
      Serial.println(value);

      // Apply message to lamp
      String message = String(value);
      message.trim();
      if (message == "ON") {digitalWrite(lamp_pin, HIGH);}
      if (message == "OFF") {digitalWrite(lamp_pin, LOW);}

    }

  }*/

}

// connect to adafruit io via MQTT
void connect() {

  Serial.print(F("Connecting to Adafruit IO... "));

  int8_t ret;

  while ((ret = mqtt.connect()) != 0) {

    switch (ret) {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
    }

    if(ret >= 0)
      mqtt.disconnect();

    Serial.println(F("Retrying connection..."));
    delay(5000);

  }

  Serial.println(F("Adafruit IO Connected!"));

}
