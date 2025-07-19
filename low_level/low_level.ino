#include <Servo.h>

Servo servo0; // Постоянного вращения
Servo servo1; // Обычный 180°
const int servo0_pin = 10;
const int servo1_pin = 11;
const int relay_pin = 9;

int servoCurrentPos = 90; // Текущая позиция сервопривода
int servoTargetPos = 90; // Целевая позиция сервопривода
uint32_t servoTimer = 0; // Таймер для контроля скорости перемещения сервопривода
int servoDelay = 5; // Задержка между шагами перемещения сервопривода

void setup() {
  Serial.begin(9600);
  servo0.attach(servo0_pin);
  servo1.attach(servo1_pin);
  pinMode(relay_pin, OUTPUT);

  servo1.write(servoCurrentPos); // начальный угол
  servo0.write(90); // остановка
}

void loop() {
  // === Чтение сериал данных ===
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    int sep1 = input.indexOf('|');
    int sep2 = input.lastIndexOf('|');

    if (sep1 != -1 && sep2 != -1 && sep1 != sep2) {
      int speed = input.substring(0, sep1).toInt();            // -100 ... 100
      int angle = input.substring(sep1 + 1, sep2).toInt();     // 0 ... 180
      int relay = input.substring(sep2 + 1).toInt();           // 0 or 1

      // === Servo 0 — постоянного вращения ===
      servo0.write(speed);

      // === Servo 1 — обычный, целевая позиция ===
      servoTargetPos = angle;

      // === Реле ===
      digitalWrite(relay_pin, relay ? LOW : HIGH);
    }
  }

  // === Плавное движение сервомотора 1 ===
  servoPosControl();
}

void servoPosControl() {
  // Проверяем, прошло ли достаточно времени с последнего изменения положения
  if (millis() - servoTimer > servoDelay) {
    int delta = 0; // Разница между текущим и целевым положением
    // Определяем направление движения сервопривода
    if (servoCurrentPos > servoTargetPos){
      delta = -1;
    } else if (servoCurrentPos < servoTargetPos){
      delta = 1;
    }
    // Обновляем текущую позицию и время
    servoCurrentPos += delta;
    servoTimer = millis();
    // Устанавливаем новую позицию сервопривода
    servo1.write(servoCurrentPos);
  }
}
