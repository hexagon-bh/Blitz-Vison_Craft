#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h> // servo library  
#include "CytronMotorDriver.h"

// Cytron 모터 드라이버 설정
CytronMD motor(PWM_DIR, 14, 12);  // PWM = 핀 5, DIR = 핀 6.
Servo s1; 
Servo s2; 
Servo s3;

// WiFi 설정
const char* ssid = "vr-rc-car";
const char* password = "12345678";

// 서버 주소
const char* servers[] = {
  "http://192.168.246.141/getData",  // 서버 1(0)
  "http://192.168.246.2/getData"   // 서버 2(1)
};
const int serverCount = 2;  // 서버 개수
int currentServer = 0;      // 현재 요청 중인 서버의 인덱스
int X = 0;
int Y = 0;
int forward = 0;
int LR = 90;
int ms = 0;
//int xpos = 90;
//int ypos = 90;

void setup() {
  s1.attach(13, 500, 2500);  // servo attach D3 pin of arduino y
  s2.attach(4, 500, 2500);  // servo attach D3 pin of arduino x
  s3.attach(5, 500, 2500);  // servo attach D3 pin of arduino 좌우
  unsigned long previousMillis = 0;  // 이전 시간 저장 변수
  const long interval = 1000;  // 간격(밀리초 단위)
  Serial.begin(115200);
  delay(1000);

  // Wi-Fi 연결
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  
}

void loop() {
  // Wi-Fi 연결 상태 확인
  if (WiFi.status() == WL_CONNECTED) {
    // 현재 서버에 요청 보내기
    String url = servers[0];
    WiFiClient client;
    HTTPClient http;

    http.begin(client, url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      // 서버 응답 처리
      String payload = http.getString();
      Serial.printf("Response from server1: %s\n", payload.c_str());

      // JSON 데이터 처리 (예제)
      int XIndex = payload.indexOf("\"xshaft\":") + 9;
      int YIndex = payload.indexOf("\"yshaft\":") + 9;
      X = payload.substring(XIndex, payload.indexOf(",", XIndex)).toInt();
      Y = payload.substring(YIndex, payload.indexOf("}", YIndex)).toInt();
      X = 180 - X;

      Serial.print("Xshaft: ");
      Serial.println(X);
      Serial.print("Yshaft: ");
      Serial.println(Y);
      s1.write(X);
      s2.write(Y);
    } 
    else {
      Serial.printf("Error connecting to server1: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    
    url = servers[1];

    http.begin(client, url);
    httpCode = http.GET();

    if (httpCode > 0) {
      // 서버 응답 처리
      String payload = http.getString();
      Serial.printf("Response from server2: %s\n", payload.c_str());

      // JSON 데이터 처리 (예제)
      int forwardIndex = payload.indexOf("\"forward\":") + 10;
      int LRIndex = payload.indexOf("\"leftright\":") + 12;
      forward = payload.substring(forwardIndex, payload.indexOf(",", forwardIndex)).toInt();
      LR = payload.substring(LRIndex, payload.indexOf("}", LRIndex)).toInt();

      Serial.print("Forward: ");
      Serial.println(forward);
      Serial.print("LeftRight: ");
      Serial.println(LR);

      if(forward == 1 && ms <= 40){
          ms += 12;
      }
      else if(forward == -1 && ms >= -70){
        ms -= 10;
      }
      else if(forward == 2){
        ms = 0;
      }
      else if(forward == 0){
        if(ms < 0){
          ms += 20;
        }
        else if(ms > 0){
          ms -= 15;
        }
      }
      motor.setSpeed(ms);
      s3.write(LR);
      Serial.printf("moter speed : %d\n", ms);
    } 
    else {
      Serial.printf("Error connecting to server2: ", http.errorToString(httpCode).c_str());
    }

    http.end();
  } 
  else {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
  }
}
