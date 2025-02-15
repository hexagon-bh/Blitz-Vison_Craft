#include "CytronMotorDriver.h"

// Cytron 모터 드라이버 설정
CytronMD motor(PWM_DIR, 2, 14);  // PWM = 핀 3, DIR = 핀 4.

void setup() {
  Serial.begin(115200);
  // 초기화 코드는 여기 추가
}

void loop() {
  // 속도를 0에서 100까지 증가
  for (int i = 0; i <= 100; i++) {
    Serial.println(i);
    motor.setSpeed(i);  // 속도 설정
    delay(50);          // 50ms 대기
  }

  // 속도를 100에서 0까지 감소
  for (int i = 100; i >= 0; i--) {
    Serial.println(i);
    motor.setSpeed(i);  // 속도 설정
    delay(50);          // 50ms 대기
  }

  // 모터 멈춤
  motor.setSpeed(0);
  delay(1000); // 1초 동안 대기
}
