/*
 * Nama   : Surya Sumirat
 * NIM    : 24/547110/PPA/06872
 * Magang GMRT - Day 3
 * Deskripsi: Kontrol 5 servo berdasarkan MPU6050 (roll, pitch, yaw) dan PIR sensor.
 * Versi: Wokwi (ESP32)
 */
/*
- Posisi "0 derajat" dalam studi kasus = posisi netral sistem (tegak lurus).
- Karena servo hanya menerima nilai 0–180 derajat:
- -90° sistem → 0° servo
- 0° sistem → 90° servo  ← posisi awal (INITIAL_POS)
- +90° sistem → 180° servo
 */
#include <Wire.h>
#include <MPU6050.h>
#include <ESP32Servo.h>

MPU6050 mpu;

// Pin PIR
const int PIR_PIN = 13;

// Servo objects
Servo servo1, servo2, servo3, servo4, servo5;

const int SERVO1_PIN = 14;
const int SERVO2_PIN = 15;
const int SERVO3_PIN = 16;
const int SERVO4_PIN = 17;
const int SERVO5_PIN = 18;

// Posisi awal dan posisi alarm
const int INITIAL_POS = 90;
const int ALARM_POS = 45;

// Variabel yaw timeout
unsigned long yawTimer = 0;
bool returnYaw = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  pinMode(PIR_PIN, INPUT);

  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);
  servo4.attach(SERVO4_PIN);
  servo5.attach(SERVO5_PIN);
  resetServos();

  Serial.println("Inisialisasi MPU6050...");
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("Gagal koneksi ke MPU6050!");
    while (1);
  }
  Serial.println("MPU6050 berhasil terhubung!");
}

void resetServos() {
  servo1.write(INITIAL_POS);
  servo2.write(INITIAL_POS);
  servo3.write(INITIAL_POS);
  servo4.write(INITIAL_POS);
  servo5.write(INITIAL_POS);
}

void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float roll = gx / 131.0;
  float pitch = gy / 131.0;
  float yaw = gz / 131.0;

  roll = constrain(roll, -90, 90);
  pitch = constrain(pitch, -90, 90);
  yaw = constrain(yaw, -90, 90);

  if (digitalRead(PIR_PIN) == HIGH) {
    Serial.println("Gerakan terdeteksi! Aktifkan alarm servo...");
    servo1.write(ALARM_POS);
    servo2.write(ALARM_POS);
    servo3.write(ALARM_POS);
    servo4.write(ALARM_POS);
    servo5.write(ALARM_POS);
    delay(1000);
    resetServos();
    delay(500);
  } else {

    // Roll → servo 1 & 2
    int angleRoll = map(roll, -90, 90, 0, 180);
    servo1.write(180 - angleRoll);
    servo2.write(180 - angleRoll);

    // Pitch → servo 3 & 4
    int anglePitch = map(pitch, -90, 90, 0, 180);
    servo3.write(anglePitch);
    servo4.write(anglePitch);

    // Yaw → servo 5
    if (abs(yaw) > 5) {
      int angleYaw = map(yaw, -90, 90, 0, 180);
      servo5.write(angleYaw);
      yawTimer = millis();
      returnYaw = false;
    } else {
      if (!returnYaw && (millis() - yawTimer > 1000)) {
        servo5.write(INITIAL_POS);
        returnYaw = true;
      }
    }
  }

  delay(50);
}
