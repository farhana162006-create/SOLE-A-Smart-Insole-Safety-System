#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
// MOVING AVERAGE FILTER
const int WINDOW_SIZE = 10;
float mag_buffer[WINDOW_SIZE];
float tilt_buffer[WINDOW_SIZE];
float ay_buffer[WINDOW_SIZE];  // NEW: Vertical movement tracking
int buffer_index = 0;
bool buffer_full = false;

float avg_mag = 1.0;
float avg_tilt = 0.0;
float avg_ay = 0.0;

void setup() {
  Serial.begin(9600);
  Wire.begin(D2, D1);

  // Initialize buffers
  for(int i = 0; i < WINDOW_SIZE; i++) {
    mag_buffer[i] = 1.0;
    tilt_buffer[i] = 0.0;
    ay_buffer[i] = 0.0;
  }

  Serial.println("Initializing MPU6050...");
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 NOT FOUND");
    delay(2000); return;
  }

  Serial.println("READY - SHOE ACTIVITY DETECTION");
  Serial.println("Ax  Ay  Az  | AVG_Mag | AVG_Tilt | Activity");
}

float get_moving_average(float buffer[], float new_value) {
  buffer[buffer_index] = new_value;
  buffer_index = (buffer_index + 1) % WINDOW_SIZE;
  if (!buffer_full) buffer_full = (buffer_index == 0);

  float sum = 0;
  for(int i = 0; i < WINDOW_SIZE; i++) {
    sum += buffer[i];
  }
  return sum / WINDOW_SIZE;
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  float Ax = ax / 16384.0;
  float Ay = ay / 16384.0;  // VERTICAL AXIS (shoe up-down)
  float Az = az / 16384.0;

  float raw_mag = sqrt(Ax*Ax + Ay*Ay + Az*Az);
  float raw_pitch = atan2(Ax, sqrt(Ay*Ay + Az*Az)) * 180 / PI;
  float raw_roll  = atan2(Ay, sqrt(Ax*Ax + Az*Az)) * 180 / PI;
  float raw_tilt  = max(abs(raw_pitch), abs(raw_roll));

  // SMOOTH ALL VALUES
  avg_mag = get_moving_average(mag_buffer, raw_mag);
  avg_tilt = get_moving_average(tilt_buffer, raw_tilt);
  avg_ay = get_moving_average(ay_buffer, abs(Ay));  // ABS for up+down peaks

  String activity = "STILL";

  // NORMAL WALKING = SIDE MOVEMENT (low Ay variation)
  if (avg_mag >= 0.6 && avg_mag < 0.9 && avg_tilt < 12 && avg_ay < 0.4) {
    activity = "WALKING";
  }
  // JOGGING = MEDIUM vertical + some side
  else if (avg_mag >= 0.9 && avg_mag < 1.2 && avg_ay >= 0.4 && avg_ay < 0.7) {
    activity = "JOGGING";
  }
  // RUNNING = HIGH vertical movement (Ay dominant)
  else if (avg_ay >= 0.7 && avg_mag >= 1.1) {
    activity = "RUNNING";
  }

  Serial.print(Ax, 2); Serial.print(" ");
  Serial.print(Ay, 2); Serial.print(" ");
  Serial.print(Az, 2); Serial.print(" | ");
  Serial.print(avg_mag, 2); Serial.print(" | ");
  Serial.print(avg_tilt, 1); Serial.print(" | ");
  Serial.println(activity);

  delay(100);
}
