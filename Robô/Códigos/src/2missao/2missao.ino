// Bibliotecas.
//#include <StarWars.h> // Música StarWars.
#include <VL53L0X.h>  // Sensor Laser.
#include <EEPROM.h>   // Memória EEPROM.
#include <HCSR04.h>   // Ultrassônico.
#include <Servo.h>    // Servomotores.
#include <Wire.h>     // Protocolo I2C.

// Pinos dos Motores.
#define mEa 29 // Pino Digital.
#define mEb 27 // Pino Digital.
#define mDa 25 // Pino Digital.
#define mDb 23 // Pino Digital.
#define mEv 2  // Pino Digital PWM.
#define mDv 3  // Pino Digital PWM.

// Pinos dos Encoders.
#define pinAD 18 // Canal A. // Interrupção nº 4
#define pinAE 19 // Canal A. // Interrupção nº 5
//#define pinBD  // Canal B.
//#define pinBD  // Canal B.

// Constantes dos Motores.
#define velE 255 // 100% de velocidade no Motor Esquerdo.
#define velD 217 // 85% de velocidade no Motor Direito.
const float debugEnc = 0.968; // Erro do encoder. Aumentar o valor (> 0.9) vai pra esquerda.
// ❤☺

const double perimetro = 10.68141502; // Perímetro da roda.
const double circunferencia = 54.35393641; // Circuinferência do robô (diâmetro = distância entre eixo das rodas).

// Parâmetros PID.
const double Kp = 5.8;  // 6    Constante Proporcional.
const double Ki = 0.6;  // 0.6  Constante Integrativa.
const double Kd = 0.7;  // 0.75 Constante Derivativa.
const double Tm = 0.03; // 0.03 // Tempo de amostragem média entre leituras.

double erro, erro1 = 0, erro2 = 0;
double PWME, PWMD, PWME1 = velE, PWMD1 = velD;

double distanciaE = 0, distanciaD = 0, distanciaT = 0;
unsigned long primeiroMillis;

volatile long pulsosE = 0, pulsosD = 0;

//---------------------------------------------------------------------------------ENCODER-------------------------------------------------------------------------------//
void ContadorEsq() { pulsosE++; }
void ContadorDir() { pulsosD++; }
void PIDEncoder() {

  if ((millis() - primeiroMillis) >= (Tm * 1000)) {
    noInterrupts();
    distanciaE = (double)(perimetro * pulsosE * debugEnc) / 1400;
    distanciaD = (double)(perimetro * pulsosD) / 1400;
    distanciaT = distanciaT + (double)(perimetro * pulsosE) / 1400;
    interrupts();

    pulsosD = pulsosE = 0;
    primeiroMillis = millis();
  }

  erro = distanciaD - distanciaE;

  //---------SATURAR PWM DE ENTRADA----------//
  if (PWME1 > 255) PWME1 = 255;
  if (PWME1 < 0) PWME1 = 0;

  if (PWMD1 > 255) PWMD1 = 255;
  if (PWMD1 < 0) PWMD1 = 0;

  //--------------CALCULO PID----------------//
  PWME = PWME1 + (double)((Kp + Kd / Tm) * erro + (Ki * Tm - 2 * Kd / Tm) * erro1 + (Kd / Tm) * erro2); // Retirou-se -Kp de (-Kp + Ki*Tm -2*Kd/Tm).
  PWMD = PWMD1 + (double)((Kp + Kd / Tm) * (-1*erro) + (Ki * Tm - 2 * Kd / Tm) * (-1 * erro1) + (Kd / Tm) * (-1 * erro2)); // O erro direito é sempre o oposto do esquerdo.
  PWME1 = PWME;
  PWMD1 = PWMD;
  erro2 = erro1;
  erro1 = erro;

  //----------SATURAR PWM DE SAÍDA-----------//
  if (PWME > 255) PWME = 255;
  if (PWME < 0) PWME = 0;

  if (PWMD > 255) PWMD = 255;
  if (PWMD < 0) PWMD = 0;

  //------------COMANDO MOTORES--------------//
  analogWrite(mEv, PWME);
  analogWrite(mDv, PWMD);
}

//----------------------------------------------------------------------------------GARRA--------------------------------------------------------------------------------//
Servo servo1; // 30 a 145 graus  - graus, mais pra tras.
Servo servo2; // 50 a 140 graus  - graus, mais alto.
Servo servo3; // 180 a 85 graus  - graus, garra fechada.

byte ultimoEstadoX = 0, ultimoEstadoY = 0, ultimoEstadoGarra;

// Funções da Garra.
void Garra(byte x, byte y, bool fechada = false) {
  if (x <= 30) x = 30;
  if (x >= 145) x = 145;
  
  // Servo1.
  if (ultimoEstadoX > x) {
    for (byte i = ultimoEstadoX; i >= x; i--) {
      servo1.write(i);
      delay(10);
    }
  } else if (ultimoEstadoX < x) {
    for (byte i = ultimoEstadoX; i <= x; i++) {
      servo1.write(i);
      delay(10);
    }
  }

  // Servo2.
  if (ultimoEstadoY > y) {
    for (byte i = ultimoEstadoY; i >= y; i--) {
      servo2.write(i);
      delay(12);
    }
  } else if (ultimoEstadoY < y) {
    for (byte i = ultimoEstadoY; i <= y; i++) {
      servo2.write(i);
      delay(12);
    }
  }

  // Servo3.
  if (fechada == true) {
    if(ultimoEstadoGarra = true) ultimoEstadoGarra = 90;
    for (byte i = ultimoEstadoGarra; i >= 90; i--) {
      servo3.write(i);
      delay(10);
    }
  }
  else  servo3.write(180);
  ultimoEstadoGarra = fechada;
  ultimoEstadoX = x;
  ultimoEstadoY = y;
}

void Inicial() {
  ultimoEstadoX = servo1.read();
  ultimoEstadoY = servo2.read();

  Garra(145, 70); // Posição inicial: Servo1 em 145º e Servo2 em 60º, garra aberta.
}

void LerArvore() { Garra(90, 110); } // Altura ideal para fazer a leitura das alturas das árvores. 
void ArvoreGrande() { Garra(70, 135); } // Altura ideal para pegar a árvore grande.
void ArvorePequena() { Garra(60, 130); } // Altura ideal para pegar a árvore pequena. 

//------------------------------------------------------------------------------SENSOR DE COR----------------------------------------------------------------------------//
#define sc A0 // Pino do Sensor de Cor.

bool CORPRETA;
bool leituraCorPreta;
byte contagemDerrubados = 0;

// Mínimos e Máximos de leitura da cor vermelha.
unsigned int CORVERMELHAMin;
unsigned int CORVERMELHAMax;

// Funções Sensor de Cor.
void SensorCor() {
  if (analogRead(sc) > CORVERMELHAMax) CORPRETA = 1;
  else CORPRETA = 0;
}

void SensorCorAux() {
  if (analogRead(sc) > CORVERMELHAMax) leituraCorPreta = 1;
  else leituraCorPreta = 0;
}

//----------------------------------------------------------------------------------LASER--------------------------------------------------------------------------------//
float distanciaUs1 = 0;   // Armazena a leitura de distância do Ultrassônico 1.
float distanciaUs2 = 0;   // Armazena a leitura de distância do Ultrassônico 2.
float distanciaLaser = 0; // Armazena a leitura de distância do Laser.

bool arvoreG; // Árvore Grande, verdadeira ou falsa. 

VL53L0X laser; // Declara o Laser 1.

// Função do Laser.
bool Laser() {
  
  distanciaLaser = (laser.readRangeContinuousMillimeters() - 20); // 20 é o erro do sensor.
 
  if (distanciaLaser <= 130.0) arvoreG = true;
  else arvoreG = false;

  Serial.print(distanciaLaser);
  Serial.print("     ");
  Serial.println(arvoreG);

  return arvoreG;
}

//------------------------------------------------------------------------------ULTRASSÔNICO-----------------------------------------------------------------------------//
// Pinos dos Ultrassônicos.
#define trig1 11 // Pino Digital.
#define trig2 10 // Pino Digital.
#define echo1 8  // Pino Digital.
#define echo2 9  // Pino Digital.

UltraSonicDistanceSensor us1(trig1, echo1); // Declara o Ultrassônico 1.
UltraSonicDistanceSensor us2(trig2, echo2); // Declara o Ultrassônico 2.

// Função do Ultrassônico.
bool Ultrassonico() {

  //distanciaUs1 = us1.measureDistanceCm();
  distanciaUs2 = us2.measureDistanceCm(); // (Medição do Ultrassônico 2 em centímetros).

  if (distanciaUs2 < 10) arvoreG = true; 
  else arvoreG = false;

  return arvoreG;
}

//---------------------------------------------------------------------------FUNÇÕES DOS MOTORES-------------------------------------------------------------------------//
void Para(unsigned long para) {

  digitalWrite(mEa, LOW);
  digitalWrite(mEb, LOW);
  digitalWrite(mDa, LOW);
  digitalWrite(mDb, LOW);
  analogWrite(mEv, 0);
  analogWrite(mDv, 0);
  delay(para);
}

void Direcao(float distancia, bool frente = true) {

  bool a = frente ? true : false;
  bool b = frente ? false : true;

  attachInterrupt(4, ContadorEsq, CHANGE);
  attachInterrupt(5, ContadorDir, CHANGE);
  
  digitalWrite(mEa, a);
  digitalWrite(mEb, b);
  digitalWrite(mDa, a);
  digitalWrite(mDb, b);
  analogWrite(mEv, velE);
  analogWrite(mDv, velD);
  primeiroMillis = millis();

  while (distanciaT < distancia) { PIDEncoder(); }
  
  digitalWrite(mEa, b);
  digitalWrite(mEb, a);
  digitalWrite(mDa, b);
  digitalWrite(mDb, a);
  analogWrite(mEv, velE);
  analogWrite(mDv, velD);
  delay(30);
  
  Para(50);
  detachInterrupt(4);
  detachInterrupt(5);
  PWME = velE, PWMD = velD, PWME1 = velE, PWMD1 = velD;
  erro = erro1 = erro2 = pulsosE = pulsosD = distanciaE = distanciaD = distanciaT = 0;
}

void Gira(float angulo, bool esquerda = true) {

  bool a = esquerda ? false : true;
  bool b = esquerda ? true : false;
  double distancia = (double)((angulo / 360) * circunferencia);

  attachInterrupt(4, ContadorEsq, CHANGE);
  attachInterrupt(5, ContadorDir, CHANGE);
  
  digitalWrite(mEa, a);
  digitalWrite(mEb, b);
  digitalWrite(mDa, b);
  digitalWrite(mDb, a);
  analogWrite(mEv, velE);
  analogWrite(mDv, velD);
  primeiroMillis = millis();

  while (distanciaT < distancia) { PIDEncoder(); }

  digitalWrite(mEa, b);
  digitalWrite(mEb, a);
  digitalWrite(mDa, a);
  digitalWrite(mDb, b);
  analogWrite(mEv, velE);
  analogWrite(mDv, velD);
  delay(30);
 
  Para(50);
  detachInterrupt(4);
  detachInterrupt(5);
  PWME = velE, PWMD = velD, PWME1 = velE, PWMD1 = velD;
  erro = erro1 = erro2 = pulsosE = pulsosD = distanciaE = distanciaD = distanciaT = 0;
}

//-------------------------------------------------------------------------FUNÇÕES COMPLEMENTARES------------------------------------------------------------------------//
void Turbo(float tempo) {

  digitalWrite(49, LOW);
  
  digitalWrite(mEa, HIGH);
  digitalWrite(mEb, LOW);
  digitalWrite(mDa, HIGH);
  digitalWrite(mDb, LOW);
  analogWrite(mEv, velE);
  analogWrite(mDv, velD);
  delay(tempo);
  
  digitalWrite(49, HIGH);
}

void Rampa(unsigned int tempo) {
  
  if(tempo > 4000) tempo = 4000;
  Garra(105, 65, true); // Garra fechada
  Garra(70, 65, true);  // Garra fechada

  Turbo(tempo);
  tempo = 2000;
  for (byte i = 48; i >= 20; i--) {
    servo1.write(i);
    delay(12);

    Turbo(tempo);
    break;
  }
}

void AbaixaRampa() {
  Garra(70, 180); // Garra aberta
  Garra(50, 180); // Garra aberta
}

void Cor() {
  Garra(72, 65, true); // Garra fechada
  servo2.write(65);    // Força o Servo
}

//--------------------------------------------------------------------------------------SETUP----------------------------------------------------------------------------//
void setup() {

  // Motores.
  pinMode(mEa, OUTPUT);
  pinMode(mEb, OUTPUT);
  pinMode(mDa, OUTPUT);
  pinMode(mDb, OUTPUT);
  pinMode(mEv, OUTPUT);
  pinMode(mDv, OUTPUT);

  // Turbo.
  pinMode(49, OUTPUT);
  digitalWrite(49, HIGH); // Desliga o Turbo (relé). Lógica invertida devido a eletrônica. 

  // Servomotores.
  servo1.attach(5);
  servo2.attach(6);
  servo3.attach(7);
  Inicial(); // Desloca a garra para a posição inicial.

  // Sensor de Cor.
  pinMode(sc, INPUT);

  // Laser.
  Wire.begin();
  laser.setTimeout(500);
  while (!laser.init());

  laser.startContinuous(); // Inicia o modo contínuo consecutivo (Lê o mais rápido possível).
  // Para usar o modo de tempo contínuo, forneça um período entre as medições em millissegundos "laser.startContinuous(100)".

  // Ultrassônicos.
  pinMode(trig1, OUTPUT);
  pinMode(trig2, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(echo2, INPUT);

  // Encoder Esquerdo.
  pinMode(pinAE, INPUT_PULLUP); // Canal A.
  //pinMode(pinBE, INPUT_PULLUP); // Canal B.

  // Encoder Direito.
  pinMode(pinAD, INPUT_PULLUP); // Canal A.
  //pinMode(pinBD, INPUT_PULLUP); // Canal B.

  Serial.begin(9600);
  // Interrupções.
  attachInterrupt(4, ContadorEsq, CHANGE);
  attachInterrupt(5, ContadorDir, CHANGE);

  // StarWars Music Theme.
  // ImperialMarch march(45, 108);

  // Inicializações.
  CORVERMELHAMin = 4 * EEPROM.read(5);
  CORVERMELHAMax = 4 * EEPROM.read(6);
}

//-------------------------------------------------------------------------------------LOOP-----------------------------------------------------------------------------//
void loop() {

//--------------------------------------------------------------------------------PRIMEIRA MISSÃO-----------------------------------------------------------------------//
  //TesteMotores(); // Utilizar APENAS para calibrar a velocidade dos motores.
  Direcao(33); // Frente
  Gira(93, false); // Direita

           //------------------------------------------------------------------------------SEGUNDA MISSÃO---------------------------------------------------------------------------//
  Direcao(20);     // Frente
  Rampa(1200);     // Turbo 
  Direcao(20);     // Frente
  
  Cor();
  
  Direcao(15);     // Frente
  
  SensorCor();

  Garra(105, 50, true); // Garra fechada

  Direcao(32, false); // Trás
  Gira(90, false);    // Direita
  Direcao(32);        // Frente

  for (byte y = 0; y < 4; y++) {

    SensorCorAux(); // Lê a cor

    if (leituraCorPreta == CORPRETA) {

      if (contagemDerrubados == 4) break;

      Direcao(10, false); // Trás
      Gira(40, false);    // Direita

      Garra(40, 50, true); // Garra fechada

      Gira(42); // Esquerda
      contagemDerrubados++; // Acrescenta 1 a mais na contagem dos poluentes derrubados.

      Garra(105, 50, true); // Garra fechada

      if (y == 3) break;
    }

    else {

      if (y == 3) break;
      Direcao(10, false); // Trás
    }

    if (contagemDerrubados >= 4) break;

    Gira(90);        // Esquerda
    Direcao(25.5);   // Frente
    Gira(90, false); // Direita
    Direcao(11.2);   // Frente
  }

  // Caso não estejam todos derrubados do lado inicial.
  if (contagemDerrubados < 4) {

    if (leituraCorPreta == CORPRETA) Direcao(18, false); // Trás
    else Direcao(25, false);                             // Trás

    Gira(90, false); // Direita
    Direcao(40);     // Frente

                                 //----------------------------------------------------OUTRO LADO------------------------------------------------------//

    Direcao(27, false); // Trás
    Gira(90, false);    // Direita
    Direcao(33);        // Frente

    for (int y = 0; y < 4; y++) {

      if (contagemDerrubados == 4) break;

      SensorCorAux();

      if (leituraCorPreta == CORPRETA) {

        if (contagemDerrubados == 4) break;

        Direcao(9, false); // Trás
        Gira(40);          // Esquerda

        Garra(40, 55, true); // Garra fechada

        Gira(42, false); // Direita
        contagemDerrubados++;

        Garra(105, 50, true); // Garra fechada

        if (y == 3) break;
      }

      else {

        if (y == 3) break;
        Direcao(10, false); // Trás
      }

      if (contagemDerrubados >= 4) break;

      Gira(90);        // Esquerda
      Direcao(26.8);   // Frente
      Gira(90, false); // Direita
      Direcao(12);     // Frente
    }
  }

  else {
    //march.playMelody();
    delay(600000); // 10 minutos.
  }
  //march.playMelody();
  delay(600000);   // 10 minutos.
}

void TesteMotores() {
  digitalWrite(mEa, HIGH);
  digitalWrite(mEb, LOW);
  digitalWrite(mDa, HIGH);
  digitalWrite(mDb, LOW);
  analogWrite(mEv, velE);
  analogWrite(mDv, velD);
  delay(60000); // 1 minuto.
}
