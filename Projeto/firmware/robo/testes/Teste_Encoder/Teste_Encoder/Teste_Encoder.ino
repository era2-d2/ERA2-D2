#include <EEPROM.h>

#define pinD 18
#define pinE 19

// Motores.
#define mEa 29
#define mEb 27
#define mDa 25
#define mDb 23
#define mEv 2
#define mDv 3

#define diametro 10.68141502

volatile long pulsosE = 0;
volatile long pulsosD = 0;

unsigned long primeiroMillis;

void ContadorEsq() {pulsosE++;}
void ContadorDir() {pulsosD++;}

void Frente() {

    digitalWrite(mEa, HIGH);
    digitalWrite(mEb, LOW);
    analogWrite(mEv, 255);
    digitalWrite(mDa, HIGH);
    digitalWrite(mDb, LOW);
    analogWrite(mDv, 214);
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
    
Serial.println(pulsosD);
// }  
//
//Serial.print("pulsosE ");
//Serial.print((EEPROM.read(81)* 255) + EEPROM.read(80));
//
//Serial.print(" pulsosD ");
//Serial.println((EEPROM.read(83) * 255)+ EEPROM.read(82));

}
