#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

const int WINDOW_SIZE = 5;
float mag_buffer[WINDOW_SIZE];
int buffer_index = 0;

float avg_dynamic = 0;
String current_state = "STILL";

unsigned long state_timer = 0;
unsigned long confirm_time = 400;

void setup() {
  Serial.begin(9600);
  Wire.begin(D2, D1);

  for(int i=0;i<WINDOW_SIZE;i++)
    mag_buffer[i] = 0;

  mpu.initialize();

  if(!mpu.testConnection()){
    Serial.println("MPU6050 NOT FOUND");
    while(1);
  }

  Serial.println("DEMO MODE READY");
}

float moving_avg(float new_val){
  mag_buffer[buffer_index] = new_val;
  buffer_index = (buffer_index + 1) % WINDOW_SIZE;

  float sum = 0;
  for(int i=0;i<WINDOW_SIZE;i++)
    sum += mag_buffer[i];

  return sum / WINDOW_SIZE;
}

void loop() {

  int16_t ax, ay, az;
  mpu.getAcceleration(&ax,&ay,&az);

  float Ax = ax / 16384.0;
  float Ay = ay / 16384.0;
  float Az = az / 16384.0;

  float raw_mag = sqrt(Ax*Ax + Ay*Ay + Az*Az);
  float dynamic = abs(raw_mag - 1.0);

  avg_dynamic = moving_avg(dynamic);

  String detected_state = current_state;

  // FALL (instant detection)
  if(raw_mag > 2.5){
    detected_state = "FALL DETECTED";
  }
  else if(avg_dynamic < 0.05){
    detected_state = "STILL";
  }
  else if(avg_dynamic >= 0.05 && avg_dynamic < 0.25){
    detected_state = "WALKING";
  }
  else if(avg_dynamic >= 0.25){
    detected_state = "RUNNING";
  }

  // State confirmation
  if(detected_state != current_state){
    if(millis() - state_timer > confirm_time){
      current_state = detected_state;
      state_timer = millis();
    }
  } else {
    state_timer = millis();
  }

  Serial.print("Dynamic: ");
  Serial.print(avg_dynamic,3);
  Serial.print(" | Activity: ");
  Serial.println(current_state);

  delay(20);
}
