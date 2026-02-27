/* Create a WiFi access point and provide a web server on it. */
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "Garagem"
#define APPSK "papmartim"
#endif

// Ligação dos PINS
const int ledbranco1 = D1;
const int ledbranco2 = D2;
const int ledverde1  = D3;
const int ledverde2  = D4;
const int sensor     = D5;

// Variáveis - PIR
int val = 0;

// Variáveis - SERVO
Servo Servo1;
Servo Servo2;

// 🔢 Contador de aberturas
int contadorAberturas = 0;

// 🚦 Estado do portão
bool portaoAberto = false;

// Variáveis WiFi
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);

// 🌐 Página Web
void handleRoot() {
  String estadoTexto = portaoAberto ? "ABERTO" : "FECHADO";
  String estadoCor   = portaoAberto ? "green" : "red";

  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='utf-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Garagem</title>";

  html += "<style>";
  html += "body{font-family:Arial;text-align:center;background:#f2f2f2;padding:20px;}";
  html += "h1{font-size:48px;}";
  html += "h2{font-size:36px;}";
  html += ".estado{font-size:60px;font-weight:bold;color:" + estadoCor + ";}";
  html += "button{font-size:40px;padding:25px 60px;margin:20px;border-radius:15px;}";
  html += "</style>";

  html += "</head><body>";

  html += "<h1>🚗 Controlo da Garagem</h1>";

  html += "<div class='estado'>" + estadoTexto + "</div><br>";

  html += "<a href='/abrir'><button>🔓 ABRIR</button></a><br>";
  html += "<a href='/stop'><button>🔒 FECHAR</button></a><br><br>";

  html += "<h2>🔢 Quantas vezes abriu</h2>";
  html += "<h1>" + String(contadorAberturas) + "</h1>";

  html += "</body></html>";

  server.send(200, "text/html", html);
}

// 🔓 Abrir portão
void abrirportao() {
  Servo1.write(180);
  Servo2.write(180);

  digitalWrite(ledverde1, HIGH);
  digitalWrite(ledverde2, HIGH);
  digitalWrite(ledbranco2, HIGH);

  contadorAberturas++;
  portaoAberto = true;

  Serial.println("A abrir portão");

  server.sendHeader("Location", "/");
  server.send(303);
}

// 🔒 Fechar portão
void stop() {
  Servo1.write(0);
  Servo2.write(0);

  digitalWrite(ledverde1, LOW);
  digitalWrite(ledverde2, LOW);
  digitalWrite(ledbranco2, LOW);

  portaoAberto = false;

  Serial.println("A fechar portão");

  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  pinMode(ledbranco1, OUTPUT);
  pinMode(ledbranco2, OUTPUT);
  pinMode(ledverde1, OUTPUT);
  pinMode(ledverde2, OUTPUT);
  pinMode(sensor, INPUT);

  digitalWrite(ledbranco1, LOW);
  digitalWrite(ledbranco2, LOW);
  digitalWrite(ledverde1, LOW);
  digitalWrite(ledverde2, LOW);

  Servo1.attach(D6);
  Servo2.attach(D7);

  Serial.begin(115200);
  delay(1000);

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();

  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.on("/abrir", abrirportao);
  server.on("/stop", stop);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  // Sensor PIR
  val = digitalRead(sensor);
  if (val == HIGH) {
    digitalWrite(ledbranco1, HIGH);
  } else {
    digitalWrite(ledbranco1, LOW);
  }
}