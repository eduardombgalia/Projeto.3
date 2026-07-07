#include <ESP32Servo.h>

// Criação do objeto para o servo
Servo Servo;

const int pPotenciometro = 34; 
const int pServo = 13;  

void setup() {
  // Inicializa a comunicação serial para monitoramento
  Serial.begin(115200);
  // Configura a resolução de leitura do ADC para 12 bits
  analogReadResolution(12);

  // Aloca o temporizador para o servo
  ESP32PWM::allocateTimer(0);
  
  Servo.setPeriodHertz(50);
  
  // Associa o objeto do servo à GPIO correspondente 
  Servo.attach(pServo, 500, 2400); 
}

void loop() {
  // Leitura analógica do potenciômetro
  int valorADC = analogRead(pPotenciometro);
  
  // Mapeia o valor lido do ADC para a escala angular do servo
  int angulo = map(valorADC, 0, 4095, 0, 180);
  
  // Envia o comando de posição para o servomotor
  Servo.write(angulo);
  
  // Envia os dados via interface serial UART
  Serial.printf("ADC Potenciometro: %d | Angulo Calculado: %d graus\n", valorADC, angulo);
  
  // Atraso para estabilidade
  delay(20);
}
