/*
 * Đề tài: Hệ thống tưới nước tự động thông minh (2 khu vực: Đất & Cát)
 * Nhóm 16 - Lớp CNTT 18-01
 * Thành viên: Đặng Hoàng Vũ, Nguyễn Đức Trường, Đào Huy Thư
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ================================================================
// 1. CẤU HÌNH CHÂN KẾT NỐI (ARDUINO UNO)
// ================================================================
#define SOIL_DAT_PIN A0   // Cảm biến độ ẩm xô ĐẤT -> chân A0
#define SOIL_CAT_PIN A1   // Cảm biến độ ẩm xô CÁT -> chân A1
#define DHT_PIN      2    // Cảm biến nhiệt độ DHT11 -> chân D2
#define BTN_MODE     3    // Nút chuyển chế độ Tự động / Thủ công -> chân D3
#define BTN_PUMP     4    // Nút Bật/Tắt bơm thủ công -> chân D4
#define RELAY_PIN    7    // Chân tín hiệu IN của Relay máy bơm -> chân D7

// ================================================================
// 2. CẤU HÌNH MÁY BƠM & NGƯỠNG ĐỘ ẨM
// ================================================================
const bool RELAY_KICH_MUC_THAP = true; 

const int NGUONG_TUOI  = 40;   // Dưới 40% (1 trong 2 xô) -> Tự động BẬT bơm
const int NGUONG_NGUNG = 60;   // Từ 60% trở lên (cả 2 xô) -> Tự động TẮT bơm

// Thông số hiệu chỉnh cho cảm biến độ ẩm điện dung (bản mạch màu đen)
const int GIA_TRI_KHO  = 580;  // Giá trị khi đất/cát khô
const int GIA_TRI_UOT  = 260;  // Giá trị khi đất/cát ướt đẫm

// ================================================================
// 3. KHỞI TẠO ĐỐI TƯỢNG & BIẾN HỆ THỐNG
// ================================================================
#define DHT_TYPE DHT11
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHT_PIN, DHT_TYPE);

bool cheDoTuDong = true;       // Mặc định chạy Tự động (AU)
bool trangThaiBom = false;     // Trạng thái hiện tại của máy bơm
unsigned long thoiGianDocCuoi = 0;

// ================================================================
// 4. HÀM KHỞI TẠO (SETUP)
// ================================================================
void setup() {
  Serial.begin(9600);

  tatBom();
  pinMode(RELAY_PIN, OUTPUT);
  tatBom();

  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_PUMP, INPUT_PULLUP);

  Wire.begin();
  #if defined(WIRE_HAS_TIMEOUT)
    Wire.setWireTimeout(3000, true);
  #endif

  dht.begin();
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("HE THONG TUOI CAY");
  lcd.setCursor(0, 1);
  lcd.print("2 XO: DAT & CAT");
  delay(1500);
  lcd.clear();
}

// ================================================================
// 5. VÒNG LẶP CHÍNH (LOOP)
// ================================================================
void loop() {
  xuLyNutNhan();

  if (millis() - thoiGianDocCuoi >= 1000) {
    thoiGianDocCuoi = millis();

    int rawDat = docCamBienTrungBinh(SOIL_DAT_PIN);
    int rawCat = docCamBienTrungBinh(SOIL_CAT_PIN);

    int doAmDat = constrain(map(rawDat, GIA_TRI_KHO, GIA_TRI_UOT, 0, 100), 0, 100);
    int doAmCat = constrain(map(rawCat, GIA_TRI_KHO, GIA_TRI_UOT, 0, 100), 0, 100);

    float nhietDo = dht.readTemperature();

    if (cheDoTuDong) {
      if (doAmDat < NGUONG_TUOI || doAmCat < NGUONG_TUOI) {
        if (!trangThaiBom) {
          batBom();
        }
      } 
      else if (doAmDat >= NGUONG_NGUNG && doAmCat >= NGUONG_NGUNG) {
        if (trangThaiBom) {
          tatBom();
        }
      }
    }

    hienThiLCD(doAmDat, doAmCat, nhietDo);

    Serial.print("Dat: "); Serial.print(doAmDat); Serial.print("% ("); Serial.print(rawDat); Serial.print(") | ");
    Serial.print("Cat: "); Serial.print(doAmCat); Serial.print("% ("); Serial.print(rawCat); Serial.print(") | ");
    Serial.print("Che do: "); Serial.print(cheDoTuDong ? "AU" : "MN");
    Serial.print(" | Bom: "); Serial.println(trangThaiBom ? "BAT" : "TAT");
  }
}

// ================================================================
// 6. CÁC HÀM ĐIỀU KHIỂN MÁY BƠM & CẢM BIẾN
// ================================================================
void batBom() {
  trangThaiBom = true;
  digitalWrite(RELAY_PIN, RELAY_KICH_MUC_THAP ? LOW : HIGH);
}

void tatBom() {
  trangThaiBom = false;
  digitalWrite(RELAY_PIN, RELAY_KICH_MUC_THAP ? HIGH : LOW);
}

int docCamBienTrungBinh(int chanCamBien) {
  long tong = 0;
  for (int i = 0; i < 10; i++) {
    tong += analogRead(chanCamBien);
    delay(2);
  }
  return tong / 10;
}

void xuLyNutNhan() {
  if (digitalRead(BTN_MODE) == LOW) {
    delay(50);
    if (digitalRead(BTN_MODE) == LOW) {
      cheDoTuDong = !cheDoTuDong;
      tatBom();
      while (digitalRead(BTN_MODE) == LOW);
    }
  }

  if (!cheDoTuDong && digitalRead(BTN_PUMP) == LOW) {
    delay(50);
    if (digitalRead(BTN_PUMP) == LOW) {
      if (trangThaiBom) {
        tatBom();
      } else {
        batBom();
      }
      while (digitalRead(BTN_PUMP) == LOW);
    }
  }
}

void hienThiLCD(int soilDat, int soilCat, float temp) {
  lcd.setCursor(0, 0);
  lcd.print("Dat:");
  lcd.print(soilDat);
  lcd.print("%   ");

  lcd.setCursor(8, 0);
  lcd.print("Cat:");
  lcd.print(soilCat);
  lcd.print("%   ");

  lcd.setCursor(0, 1);
  if (isnan(temp)) {
    lcd.print("T:--C ");
  } else {
    lcd.print("T:");
    lcd.print((int)temp);
    lcd.print("C ");
  }

  lcd.setCursor(6, 1);
  lcd.print("Bom:");
  lcd.print(trangThaiBom ? "BAT" : "TAT");

  lcd.setCursor(14, 1);
  lcd.print(cheDoTuDong ? "AU" : "MN");
}
