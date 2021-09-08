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

#define sA 2    // Definir los pines para cada segmento del display
#define sB 4
#define sC 19
#define sD 17
#define sE 16
#define sF 0
#define sG 21
#define sdP 18

#define display1 14  // Definir los pines para cada display 
#define display2 12
#define display3 15

#define prescaler 80  // Prescaler para Timer

//***************************************************************
// Prototipos de funciones
//***************************************************************

void configurarPWM(void);
void configurarLedVerde(void);
void configurarLedAmarilla(void);
void configurarLedRoja(void);
void configurarBinicio(void);
void configurarDisplay(uint8_t A, uint8_t B, uint8_t C, uint8_t D, uint8_t E, uint8_t F, uint8_t G, uint8_t dP);
void desplegar7Seg(uint8_t digito); 
void desplegarPunto(boolean punto);
void EMAADC(void);
void semaforo(void);
void separarTemperatura(void);


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

//Variables para display
uint8_t pinA, pinB, pinC, pinD, pinE, pinF, pinG, pindP;
int decenas = 0;
int unidades = 0;
int decimal = 0;
int temperatura = 0;

//***************************************************************
// ISR
//***************************************************************

void IRAM_ATTR ISRbinicio()    // Interrupción para botón de inicio
{ 
  estadobinicio = 1;           // Iniciar cuando es 1
}

void IRAM_ATTR ISRTimer(){     // Interrupción para el Timer
  contadorTimer++;
  if(contadorTimer > 2){       // Reiniciar si es mayor a 2
    contadorTimer = 0;
  }
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

  pinMode(display1, OUTPUT); //Salidas de los displays
  pinMode(display2, OUTPUT);
  pinMode(display3, OUTPUT);

  pinMode(binicio, INPUT_PULLUP); // Definir entrada del botón de inicio

  digitalWrite(ledV, 0); // Estado inicial leds del semaforo
  digitalWrite(ledA, 0);
  digitalWrite(ledR, 0);

  digitalWrite(display1, 0); //Estado inicial de cada display
  digitalWrite(display1, 0);
  digitalWrite(display1, 0);

  configurarPWM(); // Llamar funciones para configurar servo y leds
  configurarLedVerde();
  configurarLedAmarilla();
  configurarLedRoja();
  configurarBinicio();
  configurarDisplay(sA, sB, sC, sD, sE, sF, sG, sdP); // Llamar función para configurar display de 7 segmentos
  
  configurarTimer();
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

      separarTemperatura();       // Función para separar los valores de temperatura
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
//***************************************************************
// Funcion para configurar display 7 segmentos
//***************************************************************

void configurarDisplay(uint8_t A, uint8_t B, uint8_t C, uint8_t D, uint8_t E, uint8_t F, uint8_t G, uint8_t dP)
//uint8_t A, uint8_t B, uint8_t C, uint8_t D, uint8_t E, uint8_t F, uint8_t G, uint8_t dP
{
  pinA = A;            // Volver a definir los pines para utilizarlos en proximas configuraciones
  pinB = B;
  pinC = C;
  pinD = D;
  pinE = E;
  pinF = F;
  pinG = G;
  pindP = dP;
  
  pinMode(A, OUTPUT);     // Definir las salidas de cada segmento
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(dP, OUTPUT);

  digitalWrite(A, LOW);    // Definir el estado inicial de cada segmento (cátodo)
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, LOW);
  digitalWrite(dP, LOW);
}
//***************************************************************
// Funcion para desplegar los digitos del display
//***************************************************************
void desplegar7Seg(uint8_t digito)   // Configuración de los números en el display y posobles casos
{

  switch (digito)
  {
  case 0:
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF, HIGH);
    digitalWrite(pinG, LOW);
    break;
  case 1:
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, LOW);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF, LOW);
    digitalWrite(pinG, LOW);
    break;
  case 2:
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, LOW);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF, LOW);
    digitalWrite(pinG, HIGH);
    break;
  case 3:
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF, LOW);
    digitalWrite(pinG, HIGH);
    break;
  case 4:
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, LOW);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF, HIGH);
    digitalWrite(pinG, HIGH);
    break;
  case 5:
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, LOW);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF, HIGH);
    digitalWrite(pinG, HIGH);
    break;
  case 6:
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, LOW);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF, HIGH);
    digitalWrite(pinG, HIGH);
    break;
  case 7:
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, LOW);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF, LOW);
    digitalWrite(pinG, LOW);
    break;
  case 8:
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF, HIGH);
    digitalWrite(pinG, HIGH);
    break;
  case 9:
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF, HIGH);
    digitalWrite(pinG, HIGH);
    break;
  default:
    break;
  }
}

//***************************************************************
// Funcion para desplegar el punto del display
//***************************************************************
void desplegarPunto(boolean punto)     
{
  if (punto == 1)
  {
    digitalWrite(pindP, HIGH);
  }
  else
  {
    digitalWrite(pindP, LOW);
  }
}

//***************************************************************
// Funcion para separar los dígitos de temperatura
//***************************************************************
void separarTemperatura(void)
{
  decenas = ((celsius)*(10))/(100);                       // Obtener solamente valor de la decena
  unidades = ((celsius*10)-(decenas*100))/10;             // Obtener solamente valor de unidad 
  decimal = ((celsius*10)-(decenas*100))-(unidades*10);   // Obtener solamente valor de decimal 
}
//***************************************************************
// Funcion para configurar Timer
//***************************************************************
void configurarTimer(void){
  //Fosc = 80MHz 
 // Fosc / Prescaler = 80Mhz/80 = 1,000,0000
 // Tocs = 1/Fosc = 1 uSeg

 //Paso 2: Seleccionar timer
 //Timer 0, prescaler = 80, flanco de subida

 timer = timerBegin(0, prescaler, true);
 
 // Paso 3: Asignar el handler de la interrupcion
 timerAttachInterrupt(timer, ISRTimer, true);

 // Paso 4: Programar alarmas
 // Tic = 1us
 // Frecuencia = Se necesita 250 ms en uSeg

 timerAlarmWrite(timer, 500 , true);
 
 //Paso 5: Iniciar alarma 
 timerAlarmEnable(timer);
  
  } 
