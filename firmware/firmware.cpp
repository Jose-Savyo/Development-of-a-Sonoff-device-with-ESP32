#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const char* topic_cmd = "savyo/sonoff/cmd";
const char* topic_status = "savyo/sonoff/status";

const String COMANDO_SENHA_ID = "9821"; 

// Definição dos 8 pinos de saída correspondentes aos LEDs
const int quantidadeLeds = 8;
const int pinosLeds[quantidadeLeds] = {13, 12, 14, 27, 26, 25, 33, 32};

WiFiClient espClient;
PubSubClient client(espClient);

void configurarWifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void processarComandos(char* topic, byte* payload, unsigned int length) {
  String payloadStr = "";
  for (unsigned int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }

  // O formato esperado agora é "SENHA:LED_INDEX:ESTADO" (Ex: "9821:0:ON")
  int primeiroSeparador = payloadStr.indexOf(':');
  int segundoSeparador = payloadStr.lastIndexOf(':');
  
  if (primeiroSeparador == -1 || segundoSeparador == primeiroSeparador) {
    client.publish(topic_status, "ERRO: Formato invalido. Use SENHA:LED:ESTADO");
    return;
  }

  String senhaInserida = payloadStr.substring(0, primeiroSeparador);
  int ledIndex = payloadStr.substring(primeiroSeparador + 1, segundoSeparador).toInt();
  String comando = payloadStr.substring(segundoSeparador + 1);

  // Validações de segurança e escopo
  if (senhaInserida != COMANDO_SENHA_ID) {
    client.publish(topic_status, "ERRO: Senha incorreta.");
    return;
  }

  if (ledIndex < 0 || ledIndex >= quantidadeLeds) {
    client.publish(topic_status, "ERRO: Indice de LED invalido (Use de 0 a 7).");
    return;
  }

  // Execução do acionamento no pino correspondente
  if (comando == "ON") {
    digitalWrite(pinosLeds[ledIndex], HIGH);
    client.publish(topic_status, ("STATUS: LED " + String(ledIndex) + " Ligado.").c_str());
  } else if (comando == "OFF") {
    digitalWrite(pinosLeds[ledIndex], LOW);
    client.publish(topic_status, ("STATUS: LED " + String(ledIndex) + " Desligado.").c_str());
  }
}

void reconectarServidor() {
  while (!client.connected()) {
    String clientId = "ESP32Client-";
    clientId += String(WiFi.macAddress());
    
    if (client.connect(clientId.c_str())) {
      client.subscribe(topic_cmd);
      client.publish(topic_status, "STATUS: ESP32 Multi-Canais Pronto.");
    } else {
      delay(5000);
    }
  }
}

void setup() {
  // Inicialização dinâmica de todos os pinos configurados no array
  for (int i = 0; i < quantidadeLeds; i++) {
    pinMode(pinosLeds[i], OUTPUT);
    digitalWrite(pinosLeds[i], LOW); // Garante que iniciam desligados
  }
  
  configurarWifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(processarComandos);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    configurarWifi();
  }
  if (!client.connected()) {
    reconectarServidor();
  }
  client.loop();
}
