#include "driver/mcpwm.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// PINOS
#define MCPWM_PIN 25   // Saida PWM principal
#define IN1_PIN   26   // LED verde: sentido horario
#define IN2_PIN   27   // LED vermelho: sentido anti-horario
#define POT_PIN   34   // Potenciometro
#define BTN_PIN   14   // Botao de inversao

// VARIAVEIS
bool sentidoHorario = true;

int valorADC = 0;
float dutyCycle = 0.0;

int estadoBotaoAtual = HIGH;
int estadoBotaoAnterior = HIGH;

unsigned long ultimoDebounce = 0;
const unsigned long tempoDebounce = 250;

unsigned long ultimoTempoSerial = 0;
unsigned long ultimoTempoOLED = 0;

// CONFIGURACAO DO MCPWM 
void configurarMCPWM() {
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, MCPWM_PIN);

  mcpwm_config_t pwm_config;
  pwm_config.frequency = 1000;      // Frequencia de 1 kHz
  pwm_config.cmpr_a = 0.0;          // Duty inicial
  pwm_config.cmpr_b = 0.0;
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
}

// ATUALIZA SENTIDO
void atualizarSentido() {
  if (sentidoHorario) {
    digitalWrite(IN1_PIN, HIGH);  // LED verde aceso
    digitalWrite(IN2_PIN, LOW);   // LED vermelho apagado
  } else {
    digitalWrite(IN1_PIN, LOW);   // LED verde apagado
    digitalWrite(IN2_PIN, HIGH);  // LED vermelho aceso
  }
}

// ATUALIZA OLED 
void atualizarOLED() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("Controle MCPWM");

  display.setCursor(0, 16);
  display.print("ADC: ");
  display.println(valorADC);

  display.setCursor(0, 28);
  display.print("Duty: ");
  display.print(dutyCycle, 1);
  display.println(" %");

  display.setCursor(0, 40);
  display.print("Sentido: ");
  if (sentidoHorario) {
    display.println("Horario");
  } else {
    display.println("Anti-hor.");
  }

  display.setCursor(0, 54);
  display.println("PWM no GPIO 25");

  display.display();
}

// SETUP
void setup() {
  Serial.begin(115200);

  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Erro ao iniciar OLED");
    while (true);
  }

  configurarMCPWM();
  atualizarSentido();

  Serial.println("Sistema iniciado.");
  Serial.println("Controle PWM com MCPWM");
  Serial.println("Potenciometro controla o duty cycle");
  Serial.println("Botao inverte o sentido");
}

// LOOP
void loop() {
  // Leitura do potenciometro
  valorADC = analogRead(POT_PIN);

  // Conversao ADC 0-4095 para duty 0-100%
  dutyCycle = map(valorADC, 0, 4095, 0, 1000) / 10.0;

  // Atualiza MCPWM no GPIO 25
  mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, dutyCycle);
  mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);

  // Leitura do botao
  estadoBotaoAtual = digitalRead(BTN_PIN);

  if (estadoBotaoAnterior == HIGH && estadoBotaoAtual == LOW) {
    if (millis() - ultimoDebounce > tempoDebounce) {
      sentidoHorario = !sentidoHorario;
      atualizarSentido();

      Serial.print("Botao pressionado. Novo sentido: ");
      if (sentidoHorario) {
        Serial.println("Horario");
      } else {
        Serial.println("Anti-horario");
      }

      ultimoDebounce = millis();
    }
  }

  estadoBotaoAnterior = estadoBotaoAtual;

  // Atualiza Serial
  if (millis() - ultimoTempoSerial >= 500) {
    ultimoTempoSerial = millis();

    Serial.print("ADC: ");
    Serial.print(valorADC);
    Serial.print(" | Duty MCPWM: ");
    Serial.print(dutyCycle, 1);
    Serial.print(" % | Sentido: ");

    if (sentidoHorario) {
      Serial.println("Horario");
    } else {
      Serial.println("Anti-horario");
    }
  }

  // Atualiza OLED
  if (millis() - ultimoTempoOLED >= 300) {
    ultimoTempoOLED = millis();
    atualizarOLED();
  }
}
