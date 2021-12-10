#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

char nombreDia[7][12] = {"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};
char nombreMes[12][12]= {"Enero", "Febrero", "Marzo", "Abril", "Mayo",  "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"};
int segundo, minuto, hora, dia, mes, diaDeLaSemana;
long anio; //variable año

DateTime HoraFecha;

void setup () {
  Serial.begin(9600);
  rtc.begin(); //Inicializamos el RTC
  Serial.println("Estableciendo Hora y fecha...");
  bool update = 1;
  if(update){
	  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
	  Serial.println("DS1307 actualizado con la hora y fecha que se compilo este programa:");
	  Serial.print("Fecha = ");
	  Serial.print(__DATE__);
	  Serial.print("  Hora = ");
	  Serial.println(__TIME__);
	}
}

void loop () {
    HoraFecha = rtc.now(); //obtenemos la hora y fecha actual
    
    segundo=HoraFecha.second();
    minuto=HoraFecha.minute();
    hora=HoraFecha.hour();
    dia=HoraFecha.day();
    mes=HoraFecha.month();
    anio=HoraFecha.year();
    diaDeLaSemana=HoraFecha.dayOfTheWeek();
    

    //Enviamos por el puerto serie la hora y fecha.
    Serial.print(hora);
    Serial.print(":");
    Serial.print(minuto);
    Serial.print(":");
    Serial.print(segundo);
    Serial.print(" , ");
    Serial.print(nombreDia[diaDeLaSemana]);
    Serial.print(" ");
    Serial.print(dia);
    Serial.print(" de ");
    Serial.print(nombreMes[mes-1]);
    Serial.print(" del ");
    Serial.print(anio);
    Serial.println();
    delay(1000);
}