#include <DHT.h>
#include "ArduinoJson.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

 
/* Code zu dem Video https://youtu.be/5tG3JXFYrUo von Ali Panjaitan
*/
const char* ssid="Asura";
const char* pass="abcdefgh";
const char* brokerUser = "Asura";
const char* brokerPass = "Tholamtho123";
const char* broker = "broker.hivemq.com";
const char* outTopic ="ESP_data";
const char* inTopic ="Web_data";
#define DHTPIN 2 // Pin D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float t,h;
int led_red = 14; //Pin D5
int led_green = 12; //Pin D6
int led_blue = 13; //Pin D7
int buzzer = 4; // Pin D2
WiFiClient espClient;
PubSubClient client(espClient);
long currentTime, lastTime;
int count = 0;
char txt[3000];
String line;
char messages[100];


 
void setupWifi(){
  delay(100);
  Serial.print("\nConnecting to");
  Serial.println(ssid);
 
  WiFi.begin(ssid, pass);
 
  while(WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print("-");
  }
 
  Serial.print("\nConnected to ");
  Serial.println(ssid);
}
 
void reconnect(){
  while(!client.connected()){
    Serial.print("\nConncting to ");
    Serial.println(broker);
    if(client.connect("espnext01", brokerUser, brokerPass)){
      Serial.print("\nConnected to ");
      Serial.println(broker);
      client.subscribe(inTopic);
    } else {
      Serial.println("\n Trying to reconnect");
      delay(5000);
    }
  }
}
//// Call back from MQTT Server
void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Received Data: ");
  Serial.println(topic);
  for(int i=0; i<length; i++){
    Serial.print((char) payload[i]);
    txt[i] = payload[i];
  }
  line = length;
  Serial.println("AAAAA");
  Serial.println(txt);
  JsonParse();
  
}
 
void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(led_red, OUTPUT);
    pinMode(led_green, OUTPUT);
    pinMode(led_blue, OUTPUT);
    pinMode(buzzer, OUTPUT);
    setupWifi();
    client.setServer(broker, 1883);
    client.setCallback(callback);
}
 
void loop() {
    // put your main code here, to run repeatedly:
    if (!client.connected()){
      reconnect();
    }
  client.loop();
  sendSensor();

  delay(3000);
}


void JsonParse(){
    Serial.println("Start Parsing");
    StaticJsonBuffer<2000> JsonTXT;
    JsonObject& parsed = JsonTXT.parseObject(txt);
    if(!parsed.success()){
      Serial.println("Parsing Failed!");
      delay(1000);
      }
     int red = parsed["RED"];
     int green = parsed["GREEN"];
     int blue = parsed["BLUE"];
     int value = parsed["BUZZER"];
     digitalWrite(led_red, red);
     digitalWrite(led_green, green);
     digitalWrite(led_blue, blue);
     digitalWrite(buzzer, value);
     
}
void sendSensor()
{
  h = dht.readHumidity();
  t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  delay(3000);
  Serial.println(t);
  Serial.println(h);
  EncodeJson();
  currentTime = millis();
  if(currentTime - lastTime > 2000){
    Serial.print("Sending Messages: ");
    Serial.println(messages);
    client.publish(outTopic, messages);
    lastTime = millis();
  }

}
void EncodeJson(){
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
  JSONencoder["Temp"] = t;
  JSONencoder["Humi"] = h;
  int lenghtSimple = JSONencoder.measureLength();
  Serial.print("Less overhead JSON message size: ");
  Serial.println(lenghtSimple);
  JSONencoder.printTo(Serial);
  JSONencoder.printTo(messages, sizeof(messages));
  Serial.println("Sending message to MQTT topic..");
  Serial.println(messages);
  
  }
