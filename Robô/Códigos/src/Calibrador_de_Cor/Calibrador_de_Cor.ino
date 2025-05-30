// Código de Calibração Automática do Sensor de Cor.
// Biblioteca.
#include <EEPROM.h>
//#include <Servo.h>

// Sensores.
#define sc A0

// Tempo.
unsigned int Tempo = 0;

// Mínimos e Máximos de leitura da cor vermelha.
unsigned int vermelhoMin;
unsigned int vermelhoMax;

// Variáveis.
unsigned int valor = 0;
unsigned int maiorValor = 0;
unsigned int menorValor = 0;

void LerSensores() {

  while (Tempo < 15000) {

    Tempo = millis();
    valor = analogRead(sc);

    if (valor > maiorValor) {
      maiorValor = valor;

      if (menorValor == 0) {
        menorValor = maiorValor;
      }
    }

    if (menorValor > valor) {
      menorValor = valor;
    }
  }
}

void Armazena() {

  // Mínimos e Máximos de leitura da cor vermelha.
  vermelhoMin = menorValor / 4;
  vermelhoMax = (maiorValor - ((maiorValor - menorValor) / 3)) / 4;

  // Salvamento dos dados na memória EEPROM.
  EEPROM.write(5, vermelhoMin);
  EEPROM.write(6, vermelhoMax);

}

void setup() {

  pinMode(sc, INPUT);
  Serial.begin(9600);

  Serial.print("Lendo...");
}

void loop() {
  LerSensores();
  Armazena();

  Serial.println("CALIBRADO!");
}

//Servo servo1; // 30 a 145 graus  - graus, mais pra tras.
//Servo servo2; // 50 a 140 graus  - graus, mais alto.
//Servo servo3; // 180 a 85 graus  - graus, garra fechada.
//
//byte ultimoEstadoX = 0, ultimoEstadoY = 0, ultimoEstadoGarra;
//
//// Funções da Garra.
//void Garra(byte x, byte y, bool fechada = false) {
//  if (x <= 30) x = 30;
//  if (x >= 145) x = 145;
//  
//  // Servo1.
//  if (ultimoEstadoX > x) {
//    for (byte i = ultimoEstadoX; i >= x; i--) {
//      servo1.write(i);
//      delay(10);
//    }
//  } else if (ultimoEstadoX < x) {
//    for (byte i = ultimoEstadoX; i <= x; i++) {
//      servo1.write(i);
//      delay(10);
//    }
//  }
//
//  // Servo2.
//  if (ultimoEstadoY > y) {
//    for (byte i = ultimoEstadoY; i >= y; i--) {
//      servo2.write(i);
//      delay(12);
//    }
//  } else if (ultimoEstadoY < y) {
//    for (byte i = ultimoEstadoY; i <= y; i++) {
//      servo2.write(i);
//      delay(12);
//    }
//  }
//
//  // Servo3.
//  if (fechada == true) {
//    if(ultimoEstadoGarra = true) ultimoEstadoGarra = 90;
//    for (byte i = ultimoEstadoGarra; i >= 90; i--) {
//      servo3.write(i);
//      delay(10);
//    }
//  }
//  else  servo3.write(180);
//  ultimoEstadoGarra = fechada;
//  ultimoEstadoX = x;
//  ultimoEstadoY = y;
//}
