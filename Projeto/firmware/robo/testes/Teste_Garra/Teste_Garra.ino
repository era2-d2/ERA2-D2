#include <Servo.h>

Servo servo1; // 60 a 140 graus  - graus, mais pra tras                          
Servo servo2; // 60 a 140 graus  - graus, mais alto                               
Servo servo3; // 180 a 90 graus  - graus, garra fechada

int i = 0, j = 0, v1, v2;   

void TesteSimultaneo(){
  
    for(i = 140; i >= 55; i--){
        servo1.write(i);
        delay(20);
        for(j = 60 ; j <= 120;){      
         servo2.write(j);
         delay(20);
          j++;
         break;       
    }
  }
}

void Inicial() {

    v1 = servo1.read();
    v2 = servo2.read();
    
  if(v1 <= 145){
    for(i = v1; i <= 145; i++){
  servo1.write(i);
  delay(20);
  }                                           // Servo1 145 graus
}
   if(v1 > 145){
     for(i = v1; i > 145; i--){
  servo1.write(i);
  delay(20);
    
  }
}

  if(v2 <= 80){
    for(i = v2; i <= 80; i++){
  servo2.write(i);
  delay(20);
  }                                          // Servo2 80
}
   if(v2 > 80){
     for(i = v2; i > 80; i--){
  servo2.write(i);
  delay(20);
    
  }
}

  servo3.write(180);
}

void setup() {
  servo1.attach(5);  
  servo2.attach(6);
  servo3.attach(7);
 // Inicial();
  delay (500);
}

void loop() {  
  
 for(i = 145; i >= 55; i--){ // Servo1: 35 para a árvore pequena e 55 para a árvore grande.
  servo1.write(i);
  delay(20);
 }

 
 for(i = 60; i <= 110; i++){
  servo2.write(i);
  delay(20);
 }
 
 for(i = 180; i >= 90; i--){
  servo3.write(i);
  delay(20);
 }

 for(i = 55; i<= 100; i++){
  servo1.write(i);
  delay(20);
 }
 
}
