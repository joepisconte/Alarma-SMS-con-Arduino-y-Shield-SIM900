#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#define PIN_TX    7
#define PIN_RX    8
#define BAUDRATE  9600
#define MESSAGE_LENGTH 160
#define PHONE_NUMBER "9********" // Especificamos el número de celular 9******** al cual se enviará el SMS.
#define MESSAGE  "Alarma Activada!!!" // Especificamos el text del mensaje que se enviará.

char message[MESSAGE_LENGTH];
int messageIndex = 0;
char phone[16];
char datetime[24];

boolean SensorPIR;
boolean Alarma;

GPRS gprsTest(PIN_TX,PIN_RX,BAUDRATE); //Especificamos los pines del Arduino a los cuales se han conectado el RX,TX,PWR y BaudRate del SIM900.

void setup() {
  pinMode(9,INPUT); // Especificamos el Pin como entrada para leer el sensor PIR
  Serial.begin(9600);
  while(!gprsTest.init()) {
      Serial.print("Error al inicializar SIM900. Verificar conexiones\r\n");
      delay(1000);
  }
  delay(1000);  
  Serial.println("SIM900 inicializado de manera satisfactoria!");
}

void loop() {
  SensorPIR = digitalRead(9);
  if (SensorPIR == 1 && Alarma == 0){
    EnviarSMS();
    Alarma == 1; //activamos alarma.
  }
  LeerSMS();  
}

void LeerSMS(){
  messageIndex = gprsTest.isSMSunread(); // Preguntamos si hay Mensajes disponibles.
  if (messageIndex > 0) { // Si ... existe más de un SMS en la bandeja ... entonces
    gprsTest.readSMS(messageIndex, message, MESSAGE_LENGTH, phone, datetime);           
    gprsTest.deleteSMS(messageIndex);  //Borramos el SMS para no llenar la memoria del SIM900.
    Serial.print("Nro Celular: ");
    Serial.println(phone);  
    Serial.print("Fecha: ");
    Serial.println(datetime);        
    Serial.print("Mensaje Recibido: ");
    Serial.println(message);   
    AnalizarTramaSMS(message); 
  }
}

void EnviarSMS(){
   gprsTest.sendSMS(PHONE_NUMBER,MESSAGE); //enviamso el SMS al número de celular antes especificado.
}

void AnalizarTramaSMS(char Trama[]){
/* Recepción de varios datos separados por comas.
 * Se espera un mensaje en el formato: 1,0,1,1$
 * Este programa requiere un $(dolar) para indicar el final de la trama
 * Cada campo representa el estado lógico de una salida digital del Arduino.
 * El primer campo controla la sirena y los otros tres controlan la iluminación.
 * En este ejemplo se controlará el encedido y apagado de un módulo de 08 relays.
 */
   const int NUMERO_DE_CAMPOS = 4; // numero de campos esperados.
   int fieldIndex = 0; // índice actual de los campos recibidos.
   char Datos[NUMERO_DE_CAMPOS]; // array conteniendo los valores de todos los campos
   char ch;
   for (int i=0;i<MESSAGE_LENGTH;i++){
     ch = Trama[i];
     if(ch >= '0' && ch <= '9'){ // Si ... el caracter recibido está entre 0 y 9  ... entonces
       Datos[fieldIndex] = ch; //almacena el número recibido.
     } else{ 
       if (ch == ',') // Si ... detecta coma ... entonces 
       {
         if(fieldIndex < NUMERO_DE_CAMPOS-1){ 
          fieldIndex++; // Incrementa el índice de campo.
         }
       }
       else{ 
         //cualquier caracter diferente (a un número o coma) termina la adquisición, ese caracter sería el "$"
         for(int i=0; i <= fieldIndex; i++)
            { 
              switch(Datos[i]){
                case '1':
                    digitalWrite(i+2,LOW); //Encendemos los relays conectados a los pines 2, 3, 4 y 5.
                break;
                case '0':
                  if (i==0){
                    Alarma = 0; //desactiva la alarma
                  }else{
                    digitalWrite(i+2,HIGH); //Apagamos los relays conectados a los pines 2, 3, 4 y 5.
                  }
                break;
              }
            }
       }
     }
   }
}

