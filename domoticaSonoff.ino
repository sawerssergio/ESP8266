// sergiosdevs v1.1
// ESP8266 library se a utilizado una libreria para 
// los pines de potencia son 12 y signal 13 
// aun falta investigar como tener entradas 
// no olvidar setear el sonoff antes de prograr y sobre todo tension 3,3V
#include <ESP8266WiFi.h>

//------------------
int aux1;
int aux2;
int stado;

//-----------------

const char* ssid = "aux";
const char* password = "cuarentena2020";

//-----------------
IPAddress ip(192,168,3,177);     
IPAddress gateway(192,168,3,1);   
IPAddress subnet(255,255,255,0);  
//-----------------

WiFiServer server(80);
String header;

//-----------------
// Auxiliar variables que se usara en los botones

String estado = "off"; //output26State
String cerrado = "off"; //output27State

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define el tiempo en milisefundos (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200); 
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.print("Conectando a la red ");
  //Serial.println(ssid);
  //WiFi.begin(ssid, password);
  //----------------
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a:\t");
  Serial.println(ssid); 
  //----------------
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); 
  }
// Print local IP address  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin(); 
}
void loop(){
  
    WiFiClient client = server.available(); 
    
    
    if (client) {                             // Para nuevo cliente,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("Nuevo Cliente.");          
    String currentLine = "";                
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  
      currentTime = millis();
      if (client.available()) {          
        char c = client.read();             
        Serial.write(c);                    // cuado lo tenemos conectado directamente este nos ayuda a ver si conecta o no 
        header += c;
        if (c == '\n') {                    
          
          if (currentLine.length() == 0) {
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
         
            // funcionamiento de los GPIOs on y off
            
            if (header.indexOf("GET /on") >= 0) {
              Serial.println("Abriendo");
                estado = "on";     
                digitalWrite(12, HIGH);  
               } // good
            if (header.indexOf("GET /off") >= 0) {
              Serial.println("Cerrando");
                estado = "off";     
                digitalWrite(12, LOW);  
               } // good
            
            // codigo HTML web 
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS style de on/off buttons 
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            //client.println(".button3 {background-color: #FF0000;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Quillacollo</h1>"); // Nombre de la web
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p> Estado actual " + estado + "</p>");
                  
            //------------------------------------------------------------------------------------------------
            if (estado =="off") {
              client.println("<p><a href=\"/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 

               
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  delay (1000);  
}
