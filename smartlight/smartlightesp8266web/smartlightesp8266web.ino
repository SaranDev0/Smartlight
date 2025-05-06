#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


const char* ssid = "SBH-5,6,7";
const char* password = "123456789";

const int relayPin = D5;


WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); 

  
  IPAddress local_ip(192, 168, 4, 1);  
  IPAddress gateway(192, 168, 4, 1);  
  IPAddress subnet(255, 255, 255, 0);  

  WiFi.softAPConfig(local_ip, gateway, subnet);  
  WiFi.softAP(ssid, password);                  

  delay(100); // small delay for stability


  //Serial.println("Access Point started.");
  //Serial.print("Connect to WiFi: ");
  //Serial.println(ssid);
  //Serial.print("Then go to: http://");
  //Serial.println(WiFi.softAPIP());  // This is the IP of the ESP in AP mode

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  

  Serial.println("\nConnected to WiFi Network!");
  Serial.print("Device IP: ");
  Serial.println(WiFi.localIP()); 

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  Serial.println("New client connected.");

  String request = client.readStringUntil('\r');
  Serial.println("Request: " + request);
  client.flush();

  if (request.indexOf("GET /on") >= 0) {
    digitalWrite(relayPin, HIGH);  // Turn relay ON
  } else if (request.indexOf("GET /off") >= 0) {
    digitalWrite(relayPin, LOW);   // Turn relay OFF
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();

  client.println("<!DOCTYPE html><html lang='en'>");
  client.println("<head><meta charset='UTF-8'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
  client.println("<title>Relay Dashboard</title>");
  client.println("<style>");
  client.println("body { font-family: sans-serif; background:#f2f2f2; text-align:center; }");
  client.println(".card { background:white; padding:30px; margin:50px auto; max-width:400px; border-radius:10px; box-shadow:0 0 10px rgba(0,0,0,0.1); }");
  client.println(".btn { display:inline-block; padding:15px 30px; font-size:18px; margin:10px; border:none; border-radius:5px; cursor:pointer; text-decoration:none; }");
  client.println(".btn-on { background-color:green; color:white; }");
  client.println(".btn-off { background-color:red; color:white; }");
  client.println("</style></head><body>");
  client.println("<div class='card'>");
  client.println("<h1>Relay Dashboard</h1>");

  if (digitalRead(relayPin)) {
    client.println("<p>Status: <strong style='color:green;'>ON</strong></p>");
  } else {
    client.println("<p>Status: <strong style='color:red;'>OFF</strong></p>");
  }

  client.println("<a href='/on'><button class='btn btn-on'>Turn ON</button></a>");
  client.println("<a href='/off'><button class='btn btn-off'>Turn OFF</button></a>");
  client.println("</div></body></html>");
  client.println();

  delay(1);
  client.stop();
  Serial.println("Client disconnected.");
}
