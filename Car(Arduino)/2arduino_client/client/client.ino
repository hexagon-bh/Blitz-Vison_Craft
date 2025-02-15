#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// WiFi 설정
const char* ssid = "vr-rc-car";
const char* password = "12345678";

// 서버 주소 (서버 ESP8266의 IP 주소 입력)
const char* serverIP = "http://192.168.246.141";  // 서버 IP 주소 (서버 ESP8266의 IP)

void setup() {
  // 시리얼 통신 시작
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
  // 서버 URL 설정
  WiFiClient client;
  HTTPClient http;
  String url = "http://192.168.246.141/getData";

  http.begin(client, url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("Received JSON: " + payload);

    // JSON 데이터 파싱 (간단한 문자열 처리)
    int XIndex = payload.indexOf("\"xshaft\":") + 9;
    int YIndex = payload.indexOf("\"yshaft\":") + 9;
    float X = payload.substring(XIndex, payload.indexOf(",", XIndex)).toFloat();
    float Y = payload.substring(YIndex, payload.indexOf("}", YIndex)).toFloat();

    Serial.print("Xshaft: ");
    Serial.println(X);
    Serial.print("Yshaft: ");
    Serial.println(Y);
  } else {
    Serial.printf("HTTP GET failed: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  delay(1000);
}
