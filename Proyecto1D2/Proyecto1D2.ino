//***************************************************************
// Universidad del Valle de Guatemala
// Electrónica Digital 2
// Proyecto 1 - Sensor de Temperatura
// Nombre: Alessandro Paiz
// Carné: 19572
//***************************************************************


//***************************************************************
// Librerías
//***************************************************************

//***************************************************************
// Definición de pines
//***************************************************************


#define pinLM35 35 // Definir pin del sensor LM35

#define ledV 32 // Definir leds del semáforo de temperatura
#define ledA 33
#define ledR 27

#define pinPWM 5 // Definir pin para señal PWM

#define binicio 1 // Definir botón de inicio


//***************************************************************
// Prototipos de funciones
//***************************************************************

void configurarPWM(void);
void configurarLedVerde(void);
void configurarLedAmarilla(void);
void configurarLedRoja(void);
void configurarBinicio(void);
void EMAADC(void);
void semaforo(void);


//***************************************************************
// Variables globales
//***************************************************************

//Variables señal ADC
int adcRaw = 0;
double adcFiltrado = 0;
double alpha = 0.05;
float celsius = 0;

//Variables Semáforo y Servo
int estadobinicio = 0;

//***************************************************************
// ISR
//***************************************************************

void IRAM_ATTR ISRbinicio()    // Interrupción para botón de inicio
{ 
  estadobinicio = 1;           // Iniciar cuando es 1
}

//***************************************************************
// Configuración
//***************************************************************

void setup()
{

  pinMode(ledV, OUTPUT); // Salidas leds del semaforo
  pinMode(ledA, OUTPUT);
  pinMode(ledR, OUTPUT);

  pinMode(pinPWM, OUTPUT); // Salida del motor servo

  pinMode(binicio, INPUT_PULLUP); // Definir entrada del botón de inicio

  digitalWrite(ledV, 0); // Estado inicial leds del semaforo
  digitalWrite(ledA, 0);
  digitalWrite(ledR, 0);

  configurarPWM(); // Llamar funciones para configurar servo y leds
  configurarLedVerde();
  configurarLedAmarilla();
  configurarLedRoja();
  configurarBinicio();
  
}

//***************************************************************
// Loop principal
//***************************************************************

void loop()
{
  
    if (estadobinicio == 1)
    {
      EMAADC();                   // Función filtro EMMA para obtener temperatura en celsius
      estadobinicio = 0;          // Reiniciar el estaod del botón
      semaforo();                 // Función para activar leds y motor servo

    }

}

//***************************************************************
// Función para configurar PWM (LEDs y motor servo)
//***************************************************************

void configurarPWM(void)
{
  // Paso 1: Configurar el modulo PWM
  ledcSetup(0, 50, 8);

  // Paso 2: Seleccionar en que GPIO tendremos nuestra senal PWM
  ledcAttachPin(pinPWM, 0);
}

void configurarLedVerde(void)
{

  ledcSetup(1, 50, 8); // Configuración de led verde en canal 1
  ledcAttachPin(ledV, 1);
}

void configurarLedAmarilla(void)
{

  ledcSetup(2, 50, 8); // Configuración de led amarilla en canal 2
  ledcAttachPin(ledA, 2);
}

void configurarLedRoja(void)
{

  ledcSetup(3, 50, 8); // Configuración de led roja en canal 3
  ledcAttachPin(ledR, 3);
}

//***************************************************************
// Función para obtener y filtrar temperatura.
//***************************************************************

void EMAADC(void)
{
  adcRaw = analogReadMilliVolts(pinLM35);                        // Leer valores obtenidos por el LM35
  adcFiltrado = (alpha * adcRaw) + ((1.0 - alpha) * adcFiltrado);  //Filtrar en un filtro EMMA los valores de temperatura obtenidos
  celsius = (adcFiltrado / 10.0); 

  Serial.println(celsius);                                        // Imprimir el valor de temperatura en celsius
  delay(50);
  
}

//***************************************************************
// Función para configurar botón de inicio
//***************************************************************
void configurarBinicio(void)
{

  attachInterrupt(binicio, ISRbinicio, HIGH); // Configurar interrupción del botón inicio
}

//***************************************************************
// Función de semáforo de temperatura
//***************************************************************

void semaforo(void)
{
  if (celsius < 37)
  {
    ledcWrite(0, 5);   // Primera posicion del servo en 0 grados
    ledcWrite(1, 190); // Solamente se enciende led verde
    ledcWrite(2, 0);
    ledcWrite(3, 0);
  }

  if (celsius >= 37 && celsius < 37.5)
  {
      ledcWrite(0, 15); // Primera posicion del servo en 0 grados
      ledcWrite(1, 0);
      ledcWrite(2, 190); // Solamente se enciende led amarilla
      ledcWrite(3, 0);
  }

  if (celsius >= 37.5)
  {
    ledcWrite(0, 25); // Primera posicion del servo en 0 grados
    ledcWrite(1, 0);
    ledcWrite(2, 0);
    ledcWrite(3, 190); // Solamente se enciende led rojo
  }
}
