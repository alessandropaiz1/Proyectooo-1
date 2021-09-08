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


void configurarBinicio(void);
void EMAADC(void);
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
  pinMode(binicio, INPUT_PULLUP); // Definir entrada del botón de inicio

  
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
      
    }

}
//***************************************************************
// Función para configurar PWM (LEDs y motor servo)
//***************************************************************



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
