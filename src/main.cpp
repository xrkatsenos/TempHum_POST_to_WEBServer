// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <ESP8266HTTPClient.h>

// Replace with your network credentials
const char *ssid = "ssid";
const char *password = "passwd";

#define DHTPIN 12 // Digital pin connected to the DHT sensor - GPIO12 = D6

// Uncomment the type of sensor in use:
#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;

String processor(const String &var)
{
  //Serial.println(var);
  if (var == "TEMPERATURE")
  {
    return String(t);
  }
  else if (var == "HUMIDITY")
  {
    return String(h);
  }
  return String();
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  dht.begin();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println(".");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop()
{
  HTTPClient http; //Declare object of class HTTPClient

  String temp, hum, URL, postData;
  URL = "http://192.168.2.100/storeData";

  // Read temperature as Celsius (the default)
  float newT = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float newT = dht.readTemperature(true);
  // if temperature read failed, don't change t value
  if (isnan(newT))
  {
    Serial.println("Failed to read from DHT sensor!");
  }
  else
  {
    t = newT;
    Serial.println(t);
  }
  // Read Humidity
  float newH = dht.readHumidity();
  // if humidity read failed, don't change h value
  if (isnan(newH))
  {
    Serial.println("Failed to read from DHT sensor!");
  }
  else
  {
    h = newH;
    Serial.println(h);
  }

  temp = String(t); //String to interger conversion
  hum = String(h);
  //Post Data
  //postData = "temperature=" + t + "&humidity=" + h;
  postData = "temperature=" + temp + "&humidity=" + hum;

  http.begin(URL);           //Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded"); //Specify content-type header

  int httpCode = http.POST(postData); //Send the request
  String payload = http.getString();  //Get the response payload

  Serial.println(httpCode); //Print HTTP return code
  Serial.println(payload);  //Print request response payload

  http.end(); //Close connection

  delay(5000); //Post Data at every 5 seconds
}