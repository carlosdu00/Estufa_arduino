#include <Ethernet.h>
EthernetClient client;
///////////////ethernet//////////////////
#include "DHT.h"
#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
///////////////sonda/////////////////////
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#define ONE_WIRE_BUS 5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sonda(&oneWire);
////////////////////////define os padroes base para humidade do solo/////////////////////////////////////
const int seco = 641;
const int humido = 267;
/////////////////////////////////////////////////////////////
float LDR = A1;
int luminosidade = 0;
/////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  ////////////inicia o sensor do ar//////////////
  dht.begin();
  //////////////////////////
  sonda.begin();
  ////////////////////////////
  pinMode(LDR, INPUT);
  /////////////////////////configurando conexão/////////////////////////
  while (!Serial) continue;

  // Initialize Ethernet library
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  //Ethernet.init(8);  // use pin 53 for Ethernet CS

  if (!Ethernet.begin(mac)) {
    Serial.println(F("Failed to configure Ethernet"));
    return;
  }
  delay(1000);
  Serial.println(F("Connecting..."));

  // Connect to HTTP server
  client.setTimeout(10000);

  if (client.connect("http://estufaarduino.herokuapp.com", 80)) {
    Serial.println("Connected to server");
  }
}


void loop() {
  /////////////////////////leituda de temperatuda e humidade do ar /////////////////////////////////////

  float humidade_ar = dht.readHumidity();
  float temperatura_ar = dht.readTemperature()*0.993;

  if (isnan(humidade_ar) || isnan(temperatura_ar)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  
  Serial.print(F("\"temperatura_ar\": "));
  Serial.print(temperatura_ar);
  Serial.println(",");

  Serial.print(F("\"humidade_ar\": "));
  Serial.print(humidade_ar,0);
  Serial.println(",");

  ////////////////////////////////leitura da temperatura do solo//////////////////////////////


  sonda.requestTemperatures();
  float temperatura_solo=sonda.getTempCByIndex(0)*1.007;

  Serial.print(F("\"temperatura_solo\": "));
  Serial.print(temperatura_solo);
  Serial.println(",");

  ////////////////////////////leitura da humidade do solo//////////////////////////////////


  int leitura_sonsor_humidade_solo = analogRead(A0);
  int humidade_solo = map(leitura_sonsor_humidade_solo, humido, seco, 100, 0);

  Serial.print(F("\"humidade_solo\": "));
  Serial.print(humidade_solo);
  Serial.println(",");

  /////////////////////////////leitura da luminosidade/////////////////////////////////
  luminosidade = analogRead(LDR);

  Serial.print(F("\"luminosidade\": "));
  Serial.println(luminosidade);
  
  //////////////////////////HTTP request///////////////////////////////////
    String PostData = "{\"temperatura_ar\": "+String(temperatura_ar)+",\"humidade_ar\": "+String(humidade_ar)+",\"temperatura_solo\": "+String(temperatura_solo)+",\"humidade_solo\": "+String(humidade_solo)+",\"luminosidade\": "+String(luminosidade)+"}";
    Serial.println(PostData);
    client.println("POST /sistema/leituras HTTP/1.1");
    client.println("Host: estufaarduino.herokuapp.com");
    client.println("Cache-Control: no-cache");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);
    /////////////////////////////////////////////////////////
    delay(5000);
}
