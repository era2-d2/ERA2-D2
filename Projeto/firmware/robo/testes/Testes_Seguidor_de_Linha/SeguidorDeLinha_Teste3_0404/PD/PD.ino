// Sensores
const byte s1 = A15;
const byte s2 = A10;
const byte s3 = A7;
const byte s4 = A5;
const byte s5 = A0;

int calibra1 = 850;
int calibra2 = 950;
int calibra3 = 0;
int calibra4 = 950;
int calibra5 = 850;

//Calibração de cores
/*
const int amarelo = // Calibração amarela
const int verde = // Calibração verde
const int vermelho = // Calibração vermelho
*/

const bool PRETO = 1;
const bool BRANCO = 0;

int ler[5] = {0};


// Sistema PID

// Constantes controlador PID
const float Kp = 2; // Ganho proporcional
const float Kd = 0; // Ganho derivativo

// Variáveis de controle
float P = 0, D = 0, PD = 0;
int velEsq = 0, velDir = 0;
int erro = 0, erroAnterior = 0;



// Motores
const byte m1a = 22;
const byte m1b = 23;
const byte m2a = 24;
const byte m2b = 25;
const byte m1v = 2;
const byte m2v = 3;

byte velA = 110;
byte velB = 110;

int velMax = 128;
int velMin = 100;


// Mostrar os resultados lidos

void Mostrar() {
  Serial.print(analogRead(s1));
  Serial.print("   ");
  Serial.print(analogRead(s2));
  Serial.print("   ");
  Serial.print(analogRead(s3));
  Serial.print("   ");
  Serial.print(analogRead(s4));
  Serial.print("   ");
  Serial.print(analogRead(s5));
  Serial.println();
}


void Ler_Sensores() {

  // LDR 1
  if (analogRead(s1) > calibra1) {
    ler[0] == PRETO;
  }
  else {
    ler[0] == BRANCO;
  }
  
  // LDR 2
  if (analogRead(s2) > calibra2) {
    ler[1] == PRETO;
  }
  else {
    ler[1] == BRANCO;
  }

  // LDR 3
  if (analogRead(s2) > calibra3) {
    ler[2] == PRETO;
  }
  else {
    ler[2] == BRANCO;
  }
  
  // LDR 4
  if (analogRead(s3) > calibra4) {
    ler[3] == PRETO;
  }
  else {
    ler[3] == BRANCO;
  }

  // LDR 5
  if (analogRead(s4) > calibra5) {
    ler[4] == PRETO;
  }
  else {
    ler[4] == BRANCO;
  } 
}

void Calcula_Erro(){
    // Central
    if((ler[0] == BRANCO)&&(ler[1] == BRANCO)&&(ler[2] == PRETO)&&(ler[3] == BRANCO)&&(ler[4] == BRANCO)) erro = 0;

    // Direita
    else if((ler[0] == BRANCO)&&(ler[1] == BRANCO)&&(ler[2] == PRETO)&&(ler[3] == PRETO)&&(ler[4] == BRANCO)) erro = 1;
    else if((ler[0] == BRANCO)&&(ler[1] == BRANCO)&&(ler[2] == BRANCO)&&(ler[3] == PRETO)&&(ler[4] == BRANCO)) erro = 2;
    else if((ler[0] == BRANCO)&&(ler[1] == BRANCO)&&(ler[2] == BRANCO)&&(ler[3] == PRETO)&&(ler[4] == PRETO)) erro = 3;
    else if((ler[0] == BRANCO)&&(ler[1] == BRANCO)&&(ler[2] == BRANCO)&&(ler[3] == BRANCO)&&(ler[4] == PRETO)) erro = 4;

    // Esquerda
    else if((ler[0] == BRANCO)&&(ler[1] == PRETO)&&(ler[2] == PRETO)&&(ler[3] == BRANCO)&&(ler[4] == BRANCO)) erro = -1;
    else if((ler[0] == BRANCO)&&(ler[1] == PRETO)&&(ler[2] == BRANCO)&&(ler[3] == BRANCO)&&(ler[4] == BRANCO)) erro = -2;
    else if((ler[0] == PRETO)&&(ler[1] == PRETO)&&(ler[2] == BRANCO)&&(ler[3] == BRANCO)&&(ler[4] == BRANCO)) erro = -3;
    else if((ler[0] == PRETO)&&(ler[1] == BRANCO)&&(ler[2] == BRANCO)&&(ler[3] == BRANCO)&&(ler[4] == BRANCO)) erro = -4;
    
  }

void Calculo_PD() {
  
  P = erro;
  D = erro - erroAnterior;
  PD = (Kp * P) + (Kd * D);
  erroAnterior = erro;

}

// Controle dos motores

void Motores() {

  if (PD >= 0) {
    velEsq = velA;
    velDir = velB - PD;
  }
  else {
    velEsq = velA + PD;
    velDir = velB;
  }

  if (velDir > velMax) {
    velDir = velMax;
  }
  if (velDir < velMin) {
    velDir = velMin;
  }

  if (velEsq > velMax) {
    velEsq = velMax;
  }
  if (velEsq < velMin) {
    velEsq = velMin;
  }
  
  digitalWrite(m1a, HIGH);
  digitalWrite(m1b, LOW);
  digitalWrite(m2a, HIGH);
  digitalWrite(m2b, LOW);
  analogWrite(m1v, velDir);
  analogWrite(m2v, velEsq);
  
}

void setup(){
  
  // Motores
  pinMode(m1a, OUTPUT);
  pinMode(m1b, OUTPUT);
  pinMode(m2a, OUTPUT);
  pinMode(m2b, OUTPUT);
  pinMode(m1v, OUTPUT);
  pinMode(m2v, OUTPUT);

  // Sensores
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
 
}
