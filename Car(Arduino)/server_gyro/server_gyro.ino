#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include<Wire.h>

const int MPU_ADDR = 0x68;    // I2C통신을 위한 MPU6050의 주소
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;   // 가속도(Acceleration)와 자이로(Gyro)
double angleAcX, angleAcY, angleAcZ;
double angleGyX, angleGyY, angleGyZ;
double angleFiX, angleFiY, angleFiZ;

const double RADIAN_TO_DEGREE = 180 / 3.14159;  
const double DEG_PER_SEC = 32767 / 250;    // 1초에 회전하는 각도
const double ALPHA = 1 / (1 + 0.04);
// GyX, GyY, GyZ 값의 범위 : -32768 ~ +32767 (16비트 정수범위)


unsigned long now = 0;   // 현재 시간 저장용 변수
unsigned long past = 0;  // 이전 시간 저장용 변수
double dt = 0;           // 한 사이클 동안 걸린 시간 변수 

double averAcX, averAcY, averAcZ;
double averGyX, averGyY, averGyZ;

// WiFi 설정
const char* ssid = "vr-rc-car";
const char* password = "12345678";

// 웹 서버 객체 생성
ESP8266WebServer server(80);

// 온도와 습도 데이터 (예제 값)
int x = 90;
int y = 45;

unsigned long previousMillis = 0;
const long interval = 10; // 데이터 갱신 주기 (ms)

void setup() {
  // 시리얼 통신 시작
  Serial.begin(115200);
  initSensor();
  caliSensor();   //  초기 센서 캘리브레이션 함수 호출
  past = millis(); // past에 현재 시간 저장  
  delay(10);

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
    String jsonResponse = "{\"xshaft\":" + String(x) + ",\"yshaft\":" + String(y) + "}";
    
    // JSON 데이터 전송
    server.send(200, "application/json", jsonResponse);
  });

  // 서버 시작
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  getData(); 
  getDT();

  angleAcX = atan(AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2)));
  angleAcX *= RADIAN_TO_DEGREE;
  angleAcY = atan(-AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2)));
  angleAcY *= RADIAN_TO_DEGREE;
    // 가속도 센서로는 Z축 회전각 계산 불가함.
  
    // 가속도 현재 값에서 초기평균값을 빼서 센서값에 대한 보정
  angleGyX += ((GyX - averGyX) / DEG_PER_SEC) * dt;  //각속도로 변환
  angleGyY += ((GyY - averGyY) / DEG_PER_SEC) * dt;
  angleGyZ += ((GyZ - averGyZ) / DEG_PER_SEC) * dt;

    // 상보필터 처리를 위한 임시각도 저장
  double angleTmpX = angleFiX + angleGyX * dt;
  double angleTmpY = angleFiY + angleGyY * dt;
  double angleTmpZ = angleFiZ + angleGyZ * dt;

    // (상보필터 값 처리) 임시 각도에 0.96가속도 센서로 얻어진 각도 0.04의 비중을 두어 현재 각도를 구함.
  angleFiX = ALPHA * angleTmpX + (1.0 - ALPHA) * angleAcX;
  angleFiY = ALPHA * angleTmpY + (1.0 - ALPHA) * angleAcY;
  angleFiZ = angleGyZ;    // Z축은 자이로 센서만을 이용하열 구함.

  x = angleFiX + 90;
  y = angleGyZ + 90;

    // 갱신된 데이터 출력 (디버깅용)
  Serial.print("xshaft: ");
  Serial.print(x);
  Serial.print(", yshaft: ");
  Serial.println(y);
  // 클라이언트 요청 처리
  server.handleClient();
}

void initSensor() {
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);   // I2C 통신용 어드레스(주소)
  Wire.write(0x6B);    // MPU6050과 통신을 시작하기 위해서는 0x6B번지에    
  Wire.write(0);
  Wire.endTransmission(true);
}

void getData() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);   // AcX 레지스터 위치(주소)를 지칭합니다
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);  // AcX 주소 이후의 14byte의 데이터를 요청
  AcX = Wire.read() << 8 | Wire.read(); //두 개의 나뉘어진 바이트를 하나로 이어 붙여서 각 변수에 저장
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();
}

void getDT() {
  now = millis();   
  dt = (now - past) / 1000.0;  
  past = now;
}

void caliSensor() {
  double sumAcX = 0 , sumAcY = 0, sumAcZ = 0;
  double sumGyX = 0 , sumGyY = 0, sumGyZ = 0;
  getData();
  for (int i=0;i<10;i++) {
    getData();
    sumAcX+=AcX;  sumAcY+=AcY;  sumAcZ+=AcZ;
    sumGyX+=GyX;  sumGyY+=GyY;  sumGyZ+=GyZ;
  }
  averAcX=sumAcX/10;  averAcY=sumAcY/10;  averAcZ=sumAcY/10;
  averGyX=sumGyX/10;  averGyY=sumGyY/10;  averGyZ=sumGyZ/10;
}
