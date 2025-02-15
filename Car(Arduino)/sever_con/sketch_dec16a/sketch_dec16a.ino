#include <Wire.h>

const int MPU6050_ADDR = 0x68; // MPU6050 I2C 주소
const int GYRO_XOUT_H = 0x43; // 자이로 X축 상위 바이트 레지스터
const int GYRO_YOUT_H = 0x45; // 자이로 Y축 상위 바이트 레지스터
const int GYRO_ZOUT_H = 0x47; // 자이로 Z축 상위 바이트 레지스터

void setup() {
  Wire.begin(); // I2C 초기화
  Serial.begin(115200); // 시리얼 통신 시작
  
  // MPU6050 초기화
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B); // Power management 레지스터
  Wire.write(0x00); // 장치를 정상 작동 모드로 설정
  Wire.endTransmission();
}

void loop() {
  int16_t gyroX = readMPU6050(GYRO_XOUT_H); // X축 자이로 값 읽기
  int16_t gyroY = readMPU6050(GYRO_YOUT_H); // Y축 자이로 값 읽기
  int16_t gyroZ = readMPU6050(GYRO_ZOUT_H); // Z축 자이로 값 읽기

  // 시리얼 모니터에 출력
  Serial.print("Gyro X: ");
  Serial.print(gyroX);
  Serial.print(" | Gyro Y: ");
  Serial.print(gyroY);
  Serial.print(" | Gyro Z: ");
  Serial.println(gyroZ);

  delay(500); // 500ms 대기
}

// MPU6050 데이터 읽기 함수
int16_t readMPU6050(int reg) {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg); // 읽고자 하는 레지스터 주소
  Wire.endTransmission(false); // 반복 시작 조건 유지
  Wire.requestFrom(MPU6050_ADDR, 2); // 2 바이트 요청

  int16_t value = Wire.read() << 8 | Wire.read(); // 상위 바이트와 하위 바이트 결합
  return value;
}
