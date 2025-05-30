// Bibliotecas.
#include <EEPROM.h>

// Sensores.
const byte s1 = A15;
const byte s2 = A10;
const byte s3 = A7;
const byte s4 = A5;
const byte s5 = A0;
int ler[5] = {0};

// Calibração dos sensores.
int calibra1 = 4 * (EEPROM.read(0));
int calibra2 = 4 * (EEPROM.read(1));
int calibra3 = 4 * (EEPROM.read(2));
int calibra4 = 4 * (EEPROM.read(3));
int calibra5 = 4 * (EEPROM.read(4));

// Calibração de cores.
/*
  const int AMARELO = ; // Calibração amarela
  const int VERDE = ; // Calibração verde
  const int VERMELHO = ; // Calibração vermelho
*/

const bool PRETO = 1;
const bool BRANCO = 0;

// Sistema PD
// Constantes controlador PD
const float Kp = 35; // Ganho proporcional.
const float Kd = 18; // Ganho derivativo.

// Variáveis de controle.
float P = 0, D = 0, PD = 0;
int velEsq = 0, velDir = 0;
int erro = 0, erroAnterior = 0;

// Motores.
const byte m1a = 22;
const byte m1b = 23;
const byte m2a = 24;
const byte m2b = 25;
const byte m1v = 2;
const byte m2v = 3;

const byte vel1 = 140;
const byte vel2 = 130;

int velMax = 255;
int velMin = 0;

void Ler_Sensores() {

  // LDR 1
  if (analogRead(s1) > calibra1) {
    ler[0] = PRETO;
  }
  else {
    ler[0] = BRANCO;
  }

  // LDR 2
  if (analogRead(s2) > calibra2) {
    ler[1] = PRETO;
  }
  else {
    ler[1] = BRANCO;
  }

  // LDR 3
  if (analogRead(s3) > calibra3) {
    ler[2] = PRETO;
  }
  else {
    ler[2] = BRANCO;
  }

  // LDR 4
  if (analogRead(s4) > calibra4) {
    ler[3] = PRETO;
  }
  else {
    ler[3] = BRANCO;
  }

  // LDR 5
  if (analogRead(s5) > calibra5) {
    ler[4] = PRETO;
  }
  else {
    ler[4] = BRANCO;
  }
}

void Calcula_Erro() {

  // Sensor Central
  if ((ler[0] == BRANCO) && (ler[1] == BRANCO) && (ler[2] == PRETO) && (ler[3] == BRANCO) && (ler[4] == BRANCO)) erro = 0;

  // Sensores da Direita
  else if ((ler[0] == BRANCO) && (ler[1] == BRANCO) && (ler[2] == PRETO) && (ler[3] == PRETO) && (ler[4] == BRANCO)) erro = 1;
  else if ((ler[0] == BRANCO) && (ler[1] == BRANCO) && (ler[2] == BRANCO) && (ler[3] == PRETO) && (ler[4] == BRANCO)) erro = 2;
  else if ((ler[0] == BRANCO) && (ler[1] == BRANCO) && (ler[2] == BRANCO) && (ler[3] == PRETO) && (ler[4] == PRETO)) erro = 3;
  else if ((ler[0] == BRANCO) && (ler[1] == BRANCO) && (ler[2] == BRANCO) && (ler[3] == BRANCO) && (ler[4] == PRETO)) erro = 4;

  // Sensores da Esquerda
  else if ((ler[0] == BRANCO) && (ler[1] == PRETO) && (ler[2] == PRETO) && (ler[3] == BRANCO) && (ler[4] == BRANCO)) erro = -1;
  else if ((ler[0] == BRANCO) && (ler[1] == PRETO) && (ler[2] == BRANCO) && (ler[3] == BRANCO) && (ler[4] == BRANCO)) erro = -2;
  else if ((ler[0] == PRETO) && (ler[1] == PRETO) && (ler[2] == BRANCO) && (ler[3] == BRANCO) && (ler[4] == BRANCO)) erro = -3;
  else if ((ler[0] == PRETO) && (ler[1] == BRANCO) && (ler[2] == BRANCO) && (ler[3] == BRANCO) && (ler[4] == BRANCO)) erro = -4;

}

void Calculo_PD() {

  P = erro*Kp;
  D = (erro - erroAnterior)*Kd;
  PD = P + D;
  erroAnterior = erro;

}

// Controle dos motores.
void Motores() {
  
  if (PD >= 0) {
    velEsq = vel1 + PD; //TESTE
    velDir = vel2 - PD;
  }
  else {
    velEsq = vel1 + PD;
    velDir = vel2 - PD; //TESTE
  }

  if (velDir > velMax) {
    velDir = velMax;
  }

  if (velDir < velMin) {
    velDir = -1*vel2;
  }

  if (velEsq > velMax) {
    velEsq = velMax;
  }

  if (velEsq < velMin) {
    velEsq = -1*vel1;
  }

  digitalWrite(m1a, HIGH);
  digitalWrite(m1b, LOW);
  analogWrite(m1v, velEsq);

  digitalWrite(m2a, HIGH);
  digitalWrite(m2b, LOW);
  analogWrite(m2v, velDir);

}

void setup() {

  // Motores.
  pinMode(m1a, OUTPUT);
  pinMode(m1b, OUTPUT);
  pinMode(m2a, OUTPUT);
  pinMode(m2b, OUTPUT);
  pinMode(m1v, OUTPUT);
  pinMode(m2v, OUTPUT);

  // Sensores.
  pinMode(s1, INPUT);
  pinMode(s2, INPUT);
  pinMode(s3, INPUT);
  pinMode(s4, INPUT);
  pinMode(s5, INPUT);
  Serial.begin(9600);

}

void loop() {

    Ler_Sensores();
    Calcula_Erro();
    Calculo_PD();
    Motores();
    Serial.print(velEsq);
    Serial.print("       ");
    Serial.print(velDir);
    Serial.println();
}
