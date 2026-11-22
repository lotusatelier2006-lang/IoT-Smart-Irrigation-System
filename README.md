# 🌱 Hệ Thống Tưới Nước Tự Động IoT (IoT Smart Irrigation System)

Đồ án môn học **Hệ thống nhúng** - Lớp **CNTT 18-01**

## 👥 Thành viên thực hiện - Nhóm 16
* **Đặng Hoàng Vũ**
* **Nguyễn Đức Trường**
* **Đào Huy Thư**

---

## 📌 1. Giới thiệu đề tài
Hệ thống giám sát độ ẩm đất, nhiệt độ và độ ẩm không khí theo thời gian thực, từ đó tự động bật/tắt máy bơm nước dựa trên ngưỡng cài đặt hoặc cho phép người dùng điều khiển từ xa qua ứng dụng điện thoại (Blynk IoT). Giải pháp giúp tiết kiệm nước và tự động hóa việc chăm sóc cây trồng khi vắng nhà.

## 🛠️ 2. Linh kiện phần cứng sử dụng
* **Vi điều khiển trung tâm:** ESP32 DevKit V1 (tích hợp Wi-Fi)
* **Cảm biến độ ẩm đất:** Soil Moisture Sensor (Capacitive v1.2 chống ăn mòn)
* **Cảm biến môi trường:** DHT11 (đo nhiệt độ và độ ẩm không khí)
* **Cơ cấu chấp hành:** Module Relay 1 kênh 5V + Máy bơm nước mini DC 5V
* **Nguồn cấp:** Adapter 5V - 2A

## 🔌 3. Sơ đồ kết nối chân (Pinout)
| Linh kiện | Chân linh kiện | Chân ESP32 |
| :--- | :--- | :--- |
| Cảm biến độ ẩm đất | A0 (Analog) | GPIO 34 |
| Cảm biến DHT11 | DATA | GPIO 4 |
| Module Relay (Máy bơm) | IN | GPIO 26 |
| Nguồn chung | VCC / GND | 5V (VIN) / GND |

## ⚙️ 4. Tính năng chính
* **Chế độ Tự động (Auto):** Hệ thống tự động bật máy bơm khi độ ẩm đất giảm xuống dưới ngưỡng (`< 40%`) và tự ngắt khi đất đạt đủ độ ẩm (`>= 55%`).
* **Chế độ Thủ công (Manual):** Bật/tắt máy bơm trực tiếp thông qua nút nhấn trên ứng dụng điện thoại.
* **Giám sát & Lưu trữ:** Theo dõi thông số môi trường theo thời gian thực và xuất dữ liệu thực nghiệm ra file CSV.

## 📂 5. Cấu trúc thư mục dự án
* `Code/`: Mã nguồn nạp cho vi điều khiển ESP32 (`smart_watering.ino`).
* `Libraries/`: Các thư viện sử dụng trong dự án.
* `Data/`: Dữ liệu đo đạc thực nghiệm từ cảm biến (`sensor_logs.csv`).
* `Poster/`: File thiết kế Poster báo cáo khổ A4.
