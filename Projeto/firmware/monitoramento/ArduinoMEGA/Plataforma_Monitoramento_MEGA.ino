// BIBLIOTECAS.
#include <Wire.h>               //Biblioteca auxiliar pH.
#include <OneWire.h>            //Biblioteca auxiliar temperatura.
#include <DallasTemperature.h>  //Biblioteca auxiliar temperatura.

// Declaração das portas.
#define pHPin A0             //pH.
#define turbidezPin A1       //Turbidez.
#define ONE_WIRE_BUS A2      //sinal do sensor DS18B20.
#define condutividadePin A3  //Condutividade.

// Outras coisas condutividade.
#define VREF 5.0   //Tensão de referência analógica (Volt) do ADC.
#define SCOUNT 30  //Soma dos pontos de amostra.

//Declaração cálculos de turbidez.
double calc_NTU(double volt) {

  double NTU_val;
  NTU_val = ((-1120.4 * volt * volt) + (5742.3 * volt) - 4352.9) / 1000;  //((-1120.4*volt*volt)+(5742.3*volt)-4352.9)/1000;
  return NTU_val;                                                         // ((-739.464*volt*volt)+(3789.918*volt)-2872.914)/1000;
}

// Variáveis:

// Variáveis Auxiliares.
bool flag = 1, corrigecondut = 1;
unsigned long tempo = 0;

// Variáveis pH.
int measure;
float T, Po;

// Variável turbidez.
double NTU = 0.0;

//Declaração de objetos, (temperatura):
OneWire oneWire(ONE_WIRE_BUS);  //objeto one_wire
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1;

//Variáveis temperatura:
float TempC;

//Váriaveis condutividade:
int analogBuffer[SCOUNT];  //Armazena o valor analógico lido do ADC na matriz.
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
int copyIndex = 0;

float averageVoltage = 0;
float tdsValue = 0;

//Algoritmo de filtragem média, (condutividade):
int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++) {
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
  
  if ((iFilterLen & 1) > 0) {
    bTemp = bTab[(iFilterLen - 1) / 2];
  } else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}

void setup() {
  Serial.begin(115200);             //Inicializa a conexão com o ESP32.
  pinMode(pHPin, INPUT);            // pH.
  pinMode(turbidezPin, INPUT);      //Turbidez.
  pinMode(ONE_WIRE_BUS, INPUT);     //Temperatura.
  sensors.begin();                  //Temperatura.
  sensors.getAddress(sensor1, 0);   //Temperatura.
  pinMode(condutividadePin, INPUT); //Condutividade.
}

//Funções:

void pH() {

  float calibration = 22;
  unsigned long int avgValue = 0;
  int buf[10], temp;

  for (byte i = 0; i < 10; i++) {
    buf[i] = analogRead(pHPin);
  }

  for (byte i = 0; i < 9; i++) {
    for (byte j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  for (int i = 2; i < 8; i++) avgValue += buf[i];
  float pHVol = (float)avgValue * 5.0 / 1024 / 6;
  Po = -5.70 * pHVol + calibration;
  Serial.print("pH = ");
  Serial.println(Po);
}


void Turbidez() {

  float tensao = analogRead(turbidezPin) * (float) (5/1024);

  NTU = calc_NTU(tensao);

  Serial.print("A turbidez da água é: ");
  Serial.println(NTU);
}

void Temperatura() {
  // Le a informacao do sensor
  sensors.requestTemperatures();
  TempC = sensors.getTempC(sensor1);

  // Mostra dados no serial monitor
  Serial.print("A temperatura é: ");
  Serial.println(TempC);
}

void Condutividade() {

  analogBuffer[analogBufferIndex] = analogRead(condutividadePin);  //Lê o valor analógico e armazena no buffer.
  analogBufferIndex++;
  if (analogBufferIndex == SCOUNT) {
    analogBufferIndex = 0;
  }

  if (flag) {
    flag = 0;
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++) {
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
    }
  }
  //Lê o valor analógico mais estável pelo algoritmo de filtragem media e converte para o valor de tensão.
  averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 1024.0;

  //Fórmula de compensação dA temperatura: ResultadofFinal(25^C) = ResultadofFinal(atual)/(1,0+0,02*(fTP-25,0));
  float compensationCoefficient = 1.0 + 0.02 * (TempC - 25.0);
  // Compensação de Temperatura.
  float compensationVoltage = averageVoltage / compensationCoefficient;

  //Converte o valor da tensão em valor TDS.
  tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;

  Serial.print("A condutividade é: ");
  Serial.print(tdsValue, 0);
  Serial.println("ppm");
}

//Envia as informações lidas para o ESP32.
void conexao(float ph, float turbidez, float temperatura, float condutividade) {
  String mensagem = "ph=" + String(ph) + "&turbidez=" + String(turbidez) + "&temperatura=" + String(temperatura) + "&condutividade=" + String(condutividade);
  Serial.print(mensagem);
  Serial.println();
}

void loop() {
  tempo = millis();
  while (millis() - tempo < 1000) {
    pH();
    Turbidez();
    Temperatura();
    Condutividade();
  }
  conexao(Po, NTU, TempC, tdsValue);
  flag = 1;
}
