# Development-of-a-Sonoff-device-with-ESP32

## Protótipo de Dispositivo Smart IoT (Estilo Sonoff) com ESP32

Este projeto apresenta a documentação detalhada e o código-fonte para o desenvolvimento de um dispositivo inteligente de acionamento remoto (Smart On/Off), utilizando o microcontrolador ESP32 integrado ao protocolo de comunicação MQTT. 

## Arquitetura do Projeto - Fase 1: Conexão e Segurança

Na primeira fase do projeto, isolamos a camada de infraestrutura de rede e estabelecemos um protocolo básico de segurança diretamente no firmware do dispositivo. O ESP32 conecta-se à rede sem fio e assina um tópico de comando estruturado.

### Diferenciais de Implementação:
* **Validação de Payload:** Para mitigar acessos não autorizados por meio de brokers públicos, o firmware exige que as mensagens de comando sigam o formato estrito `SENHA:COMANDO`.
* **Resiliência de Rede:** Rotinas automáticas de checagem tratam quedas de conexão Wi-Fi e desconexões do servidor MQTT sem travar o loop principal do sistema.

