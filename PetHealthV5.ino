
// Librerias 
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include "MAX30105.h"
#include "heartRate.h"

// Vars Constantes
#define pinSMOV 2

// Clases / Objetos
MAX30105 SensorPulsos;
Adafruit_MLX90614 SensorTemp = Adafruit_MLX90614();

// Vars Librerias
const byte RATE_SIZE = 6; // Tamaño del Promedio de Pulsos
byte rates[RATE_SIZE]; // Array para Promediar los Pulsos
byte rateSpot = 0;
long lastBeat = 0; // Tiempo de Cuando Ocurrio el Ultimo Pulso
float beatsPerMinute; 
int beatAvg;

// Vars 
float temp = 0.0;
String mov = "";

void setup() {
  Serial.begin(9600);

  pinMode(pinSMOV, INPUT);

  // Iniciar Sensor de Temperatura con las Configuraciones por Defecto
  SensorTemp.begin(0x5A);
  
  if (!SensorPulsos.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 no encontrado");
    while (1);
  }

  SensorPulsos.setup(); // Iniciar Sensor de Pulsos con las Configuraciones por Defecto
  SensorPulsos.setPulseAmplitudeRed(0x0A); // Iniciar LED Rojo
  SensorPulsos.setPulseAmplitudeGreen(0); // Apagar LED Verde
  SensorPulsos.begin();
}

void loop() {
  long irValue = SensorPulsos.getIR();
  temp = SensorTemp.readObjectTempC();

  // Condicional -  Si se Detecta un Pulso
  if (checkForBeat(irValue) == true) {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute; //Almacenamiento de los Pulsos al Array
      rateSpot %= RATE_SIZE;

      // Promediar las Inputs del Sensor de Pulsos
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  // Condicional - Si se Detecta Movimiento
  if (digitalRead(pinSMOV) == HIGH){
    mov = "SI";
  }else{
    mov = "NO";
  }

  // Impresion de los Valores Separados en "|" para la 
  // Interpretacion del MIT-AppInventor
  Serial.print(temp);
  Serial.print("|");
  Serial.print(beatAvg);
  Serial.print("|");
  Serial.print(mov);
  
  Serial.println();
  delay(2000);
}

  // if (irValue < 50000) {
  //   Serial.print("el dedo no esta apoyado");
  // }


  // Serial.print("IR=");
  // Serial.print(irValue);
  // Serial.print(", BPM=");
  // Serial.print(beatsPerMinute);
  // Serial.print(", Avg BPM=");
  // Serial.print(beatAvg);
  // Serial.print(" TEMP:");
  // Serial.print(temp);


  // if (!mlx.begin(0x5A,1)) {
  //   Serial.println("Sensor MLX906 no encontrado");
  //   while (1);
  // };

