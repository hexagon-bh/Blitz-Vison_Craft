#include <ESP8266WiFi.h>
#include <Servo.h> // servo library  
#include "CytronMotorDriver.h"

CytronMD motor(PWM_DIR, 14, 12);  // PWM = 핀 5, DIR = 핀 6.
Servo s1; 
Servo s2; 
Servo s3;

// WiFi 설정
const char* ssid = "vr-rc-car";
const char* password = "12345678";

// 서버 설정
WiFiServer server(12345);  // 포트 번호

int controlSignal = 0;  // 전진/후진 신호 저장 변수
int leftRightSignal = 90;  // 좌우 신호 저장 변수
int ms = 0;

int gyroX = 90, gyroY = 90;  // 자이로 X, Y 값 저장 변수

void setup() {
  s1.attach(13, 500, 2500);  // servo attach D3 pin of arduino y
  s2.attach(4, 500, 2500);  // servo attach D3 pin of arduino x
  s3.attach(5, 500, 2500);  // servo attach D3 pin of arduino 좌우
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  
  if (client) {
    String message = client.readStringUntil('\n');
    
    // 수신된 메시지를 분석하여 각 값 저장
    if (message.startsWith("Control:")) {
      int controlValueStart = message.indexOf(":") + 1;
      int controlValueEnd = message.indexOf(" LeftRight:");
      controlSignal = message.substring(controlValueStart, controlValueEnd).toInt();
      
      int leftRightValueStart = message.indexOf("LeftRight:") + 10;
      leftRightSignal = message.substring(leftRightValueStart).toInt();
    }

    if (message.startsWith("GyroX:")) {
      int gyroXStart = message.indexOf("GyroX:") + 6;
      int gyroXEnd = message.indexOf(" GyroY:");
      gyroX = message.substring(gyroXStart, gyroXEnd).toInt();
      
      int gyroYStart = message.indexOf("GyroY:") + 6;
      gyroY = message.substring(gyroYStart).toInt();
    }
    
    // 수신된 데이터 출력
    Serial.print("Control Signal (Forward/Backward): ");
    Serial.println(controlSignal);
    Serial.print("Left/Right Signal: ");
    Serial.println(leftRightSignal);
    
    Serial.print("Gyro X: ");
    Serial.println(gyroX);
    Serial.print("Gyro Y: ");
    Serial.println(gyroY);

    if(controlSignal == 1 && ms <= 40){
      ms += 12;
    }
    else if(controlSignal == -1 && ms >= -100){
      ms -= 10;
    }
    else if(controlSignal == 2){
      ms = 0;
    }
    else if(controlSignal == 0){
      if(ms < 0){
        ms += 20;
      }
      else if(ms > 0){
        ms -= 15;
      }
    }
    motor.setSpeed(ms);
    s3.write(leftRightSignal);
    s1.write(gyroX);
    s2.write(90);
    
    client.stop();
  }
}
