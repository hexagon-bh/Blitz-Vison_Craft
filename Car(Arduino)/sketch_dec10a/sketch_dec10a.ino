// 핀 설정
const int potPin = A0;         // 가변저항 연결 핀 (ESP8266 아날로그 핀)
const int buttonLeftPin = D6;  // 좌측 버튼 핀 (GPIO 14)
const int buttonRightPin = D5; // 우측 버튼 핀 (GPIO 12)

double max_angle_reg = 1008;
double depo = 0;
double max_angle_servo = 50;

// 설정값
const int readCount = 10; // 1초 동안 읽을 횟수

void setup() {
  Serial.begin(115200); // 시리얼 통신 시작
  pinMode(buttonLeftPin, INPUT_PULLUP); // 좌측 버튼을 풀업 입력으로 설정
  pinMode(buttonRightPin, INPUT_PULLUP); // 우측 버튼을 풀업 입력으로 설정
}

void loop() {
  int potSum = 0;

//  // 가변저항 값 6번 읽기
//  for (int i = 0; i < readCount; i++) {
//    potSum += analogRead(potPin);
//    delay(5); // 0.05초 동안 10번 읽기 (10ms 간격)
//  }

//  // 평균값 계산
//  double potAvg = potSum / (readCount - 1);

  double pot_float = analogRead(potPin);
  
  double servo_ang = 90+(((max_angle_reg-(pot_float-16))/max_angle_reg*max_angle_servo)-(max_angle_servo/2)+depo);
  
  // 버튼 상태 읽기
  bool isLeftPressed = !digitalRead(buttonLeftPin);  // LOW일 때 버튼 눌림
  bool isRightPressed = !digitalRead(buttonRightPin);

  // 평균값 전송
  Serial.print("가변저항 평균값: ");
  Serial.println(int(servo_ang));

  // 버튼 상태 확인
  if (isLeftPressed && isRightPressed) {
    Serial.println("버튼 2개 눌림");
  } else if (isLeftPressed) {
    Serial.println("좌측 버튼 눌림");
  } else if (isRightPressed) {
    Serial.println("우측 버튼 눌림");
  } else {
    Serial.println("버튼이 눌리지 않음");
  }

  // 1초에 한 번만 전송되도록 추가 지연
  delay(10); // 약간의 여유를 더 주기
}
