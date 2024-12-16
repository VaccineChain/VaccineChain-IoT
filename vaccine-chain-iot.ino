#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#define DHTPIN 22      // GPIO pin 22
#define DHTTYPE DHT11  // DHT11 sensor type

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Greenwich-Guest";
const char* password = "greenwichvn@123";
const char* serverName = "https://b513-118-69-133-161.ngrok-free.app/api/Dht11";  // Replace with your server's IP and port
String deviceId = "DEV0" + String(DHTPIN);
String vaccineId = "VAC002";

WiFiClient client;

// NTP setup
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", 0, 3600);  // Offset in seconds (3600 = UTC+1)

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(3000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("");
  Serial.println("Connected to the WiFi network");
  Serial.print("ESP32's IP address is: ");
  Serial.println(WiFi.localIP());

  dht.begin();

  // Start the NTPClient to get time
  timeClient.begin();
}

void loop() {
  // Update the time
  timeClient.update();

  // Get the current time
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Current Time: ");
  Serial.println(formattedTime);

  float temp = dht.readTemperature();
  if (isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.println("");
  Serial.print("Vaccine ID: ");
  Serial.println(vaccineId);

  Serial.print("Device ID: ");
  Serial.println(deviceId);

  Serial.print("Temperature: ");
  Serial.println(temp);

  // Send POST request
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Start connection to the server
    http.begin(serverName);

    // Set method and headers
    http.addHeader("Content-Type", "application/json");

    // Create JSON data to send
    String jsonData = "{"
                      "\"deviceId\": \"" + deviceId + "\", "
                      "\"vaccineId\": \"" + vaccineId + "\", "
                      "\"value\": " + String(temp) + "}";

    // Send POST request
    int httpResponseCode = http.POST(jsonData);

    // Process server response
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response: " + response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }

  delay(3000);
}
