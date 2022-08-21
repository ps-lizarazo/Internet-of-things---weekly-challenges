#include <ESP8266WiFi.h>
#include "DHT.h"
#define DHTTYPE DHT11   // DHT 11

const char* ssid = "NOVUS_CXNK00289342_2.4G";
const char* password = "clave-wifi";

#define LED D1 // LED
// DHT Sensor
uint8_t DHTPin = 4;

const double k = 5.0/1023; // Rate to convert analog read to Voltage
const double R2 = 10000; // 10k Resistance
const double B = 1.3*pow(10.0,7); // Constants based on resistance vs Lux from datasheet
const double m = -1.4; // Constants based on resistance vs Lux from datasheet

// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

double light_intensity (int RawADC0) {  
    double V2 = k*RawADC0;
    double R1 = (5.0/V2 - 1)*R2;
    double lux = B*pow(R1,m);
    return lux;
}

float Temperature;
float Humidity;
double Intensity;

WiFiServer server(80);

void setup() {
 Serial.begin(115200);
 delay(10);

 pinMode(LED, OUTPUT);
 digitalWrite(LED, LOW); //LED apagado

 // Connect to WiFi network
 Serial.println();
 Serial.println();
 Serial.print("Connecting to ");
 Serial.println(ssid);

 WiFi.begin(ssid, password);

 while (WiFi.status() != WL_CONNECTED) {
   delay(500);
   Serial.print(".");
 }
 Serial.println("");
 Serial.println("WiFi connected");

 // Start the server
 server.begin();
 Serial.println("Server started");

 // Print the IP address
 Serial.print("Use this URL to connect: ");
 Serial.print("http://");
 Serial.print(WiFi.localIP());
 Serial.println("/");

}

void loop() {
 Temperature = dht.readTemperature(); // Gets the values of the temperature
 Humidity = dht.readHumidity(); // Gets the values of the humidity
 Intensity = light_intensity(analogRead(A0)); // Gets the value of light intensity

 // Check if a client has connected
 WiFiClient client = server.available();
 if (!client) {
   return;
 }

 // Wait until the client sends some data
 Serial.println("new client");
 while(!client.available()){
   delay(1);
 }

 // Read the first line of the request
 String request = client.readStringUntil('\r');
 Serial.println(request);
 client.flush();

 // Match the request

 int value = HIGH;
 if (request.indexOf("/LED=ON") != -1)  {
   digitalWrite(LED, HIGH);
   value = HIGH;
 }
 if (request.indexOf("/LED=OFF") != -1)  {
   digitalWrite(LED, LOW);
   value = LOW;
 }

 // Return the response
 client.println("HTTP/1.1 200 OK");
 client.println("Content-Type: text/html");
 client.println(""); //  do not forget this one
 client.println("<!DOCTYPE HTML>");
 client.println("<html>");

 client.print("LED is now: ");

 if(value == LOW) {
   client.print("Off");
 } else {
   client.print("On");
 }
 client.println("<br><br>");
 client.println("<a href=\"/LED=ON\"\"><button>Turn On </button></a>");
 client.println("<a href=\"/LED=OFF\"\"><button>Turn Off </button></a><br />");
 client.println("</html>");

 client.println("<br><br>");
 client.print("Temperature: ");
 client.print(Temperature);
 client.print("C");
 client.println("<br><br>");
 client.print("Humidity: ");
 client.print(Humidity);
 client.print("%");
 client.println("<br><br>");
 client.print("Light intensity: ");
 client.print(Intensity);
 client.print("lux (lx)");

 delay(1);
 Serial.println("Client disconnected");
 Serial.println("");

}