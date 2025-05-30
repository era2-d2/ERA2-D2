// sensor
const byte s1 = A15;
const byte s2 = A10;
const byte s3 = A5;
const byte s4 = A0;

int calibra1 = 700;
int calibra2 = 880;
int calibra3 = 900;
int calibra4 = 700;
const byte preto = 1;
const byte branco = 0;

int ler[5] = {0,0,0,0,0};

//motores
const byte m1a = 22;
const byte m1b = 23;
const byte m2a = 24;
const byte m2b = 25;
const byte m1v = 2;
const byte m2v = 3;

byte velocidadefrente = 90;
byte velocidadetras = 90;

void para(){
  digitalWrite(m1a, 0);
  digitalWrite(m1b, 0);
  digitalWrite(m2a, 0);
  digitalWrite(m2b, 0);
}

void frente(){
  digitalWrite(m1a, 1);
  digitalWrite(m1b, 0);
  analogWrite(m1v, velocidadefrente);
  digitalWrite(m2a, 1);
  digitalWrite(m2b, 0);
  analogWrite(m2v, velocidadefrente);
}

void tras(){
  digitalWrite(m1a, 0);
  digitalWrite(m1b, 1);
  analogWrite(m1v, velocidadetras);
  digitalWrite(m2a, 0);
  digitalWrite(m2b, 1);
  analogWrite(m2v, velocidadetras);
}

void esquerda(){
  digitalWrite(m1a, 0);
  digitalWrite(m1b, 1);
  analogWrite(m1v, 110);
  digitalWrite(m2a, 1);
  digitalWrite(m2b, 0);
  analogWrite(m2v, 150);
}

void direita(){
  digitalWrite(m1a, 1);
  digitalWrite(m1b, 0);
  analogWrite(m1v, 150);
  digitalWrite(m2a, 0);
  digitalWrite(m2b, 1);
  analogWrite(m2v, 110);
}

void lersensores(){
  
   if(analogRead(s1) > (calibra1)) {ler[1] = preto;}
  else{ler[1] = branco;}
  
   if(analogRead(s2) > (calibra2)) {ler[2] = preto;}
  else{ler[2] = branco;}
  
   if(analogRead(s3) > (calibra3)) {ler[3] = preto;}
  else{ler[3] = branco;}
  
   if(analogRead(s4) > (calibra4)) {ler[4] = preto;}
  else{ler[4] = branco;}
}

void setup() {
  //motores
  pinMode(m1a, OUTPUT);
  pinMode(m1b, OUTPUT);
  pinMode(m2a, OUTPUT);
  pinMode(m2b, OUTPUT);
  pinMode(m1v, OUTPUT);
  pinMode(m2v, OUTPUT);

  //sensor
  pinMode(s1, INPUT);
  pinMode(s2, INPUT);
  pinMode(s3, INPUT);
  pinMode(s4, INPUT);
  Serial.begin(9600);
 // frente();
  //delay(400);
}

void mostra() {
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
  lersensores();
  while(!((ler[1]==preto)&&(ler[2]==preto)&&(ler[3]==preto)&&(ler[4]==preto))){
    lersensores();
    if(ler[2]==preto)
    {
      esquerda();
    }
    if(ler[3]==preto)
    {
      direita();
    }
  }
  para();
}
