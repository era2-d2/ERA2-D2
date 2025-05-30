// Código de Calibração Automática do Sensor de Cor.
// Biblioteca.
#include <EEPROM.h>

// Sensores.
const byte s3 = A7;

unsigned int Tempo = 0;

// Mínimos e Máximos de leitura da cor vermelha.
int vermelhoMin;
int vermelhoMax;

// Variáveis.
int valor = 0;
int maiorValor = 0;
int menorValor = 0;

void Ler_Sensores() {

  while (Tempo < 10000) {

    Tempo = millis();

      valor = analogRead(s3);

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

void Armazena(){

// Mínimos e Máximos de leitura da cor vermelha.
vermelhoMin = menorValor/4;
vermelhoMax = (maiorValor - ((maiorValor - menorValor)/3))/4;
  
  EEPROM.write(5, vermelhoMin);
  EEPROM.write(6, vermelhoMax);

}

void setup() {
  
  pinMode(s3, INPUT);

}

void loop() {

  Ler_Sensores();
  Armazena();

}
