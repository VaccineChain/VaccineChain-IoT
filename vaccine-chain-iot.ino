#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>
#include <WebServer.h>

#define DHTPIN 22      // Chân GPIO 16
#define DHTTYPE DHT11  // Loại cảm biến DHT11


DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Pi Kafe 1";
const char* password = "79797979";
const char* host = "192.168.9.209";
const int port = 3000;
WiFiClient client;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("");
  Serial.println("Connected to the WiFi network");
  Serial.print("ESP32's IP adress is: ");
  Serial.println(WiFi.localIP());

  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  int temp = dht.readTemperature();
  int humid = dht.readHumidity();
  
  if (!client.connect(host, port)) {
    Serial.println("Lỗi kết nối đến webserver!!!");
    return;
  }

  //GET METHODS ==================================

  // String receiver = "/dht11";
  // receiver += "?temperature=";
  // receiver += temp;
  // receiver += "&humidity=";
  // receiver += humid;

  // Serial.print("Receiver: ");
  // Serial.println(receiver);

  // //Tạo biến chứa yêu cầu kết nối đến với máy chủ
  // String request = "GET " + receiver + " HTTP/1.1\r\n";
  // request += String("Host: ") + host + "\r\n";
  // request += "Connection: close\r\n\r\n";

  // client.print(request);
  // Serial.println();
  // Serial.println("====>Dong ket noi!!!!");
  // delay(3000);


  //POST METHODS ==================================
  String receiver = "/dht11";
  receiver += "?temperature=";
  receiver += temp;
  receiver += "&humidity=";
  receiver += humid;

  Serial.print("Receiver: ");
  Serial.println(receiver);

  // Tạo biến chứa yêu cầu kết nối đến với máy chủ
  String request = "POST " + receiver + " HTTP/1.1\r\n";
  request += "Host: " + String(host) + "\r\n";
  request += "Content-Type: application/x-www-form-urlencoded\r\n";
  request += "Content-Length: " + String(receiver.length()) + "\r\n";
  request += "Connection: close\r\n\r\n";
  request += receiver;

  client.print(request);
  Serial.println();
  Serial.println("====>Đã kết nối!!!!");
  delay(3000);
}