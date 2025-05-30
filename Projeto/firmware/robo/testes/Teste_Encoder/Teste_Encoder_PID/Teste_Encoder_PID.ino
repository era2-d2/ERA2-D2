#include <EEPROM.h>

#define pinD 18
#define pinE 19

// Motores
#define mEa 29
#define mEb 27
#define mDa 25
#define mDb 23
#define mEv 2
#define mDv 3

#define velE 255
#define velD 214

// Constantes físicas e parâmetros PID
const float perimetro = 10.68141502;
const float circunferencia = 54.35393641;
const float debugEnc = 0.988;

const float Kp = 60;
const float Ki = 15;
const float Kd = 10;
const float Tm = 0.02;

volatile long pulsosE = 0;
volatile long pulsosD = 0;

unsigned long primeiroMillis;

float erroE, erroE1 = 0, erroE2 = 0;
float erroD, erroD1 = 0, erroD2 = 0;
float PWMD1 = velD, PWMD, PWME, PWME1 = velE;

float excesso = 0;

float distanciaE = 0, distanciaD = 0;

void ContadorEsq() { pulsosE++; }
void ContadorDir() { pulsosD++; }

void CalculaPID() {
  
  PWME = PWME1 + ( (Kp + Kd/Tm) * erroE + (-Kp +Ki*Tm -2*Kd/Tm) * erroE1 + (Kd/Tm) * erroE2 )/2;
  PWME1 = PWME;
  erroE2 = erroE1;
  erroE1 = erroE;
  
  PWMD = PWMD1 + ( (Kp + Kd/Tm) * erroD + (-Kp +Ki*Tm -2*Kd/Tm) * erroD1 + (Kd/Tm) * erroD2 )/2;
  PWMD1 = PWMD;
  erroD2 = erroD1;
  erroD1 = erroD;
}
         
void Para(unsigned long tempo){
  
     digitalWrite(mEa, LOW);
     digitalWrite(mEb, LOW);
     digitalWrite(mDa, LOW);
     digitalWrite(mDb, LOW);
     analogWrite(mEv, 0);
     analogWrite(mDv, 0);
     delay(tempo);
}

void Frente(float distancia){
     interrupts();
     digitalWrite(mEa, HIGH);
     digitalWrite(mEb, LOW);
     digitalWrite(mDa, HIGH);
     digitalWrite(mDb, LOW);
     analogWrite(mEv, velE);
     analogWrite(mDv, velD);
     delay(1);
     
  while(distanciaE < distancia){
    
  if ((millis() - primeiroMillis) >= 20 ){
  
    noInterrupts();
    distanciaE = distanciaE + (float)(perimetro * pulsosE * debugEnc)/ 1400;
    distanciaD = distanciaD + (float)(perimetro * pulsosD)/ 1400;
    interrupts();
    
    pulsosD = pulsosE = 0;        
    primeiroMillis = millis();   
  }
  
  if(distanciaD != distanciaE){
     
   erroE = distanciaD - distanciaE;
   erroD = distanciaE - distanciaD;
   
  //----------EQUAÇÃO PID---------//
    
  if(PWME1 > 255) PWME1 = 255;
  if(PWME1 < 0) PWME1 = 0;
    
  if(PWMD1 > 255) PWMD1 = 255;
  if(PWMD1 < 0) PWMD1 = 0;

  CalculaPID();
  
  //--------SATURAR PWM PD-------//
    
  if(PWME > 255) PWME = 255;
  if(PWME < 0) PWME = 0;
  
  if(PWMD > 255) PWMD = 255;
  if(PWMD < 0) PWMD = 0;
    
  //--------COMANDO MOTORES------//
   analogWrite(mEv, PWME);
   analogWrite(mDv, PWMD);
  }
 }
    digitalWrite(mEa, LOW);
    digitalWrite(mEb, HIGH);
    digitalWrite(mDa, LOW);
    digitalWrite(mDb, HIGH);
    analogWrite(mEv, velE);
    analogWrite(mDv, velD);
    delay(30);
   Para(100);
   noInterrupts();
   pulsosE = pulsosD = erroE = erroE1 = erroE2 = erroD = erroD1 = erroD2 = distanciaE = distanciaD = 0;
}

void EsquerdaEixo(float grau){
     interrupts();
     digitalWrite(mEa, LOW);
     digitalWrite(mEb, HIGH);
     digitalWrite(mDa, HIGH);
     digitalWrite(mDb, LOW);
     analogWrite(mEv, velE);
     analogWrite(mDv, velD);
     delay(1);
     
  float distancia = (float)((grau/360) * circunferencia) - 0.45;
  
while(distanciaE < distancia){
    
  if ((millis() - primeiroMillis) >= 20) {
  
    noInterrupts();
    distanciaE = distanciaE + (float)(perimetro * pulsosE * debugEnc)/ 1400;
    distanciaD = distanciaD + (float)(perimetro * pulsosD)/ 1400;
    interrupts();
    
    pulsosD = pulsosE = 0;        
    primeiroMillis = millis();   
  }
  
  if(distanciaD != distanciaE){
     
   erroE = distanciaD - distanciaE;
   erroD = distanciaE - distanciaD;
   
  //----------EQUAÇÃO PID---------//
    
  if(PWME1 > 255) PWME1 = 255;
  if(PWME1 < 100) PWME1 = 100;
    
  if(PWMD1 > 255) PWMD1 = 255;
  if(PWMD1 < 100) PWMD1 = 100;
    
  CalculaPID();
  
  //--------SATURAR PWM PD-------//
    
  if(PWME > 255){
    excesso = PWME - 255;
    PWME = 255;
    PWMD = PWMD - excesso;
  }
  if(PWME < 0) PWME = 0;
  
  if(PWMD > 255){
    excesso = PWMD - 255;
    PWMD = 255;
    PWME = PWME - excesso;
  }
  if(PWMD < 0) PWMD = 0;
    
  //--------COMANDO MOTORES------//
  
   analogWrite(mEv, PWME);
   analogWrite(mDv, PWMD);
  }
 }

    digitalWrite(mEa, HIGH);
    digitalWrite(mEb, LOW);
    digitalWrite(mDa, LOW);
    digitalWrite(mDb, HIGH);
    analogWrite(mEv, velE);
    analogWrite(mDv, velD);
    delay(30);
   Para(100);
   noInterrupts();
   pulsosE = pulsosD = erroE = erroE1 = erroE2 = erroD = erroD1 = erroD2 = distanciaE = distanciaD = 0;
}

void setup() {
  
   // Motores.
   pinMode(mEa, OUTPUT);
   pinMode(mEb, OUTPUT);
   pinMode(mDa, OUTPUT);
   pinMode(mDb, OUTPUT);
   pinMode(mEv, OUTPUT);
   pinMode(mDv, OUTPUT);
   pinMode(49, OUTPUT);
    
   digitalWrite(49, HIGH);
  
  // Encoder Esquerdo.
  pinMode(pinE, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP); 
  
  // Encoder Direito.
  pinMode(pinD, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);

  //Interrupções.
  Serial.begin(9600);
  attachInterrupt(5, ContadorDir, CHANGE);
  attachInterrupt(4, ContadorEsq, CHANGE);
  primeiroMillis = millis();
  
}

void loop() {
Frente(30);
EsquerdaEixo(90);
Para(1000);

  //------------DEBUG------------//

// Serial.print("Erro 1, 2, 3  " + String(erroE) + String(" ") + String(erroE1) + String(" "));
// Serial.print(erroE2 + String(" Resultado") + String(PWME));
// 
// Serial.print("   Erro 1, 2, 3  " + String(erroD) + String(" ") + String(erroD1) + String(" "));
// Serial.println(erroD2 + String(" Resultado") + String(PWMD)); 
// Serial.println("  Dist.D " + String(distanciaD));
  
}
