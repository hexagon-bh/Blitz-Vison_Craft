#include <ESP8266WiFi.h>
#include <Wire.h>
#include <MPU6050.h>

// WiFi 설정
const char* ssid = "vr-rc-car";
const char* password = "12345678";

// 서버 IP와 포트
const char* host = "192.168.0.3";  // ESP3의 IP 주소
const int port = 12345;  // 포트 번호

WiFiClient client;
MPU6050 mpu;

int gyroX = 0, gyroY = 0;  // 자이로 X, Y 값 저장 변수

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  if (client.connect(host, port)) {
    Serial.println("Connected to ESP3");
  } else {
    Serial.println("Connection to ESP3 failed");
  }
}

void loop() {
  // 자이로 X, Y 값 읽기
  mpu.getRotation(&gyroX, &gyroY, NULL);  // 자이로 X, Y 값만 읽기 (Z는 NULL로 넘김)
  
  // 메시지 전송 (자이로 X, Y 값만 전송)
  if (client.connected()) {
    String message = "GyroX:" + String(gyroX) + " GyroY:" + String(gyroY);
    client.println(message);
    delay(1000);  // 1초마다 전송
  } else {
    Serial.println("Disconnected from ESP3");
  }
}
