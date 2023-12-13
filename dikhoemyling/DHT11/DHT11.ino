#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ESP8266WebServer.h>
#include <ML8511.h>

#define DHTPIN D1   // Định nghĩa chân kết nối DHT22
#define DHTTYPE DHT11 // Loại cảm biến DHT

const char* ssid = "KhueTorres"; // Tên WiFi
const char* password = "04021995"; // Mật khẩu WiFi
const char* serverAddress = "192.168.100.4"; // Địa chỉ server Django
IPAddress IP;

ESP8266WebServer server(80);
int UVOUT = D3; //Output from the sensor
int REF_3V3 = D4; //3.3V power on the Arduino board



DHT dht(DHTPIN, DHTTYPE);

int ledPin = D0; // Chân kết nối đèn LED

void setup() {
  pinMode(ledPin, OUTPUT); // Thiết lập chân kết nối đèn LED
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);
  Serial.begin(9600);
  dht.begin();
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.println("Connecting to WiFi..");
  }

  server.on("/", handleRoot);
  server.on("/0", turnOff);
  server.on("/1", turnOn);

  server.begin();

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // In địa chỉ IP trong Serial Monitor
  IP = WiFi.localIP();

  Serial.println("Connected to WiFi");
}

void loop() {
  delay(2000);


  int uvLevel = 0;
  int refLevel = averageAnalogRead(REF_3V3);

  //Use the 3.3V power pin as a reference to get a very accurate output value from sensor
  //float outputVoltage = 3.3 / refLevel * uvLevel;

  //float uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0); //Convert the voltage to a UV intensity level


  Serial.print("ML8511 output: ");
  Serial.print(uvLevel);

  Serial.println();

  delay(100);



  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  String device_name = "Rèm 3";

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed vto read from DHT sensor");
    return;
  }

  // Tạo JSON payload
  String data = "{\"temperature\":" + String(temperature) + ", \"humidity\":" + String(humidity) + ", \"uvLevel\":" + String(uvLevel) + ", \"device_name\":\"" + device_name + "\", \"ipaddr\":\"" + IP.toString() + "\"}";
  Serial.println(data);
  //delay(2000);

  WiFiClient client;
  if (client.connect(serverAddress, 8000)) {
    Serial.println("Connected to server");
    // HTTPClient http;
    // http.addHeader("Content-Type", "application/raw");
    // http.begin(client, serverAddress, 8000, "/addData/");
    // int httpResponseCode = http.POST(data);
    // Serial.println(httpResponseCode);
    client.print("POST ");
    client.print("/addData/");
    client.println(" HTTP/1.1");
    client.println("Host: 172.20.10.5"); // Thay thế bằng tên miền hoặc địa chỉ IP của máy chủ
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);
  }else {
    Serial.print("Fail to connect");
  }
    server.handleClient();
    
}


int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 
  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;
  return(runningValue);  
}
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void handleRoot() {
  server.send(200, "text/html", "Welcome to LED control");
}

void turnOn() {
  digitalWrite(ledPin, HIGH); // Bật đèn
  server.send(200, "text/html", "LED turned on");
}

void turnOff() {
  digitalWrite(ledPin, LOW); // Tắt đèn
  server.send(200, "text/html", "LED turned off");
}