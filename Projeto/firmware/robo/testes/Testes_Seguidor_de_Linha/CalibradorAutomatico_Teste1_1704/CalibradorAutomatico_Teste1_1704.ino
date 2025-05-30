// Código de Calibração Automática dos Sensores
// Memória
#include <EEPROM.h>

// Sensores
const byte s1 = A15;
const byte s2 = A10;
const byte s3 = A7;
const byte s4 = A5;
const byte s5 = A0;

// Calibração dos sensores.
double calibra1;
double calibra2;
double calibra3;
double calibra4;
double calibra5;

unsigned int Tempo = 0;

// Variáveis
int sensores[5] = {s1, s2, s3, s4, s5};
int valor[5] = {0};
int maiorValor[5] = {0};
int menorValor[5] = {0};

void Ler_Sensores() {

  while (Tempo < 30000) {

    Tempo = millis();

    for (int i = 0; i < 5; i ++) {

      valor[i] = analogRead(sensores[i]);

      if (valor[i] > maiorValor[i]) {
        maiorValor[i] = valor[i];

        if (menorValor[i] == 0) {
          menorValor[i] = maiorValor[i];
        }
      }

      if (menorValor[i] > valor[i]) {
        menorValor[i] = valor[i];
      }
    }
  }
}

// Calcula e armazena os valores de calibração.
void Calcula() {

  calibra1 = menorValor[0] + ((maiorValor[0] - menorValor[0]) * 0.5); // 0,6 é 60% do valor, fica melhor. A média exata talvez não fique tão boa.
  calibra2 = menorValor[1] + ((maiorValor[1] - menorValor[1]) * 0.5);
  calibra3 = menorValor[2] + ((maiorValor[2] - menorValor[2]) * 0.5);
  calibra4 = menorValor[3] + ((maiorValor[3] - menorValor[3]) * 0.5); // O sensor ficou melhor com esse valor.
  calibra5 = menorValor[4] + ((maiorValor[4] - menorValor[4]) * 0.5);

  int v1 = calibra1 / 4;
  int v2 = calibra2 / 4;
  int v3 = calibra3 / 4;
  int v4 = calibra4 / 4;
  int v5 = calibra5 / 4;

  EEPROM.write(0, v1);
  EEPROM.write(1, v2);
  EEPROM.write(2, v3);
  EEPROM.write(3, v4);
  EEPROM.write(4, v5);

}

void setup() {

  pinMode(s1, INPUT);
  pinMode(s2, INPUT);
  pinMode(s3, INPUT);
  pinMode(s4, INPUT);
  pinMode(s5, INPUT);

}

void loop() {

  Ler_Sensores();
  Calcula();

}
