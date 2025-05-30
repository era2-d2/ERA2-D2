   // Bibliotecas.
   #include <EEPROM.h>
   #include <HCSR04.h>
   #include <Servo.h>
   #include <StarWars.h>

   // Motores.
   #define mEa 29
   #define mEb 27
   #define mDa 25
   #define mDb 23
   #define mEv 2
   #define mDv 3

   // Constantes.
   #define tempoDir 8.1111 //original 8.1111   //02-08: 7.73  // 07-08: 7.72  // 09-08: 8.36 
   #define tempoEsq 8.3625 //original 8.3625   //02-08: 8.005  // 07-08: 8.36  // 09-08: 7.95 
   #define tempoReto 0.01890359

   #define velE 247
   #define velD 250

   #define porcentagemMotor 1.1
   #define porcentagemEsquerda 1.03
   #define porcentagemDireita 1.1
   #define debug 1.02

   // Sensor de cor.
   #define sc A0
   bool CORPRETA;
   bool leituraCorPreta;
   byte contagemDerrubados = 0;

   // Mínimos e Máximos de leitura da cor vermelha.
   int CORVERMELHAMin = 4 * EEPROM.read(5);
   int CORVERMELHAMax = 4 * EEPROM.read(6);

   // Garra.
   Servo servo1; // 60 a 140 graus. - graus, mais pra tras.                          
   Servo servo2; // 50 a 140 graus. - graus, mais alto.                               
   Servo servo3; // 180 a 90 graus. - graus, garra fechada.

   byte i = 0;

   // Ultrassônico.
   #define trig1 11
   #define trig2 10
   #define echo1 8
   #define echo2 9

   UltraSonicDistanceSensor us1(trig1, echo1); // (Trig, Echo)
   UltraSonicDistanceSensor us2(trig2, echo2);

   unsigned long distancia1;
   unsigned long distancia2;

   bool arvoreG = false;

   // Funções dos Motores.
   void Para(float para) {

     digitalWrite(mEa, LOW);
     digitalWrite(mEb, LOW);
     analogWrite(mEv, 0);
     digitalWrite(mDa, LOW);
     digitalWrite(mDb, LOW);
     analogWrite(mDv, 0);
     delay(para);
   }
   
   void Frente(float tempo) {

     digitalWrite(mEa, HIGH);
     digitalWrite(mEb, LOW);
     analogWrite(mEv, velE);
     digitalWrite(mDa, HIGH);
     digitalWrite(mDb, LOW);
     analogWrite(mDv, velD);
     delay(tempo * porcentagemMotor);
     Para(100);
   }

   void Tras(float tempo) {

     digitalWrite(mEa, LOW);
     digitalWrite(mEb, HIGH);
     analogWrite(mEv, velE);
     digitalWrite(mDa, LOW);
     digitalWrite(mDb, HIGH);
     analogWrite(mDv, velD);
     delay(tempo * porcentagemMotor);
     Para(100);
   }

   void DireitaEixo(float angulo) {
    
     float t = angulo * tempoDir;
     digitalWrite(mEa, HIGH);
     digitalWrite(mEb, LOW);
     analogWrite(mEv, 255);
     digitalWrite(mDa, LOW);
     digitalWrite(mDb, HIGH);
     analogWrite(mDv, 255);
     delay(t * porcentagemDireita);
     Para(100);

   }

   void EsquerdaEixo(float angulo) {

     float t = angulo * tempoEsq;
     digitalWrite(mEa, LOW);
     digitalWrite(mEb, HIGH);
     analogWrite(mEv, 255);
     digitalWrite(mDa, HIGH);
     digitalWrite(mDb, LOW);
     analogWrite(mDv, 255);
     delay(t * porcentagemEsquerda);
     Para(100);

   }

   // Funções Sensor de Cor.
   void SensorCor() {
     if (analogRead(sc) > CORVERMELHAMax) {
       CORPRETA = 1;
     } else {
       CORPRETA = 0;
     }
   }

   void SensorCorAux() {
     if (analogRead(sc) > CORVERMELHAMax) {
       leituraCorPreta = 1;
     } else {
       leituraCorPreta = 0;
     }
   }

   // Garra.
   void Inicial() {
     int v1 = servo1.read();
     int v2 = servo2.read();

     // Ajusta o servo1 para 145º.
     if (v1 < 145) {
       for (int i = v1; i <= 145; i++) {
         servo1.write(i);
         delay(12);
       }
     } else if (v1 > 145) {
       for (int i = v1; i >= 145; i--) {
         servo1.write(i);
         delay(12);
       }
     }

     // Ajusta o servo2 para 70º.
     if (v2 < 70) {
       for (int i = v2; i <= 70; i++) {
         servo2.write(i);
         delay(12);
       }
     } else if (v2 > 70) {
       for (int i = v2; i >= 70; i--) {
         servo2.write(i);
         delay(12);
       }
     }

     servo3.write(180);
   }

   void ArvoreGrande() {
     for (i = 145; i >= 45; i--) { // Servo1: 35 para a árvore pequena e 55 para a árvore grande.
       servo1.write(i);
       delay(12);
     }

     for (i = 70; i <= 130; i++) { 
       servo2.write(i);
       delay(12);
     }

   }

   void ArvorePequena() {
     for (i = 145; i >= 30; i--) { // Servo1: 35 para a árvore pequena e 55 para a árvore grande.
       servo1.write(i);
       delay(12);
     }

     for (i = 70; i <= 130; i++) { 
       servo2.write(i);
       delay(12);
       // Não fique torto por favor, te amamos por isso ❤.
     }
   }

   // Ultrassônico.
   bool Ultrassonico() {
     //distancia1 = us1.measureDistanceCm();
     distancia2 = us2.measureDistanceCm();

     if (distancia2 < 10.0) {
       arvoreG = true;
     } else {
       arvoreG = false;
     }
     return arvoreG;
   }

   // Missão da Cor.
   void Turbo(float tempo){

     digitalWrite(4, LOW);
     digitalWrite(mEa, HIGH);
     digitalWrite(mEb, LOW);
     analogWrite(mEv, velE);
     digitalWrite(mDa, HIGH);
     digitalWrite(mDb, LOW);
     analogWrite(mDv, velD);     
     delay(tempo);
     digitalWrite(4, HIGH);
}

   void Rampa(float tempo) {

     for (i = 145; i >= 48; i--) { // Servo1: 35 para a árvore pequena e 55 para a árvore grande.
       servo1.write(i);
       delay(12);
     }

     for (i = 70; i >= 65; i--) { // Servo1: 35 para a árvore pequena e 55 para a árvore grande.
       servo2.write(i);
       delay(12);
     }

     servo3.write(85);

     Turbo(tempo);

     tempo = 2000;
     
      for (i = 48; i >= 20; i--) { // Servo1: 35 para a árvore pequena e 55 para a árvore grande.
       servo1.write(i);
       delay(12);
       
     Turbo(tempo);
     break;       
     }
   }
   
void AbaixaRampa(){

     for (i = 145; i >= 70; i--) {
          servo1.write(i);
          delay(12);
        }

     for (i = 70; i <= 180; i++) {
          servo2.write(i);
          delay(12);
        }

     for (i = 70; i >= 50; i--) {
          servo1.write(i);
          delay(12);
        }

}

   void Cor() {

     for (i = 30; i <= 60; i++) { 
       servo1.write(i);
       delay(12);
     }
     servo2.write(65);
   }

   // Tempo.
   float Distancia(double distancia) {
     distancia = (float)(distancia * 1.0238 + 0.486);
     float tempo = (float)(distancia / tempoReto);
     return tempo;
   }

   // StarWars Theme.
   ImperialMarch march(45, 108);

   void setup() {
     
     // Motores.
     pinMode(mEa, OUTPUT);
     pinMode(mEb, OUTPUT);
     pinMode(mDa, OUTPUT);
     pinMode(mDb, OUTPUT);
     pinMode(mEv, OUTPUT);
     pinMode(mDv, OUTPUT);
     pinMode(4, OUTPUT);
     
     // Sensor de Cor.
     pinMode(sc, INPUT);

     // Servomotores.
     servo1.attach(5);
     servo2.attach(6);
     servo3.attach(7);

     // Ultrassônicos.
     pinMode(trig1, OUTPUT);
     pinMode(trig2, OUTPUT);
     pinMode(echo1, INPUT);
     pinMode(echo2, INPUT);

     // Inicializações.
     digitalWrite(4, HIGH);
     Inicial();
   }
   

   void loop() {

       // Primeira parte.
       EsquerdaEixo(90*debug);
       Frente(Distancia(53.5));
       EsquerdaEixo(90);
       Frente(Distancia(11.5));

       if (Ultrassonico()) {

         Tras(Distancia(12));
         DireitaEixo(90);
         Frente(Distancia(6));
         EsquerdaEixo(90);
         ArvoreGrande(); // ARVORE GRANDE.
         Frente(Distancia(6.5));

         for (i = 180; i >= 80; i--) {
           servo3.write(i);
           delay(12);
         }

         for (i = 45; i <= 110; i++) {
           servo1.write(i);
           delay(12);
         }

         for (i = 130; i >= 90; i--) {
           servo2.write(i);
           delay(12);
         }

         Tras(Distancia(6));
         DireitaEixo(90);
         Frente(Distancia(5));
         DireitaEixo(90);
         Frente(Distancia(5.5));
         EsquerdaEixo(91);
         Frente(Distancia(16));

         for (i = 110; i <= 120; i++) {
           servo1.write(i);
           delay(12);
         }

         servo3.write(180);

         // Pegar proxima árvore.
         
         Tras(Distancia(8));
         EsquerdaEixo(90);
         Frente(Distancia(18));
         DireitaEixo(20);

         for (i = 90; i <= 135; i++) {
           servo2.write(i);
           delay(12);
         }

         for (i = 110; i >= 32; i--) {
           servo1.write(i);
           delay(12);
         }
 
         for (i = 180; i >= 80; i--) {
           servo3.write(i);
           delay(8);
         }

         for (i = 32; i <= 115; i++) {
           servo1.write(i);
           delay(12);
         }

         Tras(Distancia(4));
         DireitaEixo(60);
         Frente(Distancia(16));
         servo3.write(180);

                                                                            //Árvore pequena.
         

       } else {

         Tras(Distancia(12)); //12
         DireitaEixo(90);
         Frente(Distancia(6));
         EsquerdaEixo(90);
         ArvorePequena();
         Frente(Distancia(6));
        
         for (i = 180; i >= 80; i--) {
           servo3.write(i);
           delay(12);
         }
         
         for (i = 45; i <= 100; i++) {
           servo1.write(i);
           delay(12);
         }
         
         servo1.write(100);

         Tras(Distancia(4));
         DireitaEixo(90); 
         Frente(Distancia(20));

         for (i = 120; i <= 120; i++) {
           servo1.write(i);
           delay(12);
         }

         for (i = 130; i <= 130; i++) {
           servo2.write(i);
           delay(12);
         }

         servo3.write(180);

         // Pegar próxima árvore.  
         Tras(Distancia(8.5));
         EsquerdaEixo(60);

         for (i = 130; i >= 100; i--) {
           servo2.write(i);
           delay(12);
         }

         for (i = 120; i >= 45; i--) {
           servo1.write(i);
           delay(12);
         }

         Frente(Distancia(8));

         for (i = 180; i >= 80; i--) {
           servo3.write(i);
           delay(8);
         }

         for (i = 45; i <= 110; i++) {
           servo1.write(i);
           delay(12);
         }

         for (i = 100; i >= 55; i--) {
           servo2.write(i);
           delay(12);
         }

         Tras(Distancia(9));
         DireitaEixo(60);
         Frente(Distancia(3));
         DireitaEixo(90);
         Frente(Distancia(8));
         EsquerdaEixo(90);
         Frente(Distancia(14));

         for (i = 60; i <= 110; i++) {
           servo2.write(i);
           delay(12);
         }
         
         for (i = 110; i >= 105; i--) {
           servo1.write(i);
           delay(12);
         }

         servo3.write(180);
         Tras(Distancia(8));
         DireitaEixo(89);
         Tras(Distancia(19));
         EsquerdaEixo(92);
         
       }
      
       Tras(Distancia(62));
       delay(12000);   

                                               
                                                                                  // SEGUNDA MISSÃO
     Frente(Distancia(33));
     DireitaEixo(92);
     Frente(Distancia(20));
     Rampa(650);
     EsquerdaEixo(3);
     Frente(Distancia(35));

     Cor();    
     SensorCor();

     for (i = 60; i <= 80; i++) {
       servo1.write(i);
       delay(10);
     }

     for (i = 65; i >= 55; i--) {
       servo2.write(i);
       delay(10);
     }

     for (i = 180; i >= 90; i--) {
       servo3.write(i);
       delay(5);
     }

     Tras(Distancia(32));
     DireitaEixo(88.5);
     Frente(Distancia(32));
     
     for (int y = 0; y < 4; y++) {

       SensorCorAux();
      
       if (leituraCorPreta == CORPRETA) {
        
         if(contagemDerrubados == 4){
          break;
         }

         // Alteramos durante o ensaio
         Tras(Distancia(10));
         DireitaEixo(35);
         
         for (i = 80; i >= 40; i--) {
           servo1.write(i);
           delay(10);
         }

         EsquerdaEixo(40);
         contagemDerrubados++;
         
         for (i = 40; i <= 80; i++) {
           servo1.write(i);
           delay(10);
         }

         if (y == 3) {
          break;
         }

       } else {

         if (y == 3) {
          break;
         }
         Tras(Distancia(10));
       }

       if(contagemDerrubados >= 4) {
        break;
       }

       EsquerdaEixo(94);
       Frente(Distancia(25.5));
       DireitaEixo(90);
       Frente(Distancia(11.2));
     }
    
   if(contagemDerrubados < 4){

    if (leituraCorPreta == CORPRETA) {
       Tras(Distancia(18));
    } else {
       Tras(Distancia(25));
    }
    
     DireitaEixo(89);
     Frente(Distancia(40));

                                                                               //OUTRO LADO (otherside).
     Tras(Distancia(27));
     DireitaEixo(90);
     Frente(Distancia(33));

    for (int y = 0; y < 4; y++) {
      
     if(contagemDerrubados == 4){
        break;
       }
       
       SensorCorAux();
      
       if (leituraCorPreta == CORPRETA) {
        
       if(contagemDerrubados == 4){
        break;
       }
       
         Tras(Distancia(9));
         EsquerdaEixo(40);
         

         for (i = 80; i >= 40; i--) {
           servo1.write(i);
           delay(10);
         }
      
         DireitaEixo(35);
         contagemDerrubados++;
         
         for (i = 40; i <= 80; i++) {
           servo1.write(i);
           delay(10);
         }

         if (y == 3) {
          break;
         }

       } else {    

         if (y == 3) {
          break;
         }

         Tras(Distancia(10));
       }

       if(contagemDerrubados >= 4) {
        break;
       }
       
       EsquerdaEixo(95);
       Frente(Distancia(26.8));
       DireitaEixo(89);
       Frente(Distancia(12));
     }
   } 
   else {
     march.playMelody();
     delay(1000000);
   }  
     march.playMelody();
     delay(1000000);
   
}
