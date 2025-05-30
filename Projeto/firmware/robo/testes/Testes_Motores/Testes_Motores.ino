// Motores.
#include <EEPROM.h>
#include <HCSR04.h>

#define mEa 29
#define mEb 27
#define mDa 25
#define mDb 23
#define mEv 2
#define mDv 3

#define velE 255 //248 frente
#define velD 255 //255 frente

unsigned long tempo = 0, tempo1, tempo2;
byte cont = 0;

// Ultrassônico. 
#define trig2 10
#define echo2 9

UltraSonicDistanceSensor us2(trig2, echo2);

unsigned long distancia2;

void DireitaEixo() {
  
  digitalWrite(mEa, HIGH);
  digitalWrite(mEb, LOW);
  analogWrite(mEv, velE);
  digitalWrite(mDa, LOW);
  digitalWrite(mDb, HIGH);
  analogWrite(mDv, velD);
  
}

void EsquerdaEixo() {
  
  digitalWrite(mEa, LOW);
  digitalWrite(mEb, HIGH);
  analogWrite(mEv, velE);
  digitalWrite(mDa, HIGH);
  digitalWrite(mDb, LOW);
  analogWrite(mDv, velD);
}

void Frente(){
  
  digitalWrite(mEa, HIGH);
  digitalWrite(mEb, LOW);
  analogWrite(mEv, velE);
  digitalWrite(mDa, HIGH);
  digitalWrite(mDb, LOW);
  analogWrite(mDv, velD);
    
}

void Para(){
  
  digitalWrite(mEa, HIGH);
  digitalWrite(mEb, LOW);
  analogWrite(mEv, 0);
  digitalWrite(mDa, HIGH);
  digitalWrite(mDb, LOW);
  analogWrite(mDv, 0);
    
}
void Printa(){
  
 Serial.print(EEPROM.read(10));
 Serial.print(EEPROM.read(11));
 Serial.print(EEPROM.read(12));
 Serial.print(EEPROM.read(13));
 delay(5000000000000000);
}

void Constante(){
 
  delay(10000);
  DireitaEixo(); 
  tempo1 = millis();
  delay(1000);
  distancia2 = 25; 
  while (distancia2 >= 5.05) distancia2 = (us2.measureDistanceCm()+us2.measureDistanceCm())*5;
  
  tempo2 = millis();
  Para();
  tempo = tempo + (tempo2-tempo1);
  cont++;
  
  if(cont == 10) {
    tempo = tempo/10;
    EEPROM.write(10,tempo/1000);
    EEPROM.write(11,(tempo%1000)/100);
    EEPROM.write(12,(tempo%100)/10);
    EEPROM.write(13,(tempo%10));
    while(1);
  }
}

void setup() {
  // Motores.
  pinMode(mEa, OUTPUT);
  pinMode(mEb, OUTPUT);
  pinMode(mDa, OUTPUT);
  pinMode(mDb, OUTPUT);
  pinMode(mEv, OUTPUT);
  pinMode(mDv, OUTPUT);
  Serial.begin(9600);
 
}

void loop() {
  //2645 ms frente
  //2920 ms direita eixo 
  //3010.5 ms esquerda eixo
  Frente();
  delay(1000000000000000000000000000000000000000000000000000000000000000000000000);
 }
