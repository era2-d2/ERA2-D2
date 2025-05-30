// Bibliotecas.
#include <Wire.h>               // Biblioteca auxiliar para o protocolo de comunicação I2C.
#include <OneWire.h>            // Biblioteca auxiliar para o sensor de temperatura.
#include <DallasTemperature.h>  // Biblioteca auxiliar para o sensor de temperatura.

// Declaração dos pinos.
#define pHPin A0             // pH.
#define turbidezPin A1       // Turbidez.
#define temperaturaPin A2    // Sinal do sensor DS18B20 (Temperatura).
#define condutividadePin A3  // Condutividade.

//----------------------------------------------------------------------------------OBJETOS----------------------------------------------------------------------------------//
// Temperatura.
OneWire oneWire(temperaturaPin); 
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1;

//---------------------------------------------------------------------------------CONSTANTES--------------------------------------------------------------------------------//
#define SCOUNT 30  // Tamanho do vetor de soma.
const double leituraAnalogica = ((double)5.0 / 1024.0); // 0,488 volts por valor.

//---------------------------------------------------------------------------------VARIÁVEIS---------------------------------------------------------------------------------// 
// Variáveis Auxiliares.
bool flag = 1, corrigecondut = 1;
unsigned long tempo = 0;

// Variáveis dos sensores.
double pH = 0;
double NTU = 0;
float TEMP = 0;

int analogBuffer[SCOUNT];     // Armazena os valores analógicos lidos do sensor.
int analogBufferTemp[SCOUNT]; // Cópia temporária do buffer para processamento.
int analogBufferIndex = 0;    // Índice do buffer.
int copyIndex = 0;

double tensaoEstavel = 0;
double TDS = 0;

//--------------------------------------------------------------------------------ALGORÍTIMOS--------------------------------------------------------------------------------//
// Filtragem média do sensor de condutividade.
int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (int i = 0; i < iFilterLen; i++) {
    bTab[i] = bArray[i];
  }
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
 
  if ((iFilterLen & 1) > 0) bTemp = bTab[(iFilterLen - 1) / 2];
  else bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  
  return bTemp;
}

//-----------------------------------------------------------------------------------FUNÇÕES--------------------------------------------------------------------------------//
void PH() {

  const float calibracao = 22.0;
  unsigned long somaLeiturasValidas = 0;
  unsigned int leituras[10];

  // Realiza 10 leituras do sensor.
  for (byte i = 0; i < 10; i++) {
    leituras[i] = analogRead(pHPin);
  }

  // Organiza os valores em ordem crescente.
  for (byte i = 0; i < 9; i++) {
    for (byte j = i + 1; j < 10; j++) {
      if (leituras[i] > leituras[j]) {
        int temp = leituras[i];
        leituras[i] = leituras[j];
        leituras[j] = temp;
      }
    }
  }

  for (byte i = 2; i < 8; i++) somaLeiturasValidas += leituras[i]; // Descarta as duas primeiras e duas últimas leituras e soma.
  double pHVol = somaLeiturasValidas * leituraAnalogica / 6; // Faz a média entre as 6 leituras restantes.
  pH = -5.70 * pHVol + calibracao;  // Retorna um valor de pH.
  pH = -1* pH;
  
}

void Turbidez() {

  double tensao = (analogRead(turbidezPin) * leituraAnalogica);  
  NTU = (double)((-1.1204 * tensao * tensao) + (5.7423 * tensao) -4.3529);  // Calculo NTU.
  NTU = -1* NTU;
}

void Temperatura() {
  
  sensors.requestTemperatures(); // Le a informação do sensor.
  TEMP = sensors.getTempC(sensor1);
}

void Condutividade() {

  analogBuffer[analogBufferIndex] = analogRead(condutividadePin);  //Lê o valor analógico e armazena no buffer.
  analogBufferIndex++;  
  
  if (analogBufferIndex == SCOUNT) analogBufferIndex = 0;

  if(flag) {
    flag = 0;
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++) {
       analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
    }
  }
  
  // Lê o valor analógico mais estável pelo algoritmo de filtragem média e converte para um valor de tensão.
  tensaoEstavel = getMedianNum(analogBufferTemp, SCOUNT) * leituraAnalogica;

  // Compensação do valor de condutividade em função da temperatura.
  float compensationCoefficient = 1.0 + 0.02 * (TEMP - 25.0); // ResultadofFinal(25^C) = ResultadofFinal(atual)/(1,0 + 0,02 * (temperatura - 25,0));
  float compensation = tensaoEstavel / compensationCoefficient;

  // Converte o valor da tensão em um valor de condutividade. O valor calculado é multiplicado por 0.5 para converter para TDS em ppm.
  TDS = ((133.42 * compensation * compensation * compensation) - 255.86 * compensation * compensation + 857.39 * compensation) * 0.5;
}

void Conexao(double pH = 0, double turbidez = 0, float temperatura = 0, double condutividade = 0) {
  
  // Envia as informações lidas para o ESP8266 01.
  String mensagem = String(pH) + "/" + String(turbidez) + "/" + String(temperatura) + "/" + String(condutividade);
  Serial.print(mensagem);
}

//-----------------------------------------------------------------------------------SETUP-----------------------------------------------------------------------------------//
void setup() {
  
  // Inicializa o Monitor Serial para a conexão com o ESP8266 01.
  Serial.begin(115200);      
  while(!Serial);       

  // Define os pinos como entrada.
  pinMode(pHPin, INPUT);            // pH.
  pinMode(turbidezPin, INPUT);      //Turbidez.
  pinMode(temperaturaPin, INPUT);   //Temperatura.
  pinMode(condutividadePin, INPUT); //Condutividade.

  // Inicializações do sensor de temperatura.
  sensors.begin();                  //Temperatura.
  sensors.getAddress(sensor1, 0);   //Temperatura.  
}

//------------------------------------------------------------------------------------LOOP-----------------------------------------------------------------------------------//
void loop() {
  // Lê os dados continuamente e envia a cada 1 segundo.
  tempo = millis();
  while (millis() - tempo < 1000) { 
    PH();
    Turbidez();
    Temperatura();
    Condutividade();
  }
  Conexao(pH, NTU, TEMP, TDS);
  flag = 1;
}
