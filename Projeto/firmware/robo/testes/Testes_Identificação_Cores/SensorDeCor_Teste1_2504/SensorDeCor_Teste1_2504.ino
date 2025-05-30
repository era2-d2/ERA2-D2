// Bibliotecas.
#include <EEPROM.h>

// Sensor.
const byte s3 = A7;

int VERMELHO;
int PRETO;

// Mínimos e Máximos de leitura da cor vermelha.
int vermelhoMin = 4 * EEPROM.read(5);
int vermelhoMax = 4 * EEPROM.read(6);

void Ler_Sensores() {

  if ((analogRead(s3) > vermelhoMin)&&(analogRead(s3) < vermelhoMax)) {
    VERMELHO = 1;
  }   
  else { 
    VERMELHO = 0;
  }

  if (analogRead(s3) > vermelhoMax){
    PRETO = 1;
  }
  else {
    PRETO = 0;
  }
  
}

void setup() {

  // Sensor.
  pinMode(s3, INPUT);
  Serial.begin(9600);

}

void loop() {

    Ler_Sensores(); 
    Serial.print("VERMELHO: ");
    Serial.print(VERMELHO);
    Serial.print("     ");
    Serial.print("PRETO: ");
    Serial.print(PRETO);
    Serial.println();
}
