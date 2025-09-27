// incluindo as bibliotecas necessárias
#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

#define servoPin 2 //pino do servo

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char *SSID = "Wokwi-GUEST"; //SSID do wifi
const char *PASSWORD = ""; //senha do wifi
const char *BROKER_MQTT = "broker.hivemq.com"; //endereço do broker
const int BROKER_PORT = 1883; // porta do broker
const char *TOPIC_SUB_DISPLAY = "sprint/victor"; //topico que quer subscrever

WiFiClient espClient;
PubSubClient MQTT(espClient);
Servo servoMotor;

// função para escrever no diplay mais facilmente
void displayText(String line1, String line2 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(line1);
  if (line2 != "") {
    display.setCursor(0, 16);
    display.println(line2);
  }
  display.display();
}

// função para iniciar conexão
void initWiFi() {
  displayText("Conectando WiFi:", SSID);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(100);
  
  String ip = WiFi.localIP().toString();
  displayText("Conectado", "IP: " + ip);
  delay(2000);
}

// função para iniciar a conexão com o broker mqtt e receber as mensagens do tópico subscrito
void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback([](char *t, byte *p, unsigned int l) {
    String msg = String((char*)p).substring(0, l);
    displayText(msg);
    if (msg == "Acesso Liberado") {
      servoMotor.write(90);
      delay(2000);
      servoMotor.write(0);
    }
  });
}

// função para reconectar com o broker caso a conexão tenha sido interrompida
void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.println();("Conectando MQTT...", BROKER_MQTT);
    if (MQTT.connect(ID_MQTT)) {
      Serial.println("MQTT Conectado!");
      MQTT.subscribe(TOPIC_SUB_DISPLAY);
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    displayText("Falha no Display");
    while (true);
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  servoMotor.attach(servoPin);
  servoMotor.write(0);

  initWiFi();
  initMQTT();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) initWiFi();
  if (!MQTT.connected()) reconnectMQTT();
  MQTT.loop();
}