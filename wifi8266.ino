//Lolin(WEMOS) D1 R2 y mini
//Librerias
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

  //Datos de internet
String ssid = "ale";
String password = "12345678";
WiFiServer servidor(80); 

  //Pines 
#define trigPin D0
#define echo D1
#define pila A0

  //Altura del tinaco
int tinaco = 30;


  //-------------------------SETUP---------------------------------//
void setup() {
  
  Serial.begin(9600);//iniciailzamos la comunicación

  //Iniciar conexión
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
    }
  
  //Iniciar DNS
    if (!MDNS.begin("Medidor")) {
        while (1) {
          delay(1000);
        }
      }
    servidor.begin();
    MDNS.addService("http", "tcp", 80);

  //Definir pines
    pinMode(trigPin, OUTPUT);
    digitalWrite(trigPin, LOW);
    pinMode(echo, INPUT);
    pinMode(pila, INPUT);


}

  //-------------------------LOOP---------------------------------//
void loop() {

  //Servidor y DNS
   MDNS.update();
   WiFiClient cliente = servidor.available();
    if(!cliente){
        return;
      }

  //Sensor ultrasónico
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);      //Enviamos un pulso de 10us
    digitalWrite(trigPin, LOW);


    long t; //timepo que demora en llegar el eco
    long d; //distancia en centimetros
    t = pulseIn(echo, HIGH); //obtenemos el ancho del pulso
    d = t / 59;
    delay(100);

  //Obtener valores en porcentaje
    int dis = porcentaje(d);
    int bat = porcentajePila(analogRead(pila));

    //-------------------------HTML---------------------------------//
     cliente.print("HTTP/1.1 200 OK");
     cliente.println("Content-Type: text/html");
     cliente.println("");
     cliente.println("<!DOCTYPE html> <html> <head> <title> Medidor Tinaco </title> <meta name='viewport' content='width=device-width'> <meta http-equiv='refresh' content='10' />");
     cliente.println("<link rel='preconnect' href='https://fonts.googleapis.com'> <link rel='preconnect' href='https://fonts.gstatic.com' crossorigin> <link href='https://fonts.googleapis.com/css2?family=Poppins:wght@300&family=Source+Sans+Pro:wght@900&display=swap' rel='stylesheet'>"); 
     cliente.println( "<style> body { padding: 30px; background-color: rgb(235, 235, 235); font-family: 'Poppins', sans-serif; } ");
     cliente.println("h1 { text-align: center; font-family: 'Source Sans Pro', sans-serif; font-size: 40px; color: rgb(33, 33, 87); margin: 0; }");
     cliente.println(".graph { display: flex; flex-direction: row; align-items: center; justify-content: center; padding: 50px; height: 280px; background-color: rgb(255, 255, 255); margin: 30px; box-shadow: 2px 2px 10px rgb(70, 70, 70); font-size: 19px; } ");
     cliente.println(".bar, .barFill { bottom: 0; /* box-shadow: inset 0px 0px 3px rgb(70, 70, 70);*/ } .bar { position: relative; background-color: rgb(226, 224, 224); height: 100%; border-radius: 13px; width: 26px; /* border: 2px solid rgb(33, 33, 87); */ }");
     cliente.println(".percentage { display: inline-block; padding-left: 30px; } .footer { background-color: rgb(34, 34, 77); position: fixed; display: flex; flex-direction: row; justify-content: space-between; height: 65px; bottom: 0; left: 0; right: 0; padding: 30px 25px 10px 25px; margin: 0; font-size:  17px; color: white; }"); 
     cliente.println(".bold2 { font-weight: bold; color: lightblue; } .bold { font-weight: bold; color: rgb(33, 33, 87); } .barFill { position: absolute; background-color:  rgb(110, 167, 252); border-radius: 13px;  width: 25px; height:");
     cliente.print(dis);
     cliente.print("%; }  </style></head>");
     cliente.println("<body> <h1> Medidor Tinaco </h1> <div class='graph'> <div class='bar'> <div class='barFill'> </div> </div> <div class='percentage'> <span class = 'bold'> Nivel de agua: </span> <br>");
     cliente.print(dis);
     cliente.print("% </div> </div>");
     cliente.println("<div class='footer' > <div class='bateria'> <span class = 'bold2'> Bateria: </span>");
     cliente.print(bat);
     cliente.print("</div>");
     cliente.println("<div class='red'> <span class='bold2'>  Red: </span> ");
     cliente.print(WiFi.SSID());
     cliente.print(" <span class='bold2'>  <br> Estado: </span> Conectado </div> </div> </body> </html>" );
     
      
}

  //Función porcentaje de agua
  int porcentaje (long distance){
    int lleno = tinaco - (int)distance;
    return lleno*100/tinaco; 
  
  }

  //Función porcentaje de pila
  int porcentajePila (int b) {
    return b*100/1024;
    
  }
