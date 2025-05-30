// Bibliotecas.
#include <VL53L0X.h>
#include <EEPROM.h>
#include <HCSR04.h>
#include <Servo.h>
#include <Wire.h>
//#include <StarWars.h>

// Motores.
#define mEa 29
#define mEb 27
#define mDa 25
#define mDb 23
#define mEv 2
#define mDv 3

// Constantes.
#define tempoDir 8.1111 // original 8.1111   //02-08: 7.73  // 07-08: 7.72  // 09-08: 8.36
#define tempoEsq 8.3625 // original 8.3625   //02-08: 8.005  // 07-08: 8.36  // 09-08: 7.95
#define tempoReto 0.01890359

#define velE 240
#define velD 255

#define porcentagemMotor 1.1
#define porcentagemEsquerda 1.03
#define porcentagemDireita 1.06
#define debug 1.02

// Sensor de cor.
#define sc A0
bool CORPRETA;
bool leituraCorPreta;
byte contagemDerrubados = 0;

// Mínimos e Máximos de leitura da cor vermelha.
int CORVERMELHAMin = 4 * EEPROM.read(5);
int CORVERMELHAMax = 4 * EEPROM.read(6);

// Calibração de cores.
#define PRETO 1
#define BRANCO 0

// Garra.
Servo servo1; // 60 a 140 graus  - graus, mais pra tras
Servo servo2; // 50 a 140 graus  - graus, mais alto
Servo servo3; // 180 a 90 graus  - graus, garra fechada

byte i = 0;

// Laser.
VL53L0X laser;
unsigned long distanciaLaser;

// Ultrassônico.
#define trig1 11
#define trig2 10
#define echo1 8
#define echo2 9

UltraSonicDistanceSensor us1(trig1, echo1); // (Trig, Echo)
UltraSonicDistanceSensor us2(trig2, echo2);

unsigned long distancia1;
unsigned long distancia2;

bool arvoreP = false;
bool arvoreG = false;

// Funções dos Motores.
void Para(int para)
{

    digitalWrite(mEa, LOW);
    digitalWrite(mEb, LOW);
    analogWrite(mEv, 0);
    digitalWrite(mDa, LOW);
    digitalWrite(mDb, LOW);
    analogWrite(mDv, 0);
    delay(para);
}

// Tempo.
float Distancia(float distancia)
{
    distancia = (float)(distancia * 1.0238 + 0.486);
    float tempo = (float)(distancia / tempoReto);
    return tempo;
}

void Direcao(float tempo, bool frente = true)
{

    bool a = frente ? true : false;
    bool b = frente ? false : true;

    digitalWrite(mEa, a);
    digitalWrite(mEb, b);
    analogWrite(mEv, velE);

    digitalWrite(mDa, a);
    digitalWrite(mDb, b);
    analogWrite(mDv, velD);
    delay(tempo * porcentagemMotor);
    Para(100);
}

void Gira(float angulo, bool esquerda = true)
{

    bool a = esquerda ? false : true;
    bool b = esquerda ? true : false;
    float tempo = esquerda ? (angulo * tempoEsq * porcentagemEsquerda) : (angulo * tempoDir * porcentagemDireita);

    digitalWrite(mEa, a);
    digitalWrite(mEb, b);
    analogWrite(mEv, velE);

    digitalWrite(mDa, b);
    digitalWrite(mDb, a);
    analogWrite(mDv, velD);
    delay(tempo);
    Para(100);
}

// Funções Sensor de Cor.
void SensorCor()
{
    if (analogRead(sc) > CORVERMELHAMax)
    {
        CORPRETA = 1;
    }
    else
    {
        CORPRETA = 0;
    }
}

void SensorCorAux()
{
    if (analogRead(sc) > CORVERMELHAMax)
    {
        leituraCorPreta = 1;
    }
    else
    {
        leituraCorPreta = 0;
    }
}

// Garra.
void Inicial()
{
    int v1 = servo1.read();
    int v2 = servo2.read();

    // Ajusta o servo1 para 145º.
    if (v1 < 145)
    {
        for (int i = v1; i <= 145; i++)
        {
            servo1.write(i);
            delay(12);
        }
    }
    else if (v1 > 145)
    {
        for (int i = v1; i >= 145; i--)
        {
            servo1.write(i);
            delay(12);
        }
    }

    // Ajusta o servo2 para 70º.
    if (v2 < 70)
    {
        for (int i = v2; i <= 70; i++)
        {
            servo2.write(i);
            delay(12);
        }
    }
    else if (v2 > 70)
    {
        for (int i = v2; i >= 70; i--)
        {
            servo2.write(i);
            delay(12);
        }
    }

    servo3.write(180);
}

void ArvoreGrande()
{
    for (i = 145; i >= 45; i--)
    { // Servo1: 35 para a árvore pequena e 55 para a árvore grande.
        servo1.write(i);
        delay(12);
    }

    for (i = 70; i <= 130; i++)
    {
        servo2.write(i);
        delay(12);
    }
}

void ArvorePequena()
{
    for (i = 145; i >= 30; i--)
    { // Servo1: 35 para a árvore pequena e 55 para a árvore grande.
        servo1.write(i);
        delay(12);
    }

    for (i = 70; i <= 130; i++)
    {
        servo2.write(i);
        delay(12);
        // Não fique torto por favor, te amamos por isso ❤.
    }
}

// Laser.
bool Laser()
{
  distanciaLaser = laser.readRangeContinuousMillimeters() - 25; // 25 é o erro do sensor.
  distanciaLaser < 100 ? arvoreG = 1 : arvoreG = 0;
}

// Ultrassônico.
bool Ultrassonico()
{
    // distancia1 = us1.measureDistanceCm();
    distancia2 = us2.measureDistanceCm();

    if (distancia2 < 10.0)
    {
        arvoreG = true;
    }
    else
    {
        arvoreG = false;
    }
    return arvoreG;
}

// Missão da Cor.
void Turbo(int tempo)
{

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

void Rampa(int tempo)
{

    for (i = 145; i >= 48; i--)
    { // Servo1: 35 para a árvore pequena e 55 para a árvore grande.
        servo1.write(i);
        delay(12);
    }

    for (i = 70; i >= 65; i--)
    { // Servo1: 35 para a árvore pequena e 55 para a árvore grande.
        servo2.write(i);
        delay(12);
    }

    servo3.write(85);

    Turbo(tempo);

    tempo = 2000;

    for (i = 48; i >= 20; i--)
    { // Servo1: 35 para a árvore pequena e 55 para a árvore grande.
        servo1.write(i);
        delay(12);

        Turbo(tempo);
        break;
    }
}

void AbaixaRampa()
{

    for (i = 145; i >= 70; i--)
    {
        servo1.write(i);
        delay(12);
    }

    for (i = 70; i <= 180; i++)
    {
        servo2.write(i);
        delay(12);
    }

    for (i = 70; i >= 50; i--)
    {
        servo1.write(i);
        delay(12);
    }
}

void Cor()
{

    for (i = 30; i <= 60; i++)
    {
        servo1.write(i);
        delay(12);
    }
    servo2.write(65);
}

void setup()
{

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

    // StarWars Theme
    // ImperialMarch march(45, 108);

    // Laser.
//    Wire.begin();
//    
//    laser.setTimeout(500);
//    
//    if (!laser.init()) while (true);
//
//    // Inicia o modo contínuo consecutivo (Lê o mais rápido possível).
//    laser.startContinuous();
  
    //Para usar o modo de tempo contínuo, forneça um período entre as medições em millissegundos laser.startContinuous(100).
}

void loop()
{

    // Primeira parte.

    Gira(90 * debug);         // Esquerda
    Direcao(Distancia(53.5)); // Frente
    Gira(90);                 // Esquerda
    Direcao(Distancia(11.5)); // Frente

    if (Ultrassonico())
    {

        Direcao(Distancia(12), false); // Trás
        Gira(90, false);               // Direita
        Direcao(Distancia(6));         // Frente
        Gira(90);                      // Esquerda
        ArvoreGrande();                // ARVORE GRANDE.
        Direcao(Distancia(6.5));       // Frente

        for (i = 180; i >= 80; i--)
        {
            servo3.write(i);
            delay(12);
        }

        for (i = 45; i <= 110; i++)
        {
            servo1.write(i);
            delay(12);
        }

        for (i = 130; i >= 90; i--)
        {
            servo2.write(i);
            delay(12);
        }

        Direcao(Distancia(6), false); // Trás
        Gira(90, false);              // Direita
        Direcao(Distancia(5));        // Frente
        Gira(90, false);              // Direita
        Direcao(Distancia(5.5));      // Frente
        Gira(91);                     // Esquerda
        Direcao(Distancia(16));       // Frente

        for (i = 110; i <= 120; i++)
        {
            servo1.write(i);
            delay(12);
        }

        servo3.write(180);

        // Pegar próxima árvore.

        Direcao(Distancia(8), false); // Trás
        Gira(90);                     // Esquerda
        Direcao(Distancia(18));       // Frente
        Gira(20, false);              // Direita

        for (i = 90; i <= 135; i++)
        {
            servo2.write(i);
            delay(12);
        }

        for (i = 110; i >= 32; i--)
        {
            servo1.write(i);
            delay(12);
        }

        for (i = 180; i >= 80; i--)
        {
            servo3.write(i);
            delay(8);
        }

        for (i = 32; i <= 115; i++)
        {
            servo1.write(i);
            delay(12);
        }

        Direcao(Distancia(4), false); // Trás
        Gira(60, false);              // Direita
        Direcao(Distancia(16));       // Frente
        servo3.write(180);

        // Árvore pequena.
    }
    else
    {

        Direcao(Distancia(12), false); // Trás
        Gira(90, false);               // Direita
        Direcao(Distancia(6));         // Frente
        Gira(90);                      // Esquerda
        ArvorePequena();
        Direcao(Distancia(6)); // Frente

        for (i = 180; i >= 80; i--)
        {
            servo3.write(i);
            delay(12);
        }

        for (i = 45; i <= 100; i++)
        {
            servo1.write(i);
            delay(12);
        }

        servo1.write(100);

        Direcao(Distancia(4), false); // Trás
        Gira(90, false);              // Direita
        Direcao(Distancia(20));       // Frente

        for (i = 120; i <= 120; i++)
        {
            servo1.write(i);
            delay(12);
        }

        for (i = 130; i <= 130; i++)
        {
            servo2.write(i);
            delay(12);
        }

        servo3.write(180);

        // Pegar próxima árvore.
        Direcao(Distancia(8.5), false); // Trás
        Gira(60);                       // Esquerda

        for (i = 130; i >= 100; i--)
        {
            servo2.write(i);
            delay(12);
        }

        for (i = 120; i >= 45; i--)
        {
            servo1.write(i);
            delay(12);
        }

        Direcao(Distancia(8)); // Frente

        for (i = 180; i >= 80; i--)
        {
            servo3.write(i);
            delay(8);
        }

        for (i = 45; i <= 110; i++)
        {
            servo1.write(i);
            delay(12);
        }

        for (i = 100; i >= 55; i--)
        {
            servo2.write(i);
            delay(12);
        }

        Direcao(Distancia(9), false); // Trás
        Gira(60, false);              // Direita
        Direcao(Distancia(3));        // Frente
        Gira(90, false);              // Direita
        Direcao(Distancia(8));        // Frente
        Gira(90);                     // Esquerda
        Direcao(Distancia(14));       // Frente

        for (i = 60; i <= 110; i++)
        {
            servo2.write(i);
            delay(12);
        }

        for (i = 110; i >= 105; i--)
        {
            servo1.write(i);
            delay(12);
        }

        servo3.write(180);
        Direcao(Distancia(8), false);  // Trás
        Gira(89, false);               // Direita
        Direcao(Distancia(19), false); // Trás
        Gira(92);                      // Esquerda
    }

    Direcao(Distancia(62), false); // Trás
    // march.playMelody();
    delay(120000);

    // SEGUNDA MISSÃO

    Direcao(Distancia(34.2)); // Frente
    Gira(92, false);          // Direita
    Direcao(Distancia(20));   // Frente
    Rampa(650);
    Gira(3);                // Esquerda
    Direcao(Distancia(35)); // Frente

    Cor();
    SensorCor();

    for (i = 60; i <= 80; i++)
    {
        servo1.write(i);
        delay(10);
    }

    for (i = 65; i >= 55; i--)
    {
        servo2.write(i);
        delay(10);
    }

    for (i = 180; i >= 90; i--)
    {
        servo3.write(i);
        delay(5);
    }

    Direcao(Distancia(32), false); // Trás
    Gira(88.5, false);             // Direita
    Direcao(Distancia(32));        // Frente

    for (int y = 0; y < 4; y++)
    {

        SensorCorAux();

        if (leituraCorPreta == CORPRETA)
        {

            if (contagemDerrubados == 4)
            {
                break;
            }

            // Alteramos durante o ensaio
            Direcao(Distancia(10), false); // Trás
            Gira(35, false);               // Direita

            for (i = 80; i >= 40; i--)
            {
                servo1.write(i);
                delay(10);
            }

            Gira(40); // Esquerda
            contagemDerrubados++;

            for (i = 40; i <= 80; i++)
            {
                servo1.write(i);
                delay(10);
            }

            if (y == 3)
            {
                break;
            }
        }
        else
        {

            if (y == 3)
            {
                break;
            }
            Direcao(Distancia(10), false); // Trás
        }

        if (contagemDerrubados >= 4)
        {
            break;
        }

        Gira(94);                 // Esquerda
        Direcao(Distancia(25.5)); // Frente
        Gira(90, false);          // Direita
        Direcao(Distancia(11.2)); // Frente
    }

    if (contagemDerrubados < 4)
    {

        if (leituraCorPreta == CORPRETA)
        {
            Direcao(Distancia(18), false); // Trás
        }
        else
        {
            Direcao(Distancia(25), false); // Trás
        }

        Gira(89, false);        // Direita
        Direcao(Distancia(40)); // Frente

        // Outro lado (otherside).
        Direcao(Distancia(27), false); // Trás
        Gira(90, false);               // Direita
        Direcao(Distancia(33));        // Frente

        for (int y = 0; y < 4; y++)
        {

            if (contagemDerrubados == 4)
            {
                break;
            }

            SensorCorAux();

            if (leituraCorPreta == CORPRETA)
            {

                if (contagemDerrubados == 4)
                {
                    break;
                }

                Direcao(Distancia(9), false); // Trás
                Gira(40);                     // Esquerda

                for (i = 80; i >= 40; i--)
                {
                    servo1.write(i);
                    delay(10);
                }

                Gira(35, false); // Direita
                contagemDerrubados++;

                for (i = 40; i <= 80; i++)
                {
                    servo1.write(i);
                    delay(10);
                }

                if (y == 3)
                {
                    break;
                }
            }
            else
            {

                if (y == 3)
                {
                    break;
                }

                Direcao(Distancia(10), false); // Trás
            }

            if (contagemDerrubados >= 4)
            {
                break;
            }

            Gira(95);                 // Esquerda
            Direcao(Distancia(26.8)); // Frente
            Gira(89, false);          // Direita
            Direcao(Distancia(12));   // Frente
        }
    }
    else
    {
        // march.playMelody();
        delay(1000000);
    }
    // march.playMelody();
    delay(1000000);
}
