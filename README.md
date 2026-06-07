
# Painel de Controle IoT Multi-Canais (Estilo Sonoff) com ESP32

Este repositório contém o código-fonte, os arquivos de configuração de hardware e a interface de usuário para um protótipo avançado de automação residencial multi-canais (8 saídas independentes), inspirado no ecossistema Sonoff. O sistema utiliza o microcontrolador ESP32 integrado ao protocolo de comunicação MQTT e conta com uma interface web estática hospedada diretamente no GitHub Pages.

## 🚀 Arquitetura do Sistema

O projeto foi construído de forma totalmente desacoplada, utilizando comunicação segura via WebSockets no lado do cliente web e o protocolo MQTT convencional no lado do hardware simulado.

```text
+-----------------------+              +-----------------------+              +-----------------------+
|   GitHub Pages (UI)   |  WebSockets  |    Broker HiveMQ      |     MQTT     |    ESP32 Simulado     |
|  - HTML5 / CSS Grid   |   Seguro     |  (broker.hivemq.com)  |  Puro (1883) |      (No Wokwi)       |
|  - JS (Paho MQTT)     | -----------> |                       | -----------> |                       |
|  - Validação de Senha |  Porta 8884  |  Roteamento Interno   |              |  - Vetor de 8 GPIOs   |
|  - Grade de 8 Canais  |  (Cripto)    |  de Mensagens (WSS)   |              |  - Parsing de Payload |
+-----------------------+              +-----------------------+              +-----------------------+

```

### Componentes Principais:

1. **Frontend (Interface do Usuário):** Desenvolvido em HTML5 e CSS Grid dinâmico. Adapta-se automaticamente a telas de smartphones e computadores. Utiliza a biblioteca Paho MQTT via CDN para estabelecer conexão com o broker usando criptografia TLS (WSS).
2. **Mensageria (Broker):** Utiliza a infraestrutura pública da HiveMQ, atuando como ponte de baixa latência e fazendo a conversão transparente de dados entre a porta WebSockets (8884) e a porta TCP padrão (1883).
3. **Firmware (Hardware Simulado):** Código em C++ compilado para o ESP32 dentro do ambiente Wokwi. Gerencia o estado de 8 pinos digitais simultâneos através de vetores (arrays) e estruturas de repetição eficientes.

---

## 🔒 Mecanismo de Segurança (Validação por Payload)

Para mitigar os riscos de varredura ou comandos acidentais comuns em brokers públicos de MQTT, foi implementado um protocolo de validação estrito diretamente no firmware do dispositivo.

Toda mensagem enviada pela página web é envelopada no formato de string estruturada:
`SENHA:INDICE_DO_LED:ESTADO`

**Exemplo Prático:**

* Para ligar o canal 4 usando a credencial padrão configurada (`9821`), a página web transmite a string `9821:3:ON`.
* O ESP32 intercepta o pacote, realiza o tratamento da string buscando o caractere delimitador (`:`), valida se o token numérico coincide com a memória local e confere se o índice está dentro do escopo de canais válidos (0 a 7). Se houver divergência, o comando é descartado e um log de erro é publicado de volta para o display do site.

---

## 💻 Estrutura do Repositório

```text
├── index.html            # Interface web com a grade dinâmica e lógica Paho MQTT
├── firmware/
│   └── src/
│       └── main.cpp        # Firmware do ESP32 para gerenciamento dos 8 canais
├── diagram.json          # Arquivo de configuração e conexões da simulação Wokwi
└── README.md             # Documentação técnica do projeto

```

---

## 🔮 Atualizações Futuras: Agendamento Temporal (Timer & Scheduler)

O próximo marco miliário (Milestone) do projeto focará na transformação do dispositivo em um temporizador inteligente de automação, permitindo agendar o acionamento e o desligamento automático dos canais de forma independente.

### Próximas Implementações Técnicas:

* **Sincronização de Relógio com NTP (Network Time Protocol):** Integração da biblioteca nativa do ESP32 para buscar o horário oficial e o fuso horário da rede através de servidores de tempo da internet, dispensando a necessidade de um módulo físico de RTC (Real Time Clock).
* **Novo Payload de Agendamento:** Expansão do interpretador de comandos do firmware para aceitar strings do tipo `SENHA:INDICE_DO_LED:TIMER:MINUTOS` (para contagem regressiva) ou `SENHA:INDICE_DO_LED:SCHEDULE:HH:MM` (para horários fixos do dia).
* **Interface Dinâmica de Agendamento:** Inclusão de campos do tipo `<input type="time">` em cada um dos 8 cartões da página web, permitindo que o usuário envie o comando de temporização diretamente da tela do celular.
* **Rotina de Checagem no Loop Principal:** Criação de uma função não-bloqueante baseada na função `millis()` para comparar constantemente o horário atual obtido via NTP com os horários agendados na memória do ESP32, garantindo precisão milimétrica sem congelar o tráfego de dados MQTT.

```

Você prefere que eu faça mais algum ajuste técnico nas especificações dos tópicos ou essa estrutura atende completamente ao que você precisa subir no commit de hoje?

```
