/*
 * Đề tài: Hệ thống tưới nước tự động IoT
 * Nhóm 16 - Lớp CNTT 18-01
 * Thành viên: Đặng Hoàng Vũ, Nguyễn Đức Trường, Đào Huy Thư
 */

#define BLYNK_TEMPLATE_ID "TMPxxxxxx"
#define BLYNK_TEMPLATE_NAME "TuoiCayTuDong"
#define BLYNK_AUTH_TOKEN "Your_Blynk_Token"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

char ssid[] = "Ten_WiFi";
char pass[] = "Mat_Khau_WiFi";

#define SOIL_PIN 34
#define DHT_PIN 4
#define RELAY_PIN 26
#define DHTTYPE DHT11

DHT dht(DHT_PIN, DHTTYPE);
BlynkTimer timer;

int soilPercent = 0;
int threshold = 40;       // Ngưỡng độ ẩm đất bật bơm (%)
bool autoMode = true;     // Chế độ tự động mặc định
bool pumpState = false;

BLYNK_WRITE(V1) {
  if (!autoMode) {
    pumpState = param.asInt();
    digitalWrite(RELAY_PIN, pumpState ? LOW : HIGH);
  }
}

BLYNK_WRITE(V2) {
  autoMode = param.asInt();
}

BLYNK_WRITE(V3) {
  threshold = param.asInt();
}

void sendSensorData() {
  int rawSoil = analogRead(SOIL_PIN);
  soilPercent = constrain(map(rawSoil, 4095, 1500, 0, 100), 0, 100);

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (autoMode) {
    if (soilPercent < threshold) {
      pumpState = true;
      digitalWrite(RELAY_PIN, LOW);
    } else if (soilPercent >= threshold + 15) {
      pumpState = false;
      digitalWrite(RELAY_PIN, HIGH);
    }
  }

  Blynk.virtualWrite(V0, soilPercent);
  Blynk.virtualWrite(V1, pumpState);
  if (!isnan(temp) && !isnan(hum)) {
    Blynk.virtualWrite(V4, temp);
    Blynk.virtualWrite(V5, hum);
  }

  Serial.printf("Do am dat: %d%% | Nhiet do: %.1fC | Do am KK: %.1f%% | Bom: %s\n",
                soilPercent, temp, hum, pumpState ? "ON" : "OFF");
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
