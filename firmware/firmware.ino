#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const char* topic_cmd = "savyo/sonoff/cmd";
const char* topic_status = "savyo/sonoff/status";

// Senha numérica cadastrada no firmware
const String COMANDO_SENHA_ID = "9821"; 

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

  int indiceSeparador = payloadStr.indexOf(':');
  if (indiceSeparador == -1) {
    client.publish(topic_status, "ERRO: Formato de mensagem inválido.");
    return;
  }

  String senhaInserida = payloadStr.substring(0, indiceSeparador);
  String comando = payloadStr.substring(indiceSeparador + 1);

  if (senhaInserida == COMANDO_SENHA_ID) {
    if (comando == "ON") {
      client.publish(topic_status, "STATUS: Lâmpada Ligada.");
    } else if (comando == "OFF") {
      client.publish(topic_status, "STATUS: Lâmpada Desligada.");
    }
  } else {
    client.publish(topic_status, "ERRO: Senha incorreta.");
  }
}

void reconectarServidor() {
  while (!client.connected()) {
    String clientId = "ESP32Client-";
    clientId += String(WiFi.macAddress());
    
    if (client.connect(clientId.c_str())) {
      client.subscribe(topic_cmd);
      client.publish(topic_status, "STATUS: ESP32 Pronto e Conectado.");
    } else {
      delay(5000);
    }
  }
}

void setup() {
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
