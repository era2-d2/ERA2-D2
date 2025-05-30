// Bibliotecas.
#include <VL53L0X.h>  // Sensor Laser.
#include <Wire.h>     // Protocolo I2C.
#include <Servo.h>    // Servomotores.

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
      delay(12);
    }
  } else if (ultimoEstadoX < x) {
    for (byte i = ultimoEstadoX; i <= x; i++) {
      servo1.write(i);
      delay(12);
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

  Garra(145, 70); // Posição inicial: Servo1 em 145º e Servo2 em 70º, garra aberta.
}

void LerArvore() { Garra(90, 110); } // Altura ideal para fazer a leitura das alturas das árvores. 
void ArvoreGrande() { Garra(50, 130); } // Altura ideal para pegar a árvore grande.
void ArvorePequena() { Garra(40, 130); } // Altura ideal para pegar a árvore pequena. 

//----------------------------------------------------------------------------------LASER--------------------------------------------------------------------------------//
float distanciaLaser; // Armazena a leitura de distância do Laser.

bool arvoreG; // Árvore Grande, verdadeira ou falsa. 

VL53L0X laser; // Declara o Laser 1.

// Função do Laser.
bool Laser() {

 for(byte i = 0; i < 3; i++){
  distanciaLaser = distanciaLaser + (laser.readRangeContinuousMillimeters() - 15); // 15 é o erro do sensor.
 }

 distanciaLaser = (float)(distanciaLaser / 4);
 
  if (distanciaLaser <= 120.0) arvoreG = true;
  else arvoreG = false;

  return arvoreG;
}

void setup() {

  // Servomotores.
  servo1.attach(5);
  servo2.attach(6);
  servo3.attach(7);
  Inicial(); // Desloca a garra para a posição inicial.

  // Laser.
  Wire.begin();
  laser.setTimeout(500);
  while (!laser.init());

  laser.startContinuous(); // Inicia o modo contínuo consecutivo (Lê o mais rápido possível).
  // Para usar o modo de tempo contínuo, forneça um período entre as medições em millissegundos "laser.startContinuous(100)".

  Serial.begin(9600);

}

void loop() {
  
  LerArvore(); // Garra

  if (Laser()) Serial.println("Arvore Grande" + String(distanciaLaser));                
  else Serial.println("Arvore Pequena");

  delay(1000);
}
