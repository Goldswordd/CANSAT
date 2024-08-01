#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#define BAND 419E6

const char* ssid = "401cafe";
const char* password = "deochothisao";
 
ESP8266WebServer server(80);

#define TX_PIN 2
#define RX_PIN 3

SoftwareSerial loraSerial(D2, D3); // Khai báo SoftwareSerial cho module LoRa

String device_id;
String temperature;
String pressure;
String altitude;
String humidity;
String rainfall;
String lux;
WiFiClient client;
int data_count = 0; // Biến đếm số lần dữ liệu đã nhận
Chart temperatureChart;


void setup() {
  Serial.begin(9600);
  while (!Serial);

  loraSerial.begin(9600); // Khởi động SoftwareSerial

  Serial.println("Chương trình đã khởi động.");
  Serial.println("Connecting to ");
  Serial.println(ssid);
 
  //connect to your local wi-fi network
  WiFi.begin(ssid, password);
 
  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
 
  server.begin();
  Serial.println("HTTP server started");
  
}

void loop() {
   
  String LoRaMessage;
  int pos1, pos2, pos3, pos4, pos5, pos6;

  if (loraSerial.available() > 0) {
    String LoRaData = loraSerial.readString();
    Serial.print("Received packet: ");
    Serial.println(LoRaData);

    pos1 = LoRaData.indexOf('a');
    pos2 = LoRaData.indexOf('b');
    pos3 = LoRaData.indexOf('c');
    pos4 = LoRaData.indexOf('d');
    pos5 = LoRaData.indexOf('e');
    pos6 = LoRaData.indexOf('f');
    device_id = LoRaData.substring(0, pos1);
    temperature = LoRaData.substring(pos1 + 1, pos2);
    pressure = LoRaData.substring(pos2 + 1, pos3);
    altitude = LoRaData.substring(pos3 + 1, pos4);
    humidity = LoRaData.substring(pos4 + 1, pos5);
    rainfall = LoRaData.substring(pos5 + 1, pos6);
    lux = LoRaData.substring(pos6 + 1, LoRaData.length());

    Serial.print("Device ID: ");
    Serial.println(device_id);

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");

    Serial.print("Pressure: ");
    Serial.print(pressure);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude: ");
    Serial.print(altitude);
    Serial.println(" m");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Rainfall: ");
    Serial.print(rainfall);
    Serial.println(" %");

    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");

    Serial.println();
    
       // Cập nhật dữ liệu cho biểu đồ
    server.handleClient();
    data_count++; // Tăng biến đếm sau mỗi lần nhận dữ liệu
  }
}

void handle_OnConnect()
{
  server.send(200, "text/html", SendHTML(temperature.toFloat(), humidity.toFloat(), pressure.toFloat(), altitude.toFloat(), rainfall.toFloat(), lux.toFloat()));
}
void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}
String SendHTML(float temperature, float humidity, float pressure, float altitude, float rainfall, float lux)
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Wireless Weather Station</title>\n";
  ptr += "<script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\n"; // Thêm thư viện Chart.js
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "canvas { -moz-user-select: none; -webkit-user-select: none; -ms-user-select: none; }\n"; // Tắt lựa chọn
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>Wireless Weather Station</h1>\n";
  
  // Thêm canvas cho từng biểu đồ
  ptr += "<canvas id=\"temperatureChart\" width=\"800\" height=\"400\"></canvas><br>\n";
  ptr += "<canvas id=\"humidityChart\" width=\"800\" height=\"400\"></canvas><br>\n";
  ptr += "<canvas id=\"pressureChart\" width=\"800\" height=\"400\"></canvas><br>\n";
  ptr += "<canvas id=\"altitudeChart\" width=\"800\" height=\"400\"></canvas><br>\n";
  ptr += "<canvas id=\"rainfallChart\" width=\"800\" height=\"400\"></canvas><br>\n";
  ptr += "<canvas id=\"luxChart\" width=\"800\" height=\"400\"></canvas><br>\n";
  
  // Script JavaScript để vẽ biểu đồ
  ptr += "<script>\n";
  ptr += "var ctx1 = document.getElementById('temperatureChart').getContext('2d');\n";
  ptr += "var temperatureChart = new Chart(ctx1, {\n";
  ptr += "  type: 'line',\n";
  ptr += "  data: {\n";
  ptr += "    labels: [],\n"; // Sử dụng các nhãn rỗng ban đầu
  ptr += "    datasets: [{\n";
  ptr += "      label: 'Temperature (C)',\n";
  ptr += "      data: [],\n"; // Sử dụng dữ liệu rỗng ban đầu
  ptr += "      borderColor: 'red',\n";
  ptr += "      borderWidth: 1,\n";
  ptr += "      fill: false\n";
  ptr += "    }]\n";
  ptr += "  },\n";
  ptr += "  options: {\n";
  ptr += "    responsive: false,\n";
  ptr += "    scales: {\n";
  ptr += "      xAxes: [{\n";
  ptr += "        display: true\n";
  ptr += "      }],\n";
  ptr += "      yAxes: [{\n";
  ptr += "        display: true\n";
  ptr += "      }]\n";
  ptr += "    }\n";
  ptr += "  }\n";
  ptr += "});\n";

  // Lặp lại cho các biểu đồ còn lại
  // ...

  ptr += "</script>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
