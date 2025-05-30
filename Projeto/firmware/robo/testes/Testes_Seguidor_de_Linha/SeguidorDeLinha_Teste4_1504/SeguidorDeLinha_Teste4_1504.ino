// Sensor
const byte s1 = A15;
const byte s2 = A10;
const byte s3 = A5;
const byte s4 = A0;

int calibra1 = 700;
int calibra2 = 864;
int calibra3 = 900;
int calibra4 = 770;

const bool PRETO = 1;
const bool BRANCO = 0;

int ler[5] = {0,0,0,0,0};

// Motores
const byte m1a = 22;
const byte m1b = 23;
const byte m2a = 24;
const byte m2b = 25;
const byte m1v = 2;
const byte m2v = 3;

byte velocidadefrente = 100;
byte velocidadetras = 90;

void Para(){
  digitalWrite(m1a, 0);
  digitalWrite(m1b, 0);
  digitalWrite(m2a, 0);
  digitalWrite(m2b, 0);
}

void Frente(){
  digitalWrite(m1a, 1);
  digitalWrite(m1b, 0);
  analogWrite(m1v, velocidadefrente);
  digitalWrite(m2a, 1);
  digitalWrite(m2b, 0);
  analogWrite(m2v, velocidadefrente);
}

void Tras(){
  digitalWrite(m1a, 0);
  digitalWrite(m1b, 1);
  analogWrite(m1v, velocidadetras);
  digitalWrite(m2a, 0);
  digitalWrite(m2b, 1);
  analogWrite(m2v, velocidadetras);
}

void Esquerda(){
  digitalWrite(m1a, 0);
  digitalWrite(m1b, 1);
  analogWrite(m1v, 80);
  digitalWrite(m2a, 1);
  digitalWrite(m2b, 0);
  analogWrite(m2v, 100);
}

void Direita(){
  digitalWrite(m1a, 1);
  digitalWrite(m1b, 0);
  analogWrite(m1v, 100);
  digitalWrite(m2a, 0);
  digitalWrite(m2b, 1);
  analogWrite(m2v, 80);
}

void Ler_Sensores(){
  
   if(analogRead(s1) > (calibra1)) {ler[1] = PRETO;}
  else{ler[1] = BRANCO;}
  
   if(analogRead(s2) > (calibra2)) {ler[2] = PRETO;}
  else{ler[2] = BRANCO;}
  
   if(analogRead(s3) > (calibra3)) {ler[3] = PRETO;}
  else{ler[3] = BRANCO;}
  
   if(analogRead(s4) > (calibra4)) {ler[4] = PRETO;}
  else{ler[4] = BRANCO;}
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
  pinMode(s3, INPUT);
  pinMode(s4, INPUT);
  Serial.begin(9600);
}

void Mostra() {
  Serial.print(analogRead(s1));
  Serial.print("  ");
  Serial.print(analogRead(s2));
  Serial.print("  ");
  Serial.print(analogRead(s3));
  Serial.print("  ");
  Serial.print(analogRead(s4));
  Serial.println("  ");
 }
  
void loop() {
     Ler_Sensores();
     Mostra();
    
  while(!((ler[1] == PRETO)&&(ler[2] == PRETO)&&(ler[3] == PRETO)&&(ler[4] == PRETO))){
     Ler_Sensores();
 
    if(ler[2] == PRETO){
      Ler_Sensores();
      Esquerda();
    }
  
    if(ler[3] == PRETO){
      Ler_Sensores();   
      Direita();
     }
  
  }
  Para();
}
