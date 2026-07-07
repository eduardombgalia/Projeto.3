#include <Arduino.h>

const int pinR = 25;
const int pinG = 26;
const int pinB = 27;

// Parâmetros do PWM
const int freq = 5000;      
const int resolucao = 8;

// Ciclo de trabalho
int dutyR = 0, dutyG = 0, dutyB = 0;

// Incrementos para cada cor
const int incG = 5;   
const int incB = 10; 
const int incR = 15;  

void setup() {
  Serial.begin(115200); 
  
  // Configura frequência, resolução e associa ao pino
  ledcAttach(pinR, freq, resolucao);
  ledcAttach(pinG, freq, resolucao);
  ledcAttach(pinB, freq, resolucao);
}

void loop() {
  // Incrementa e aplica o operador mod (%) para resetar ao atingir 256 
  dutyR = (dutyR + incR) % 256;
  dutyG = (dutyG + incG) % 256;
  dutyB = (dutyB + incB) % 256;

  // Pino função de escrita
  ledcWrite(pinR, dutyR);
  ledcWrite(pinG, dutyG);
  ledcWrite(pinB, dutyB);

  // Monitoramento via UART
  Serial.printf("Incrementos -> R: %d, G: %d, B: %d | Duty Atual -> R: %d, G: %d, B: %d\n", 
                incR, incG, incB, dutyR, dutyG, dutyB);

  delay(200);
}
