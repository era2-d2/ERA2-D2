// Sensores
const byte s1 = A15;
const byte s2 = A10;
const byte sc = A7;
const byte s3 = A5;
const byte s4 = A0;

const int calibra1 = 680;
const int calibra2 = 890;
const int calibra3 = 890;
const int calibra4 = 720;

// Calibração de cores
/*
const int amarelo = // Calibração amarela
const int verde = // Calibração verde
const int vermelho = // Calibração vermelho
*/

const bool PRETO = 1;
const bool BRANCO = 0;

int ler[5] = {0,0,0,0,0};


// Motores
const byte m1a = 22;
const byte m1b = 23;
const byte m2b = 24;
const byte m2a = 25;
const byte m1v = 2;
const byte m2v = 3;

byte velocidadeFrente = 90;
byte velocidadeTras = 90;



void Mostrar() {
  Serial.print(analogRead(s1));
  Serial.print("   ");
  Serial.print(analogRead(s2));
  Serial.print("   ");
  Serial.print(analogRead(sc));
  Serial.print("   ");
  Serial.print(analogRead(s3));
  Serial.print("   ");
  Serial.print(analogRead(s4));
  Serial.println();
}

void Parar(){
  digitalWrite(m1a, 0);
  digitalWrite(m1b, 0);
  digitalWrite(m2a, 0);
  digitalWrite(m2b, 0);
}

void Frente(){
  digitalWrite(m1a, 1);
  digitalWrite(m1b, 0);
  analogWrite(m1v, velocidadeFrente);
  digitalWrite(m2a, 1);
  digitalWrite(m2b, 0);
  analogWrite(m2v, velocidadeFrente);
}

void Tras(){
  digitalWrite(m1a, 1);
  digitalWrite(m1b, 0);
  analogWrite(m1v, velocidadeTras);
  digitalWrite(m2a, 1);
  digitalWrite(m2b, 0);
  analogWrite(m2v, velocidadeTras);
}

void Esquerda(){
  digitalWrite(m1a, 0);
  digitalWrite(m1b, 1);
  analogWrite(m1v, 100);
  digitalWrite(m2a, 1);
  digitalWrite(m2b, 0);
  analogWrite(m2v, 150);
}

void Direita(){
  digitalWrite(m1a, 1);
  digitalWrite(m1b, 0);
  analogWrite(m1v, 150);
  digitalWrite(m2a, 0);
  digitalWrite(m2b, 1);
  analogWrite(m2v, 100);
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

  // LDR 3 (Identificador de Cores)
  /* if (analogRead(sc) > calibra1) {
    ler[2] == PRETO;
  }
  else {
    ler[2] == BRANCO;
  } */

  
  // LDR 4
  if (analogRead(s3) > calibra3) {
    ler[3] == PRETO;
  }
  else {
    ler[3] == BRANCO;
  }

  // LDR 5
  if (analogRead(s4) > calibra4) {
    ler[4] == PRETO;
  }
  else {
    ler[4] == BRANCO;
  } 
}

void Seguir_Linha() {
  
  Ler_Sensores();
 
  while(ler[1] == PRETO) { 
    Ler_Sensores();
    Esquerda();
  }
           
  while(ler[3] == PRETO) {
    Ler_Sensores();
    Direita();
  }
  
}


void setup() {
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
  pinMode(sc, INPUT);
  pinMode(s3, INPUT);
  pinMode(s4, INPUT);
  Serial.begin(9600);
}

void loop() {
  //Mostrar();
  Seguir_Linha();
}
