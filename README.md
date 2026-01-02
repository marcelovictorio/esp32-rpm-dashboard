🚗 ESP32 RPM – Painel Automotivo Digital com OLED e Dashboard Web

Este projeto implementa um medidor de RPM em tempo real utilizando ESP32, com visualização simultânea em um display OLED e em um dashboard web automotivo acessado pelo navegador.

A solução combina eletrônica embarcada, processamento em tempo real e interface web, tornando-se uma excelente referência para projetos IoT, automotivos e maker.

🔧 Funcionalidades

Medição de RPM via sensor infravermelho (IR / LM393)

Leitura de pulsos por interrupção (hardware interrupt)

Cálculo e suavização do valor de RPM

Exibição em display OLED 128×64

RPM instantâneo

RPM máximo

VU Meter animado

Dashboard web em tempo real

Tacômetro semicircular (estilo automotivo)

Atualização contínua via Wi-Fi

Acesso por smartphone ou PC

ESP32 operando como Access Point (AP)

Exibição do endereço IP no display OLED

Suporte a mDNS (nome DNS local)

Código modular e comentado

🧰 Hardware Utilizado

ESP32 (DevKit / WROOM / similar)

Sensor infravermelho IR (LM393 / FC-51)

Display OLED 0.96" 128×64 (I²C)

Fonte 5V estável (recomendado uso de capacitores de filtragem)

📚 Bibliotecas

WiFi

WebServer

ESPmDNS

Adafruit SSD1306

Adafruit GFX

(Opcional) QRCode – caso utilize exibição de QR Code no OLED

🌐 Como Acessar o Dashboard

Ligue o ESP32

Conecte-se ao Wi-Fi ESP32-RPM

Verifique o endereço IP exibido no display OLED

Acesse o IP pelo navegador do smartphone ou computador

🎯 Aplicações

Projetos automotivos e painéis digitais

Bancada de testes e motores

Robótica e automação

Estudos de ESP32, IoT e interfaces embarcadas

Demonstrações didáticas e workshops

🚀 Próximas Evoluções (Roadmap)

WebSocket para menor latência

OTA (atualização de firmware via navegador)

Suporte a múltiplos sensores

Layouts alternativos de dashboard

Versão otimizada para ESP32-C3

📄 Licença

Este projeto é open-source e pode ser utilizado para fins educacionais e experimentais.
