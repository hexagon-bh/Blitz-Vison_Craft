#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const int potPin = A0;         // 가변저항 연결 핀 (ESP8266 아날로그 핀)
const int buttonLeftPin = D6;  // 좌측 버튼 핀 (GPIO 14)
const int buttonRightPin = D5; // 우측 버튼 핀 (GPIO 12)

double max_angle_reg = 1008;
double depo = 0;
double max_angle_servo = 50;

// WiFi 설정
const char* ssid = "vr-rc-car";
const char* password = "12345678";

// 웹 서버 객체 생성
ESP8266WebServer server(80);

// 온도와 습도 데이터 (예제 값)
int forward = 1;
int LR = 120;

void setup() {
  // 시리얼 통신 시작
  Serial.begin(115200);
  pinMode(buttonLeftPin, INPUT_PULLUP); // 좌측 버튼을 풀업 입력으로 설정
  pinMode(buttonRightPin, INPUT_PULLUP); // 우측 버튼을 풀업 입력으로 설정
  delay(1000);

  // Wi-Fi 연결
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("Server IP Address: ");
  Serial.println(WiFi.localIP()); // ESP8266의 IP 주소 출력

  // `/getData` 요청 처리
  server.on("/getData", []() {
    // JSON 형식으로 데이터를 간단하게 구성
    String jsonResponse = "{\"forward\":" + String(forward) + ",\"leftright\":" + String(LR) + "}";
    
    // JSON 데이터 전송
    server.send(200, "application/json", jsonResponse);
  });

  // 서버 시작
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  bool isLeftPressed = !digitalRead(buttonLeftPin);  // LOW일 때 버튼 눌림
  bool isRightPressed = !digitalRead(buttonRightPin);

  double pot_float = analogRead(potPin);

  double servo_ang = 90+(((max_angle_reg-(pot_float-16))/max_angle_reg*max_angle_servo)-(max_angle_servo/2)+depo);
  LR = int(servo_ang);
  
  if (isLeftPressed && isRightPressed) {
    forward = 2;
  } else if (isLeftPressed) {
    forward = 1;
  } else if (isRightPressed) {
    forward = -1;
  } else {
    forward = 0;
  }
  // 클라이언트 요청 처리
  server.handleClient();
  
}
