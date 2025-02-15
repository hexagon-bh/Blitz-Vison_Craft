#include <ESP8266WiFi.h>

const int potPin = A0;         // 가변저항 연결 핀 (ESP8266 아날로그 핀)
const int buttonLeftPin = D6;  // 좌측 버튼 핀 (GPIO 14)
const int buttonRightPin = D5; // 우측 버튼 핀 (GPIO 12)

double max_angle_reg = 1008;
double depo = 0;
double max_angle_servo = 20;

int forward = 1;
int LR = 120;
// WiFi 설정
const char* ssid = "vr-rc-car";
const char* password = "12345678";

// 서버 IP와 포트
const char* host = "192.168.246.220";  // ESP3의 IP 주소
const int port = 12345;  // 포트 번호

WiFiClient client;

int controlSignal = 0;  // 전진/후진 신호를 저장할 정수형 변수
int leftRightSignal = 0;  // 좌우 신호를 저장할 정수형 변수 (임의의 값)

void setup() {
  Serial.begin(115200);
  pinMode(buttonLeftPin, INPUT_PULLUP); // 좌측 버튼을 풀업 입력으로 설정
  pinMode(buttonRightPin, INPUT_PULLUP); // 우측 버튼을 풀업 입력으로 설정

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
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
  
  if (client.connect(host, port)) {
    String command = "Control:" + String(forward) + " LeftRight:" + String(LR);
    client.println(command);  // 전진/후진 신호와 좌우 신호 전송
    delay(100);  // 1초 간격으로 전송
  } else {
    Serial.println("Connection failed");
  }
}
