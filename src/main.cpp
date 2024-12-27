#include <WiFi.h>                   // Biblioteca para conectividade WiFi
#include <Wire.h>                   // Biblioteca para comunicação I2C
#include <Adafruit_GFX.h>           // Biblioteca gráfica da Adafruit
#include <Adafruit_SSD1306.h>       // Biblioteca para o display OLED
#include "time.h"                   // Biblioteca para manipulação de tempo
#include "config.h"  // Inclua o arquivo de configuração da rede Wi-Fi

#define SCREEN_WIDTH 128 // Largura do display OLED, em pixels
#define SCREEN_HEIGHT 64 // Altura do display OLED, em pixels

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -14400;  // Offset de GMT para o horário de Brasília
const int   daylightOffset_sec = 3600;  // Ajuste para o horário de verão

void setup() {
  Serial.begin(115200);

  // Inicializa o display OLED com endereço I2C 0x3C
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("Falha ao inicializar o display OLED"));
    for(;;); // Entra em loop infinito caso a inicialização falhe
  }
  
  display.display();
  delay(2000);
  display.clearDisplay();

  // Conectar ao Wi-Fi
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado.");
  
  // Inicializa e obtém o tempo
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}

void loop() {
  delay(1000);
  printLocalTime();
  printSignalStrength();  // Adiciona a intensidade do sinal à tela
}

void printLocalTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Falha ao obter o tempo");
    return;
  }
  
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  char timeHour[3];
  strftime(timeHour, sizeof(timeHour), "%H", &timeinfo);
  
  char timeWeekDay[10];
  strftime(timeWeekDay, sizeof(timeWeekDay), "%A", &timeinfo);

  // Mostrar texto no display OLED
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 10);
  display.printf("%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.printf("%s, %02d/%02d/%d", timeWeekDay, timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
  display.display();  // Exibe o texto no display
}

void printSignalStrength() {
  long rssi = WiFi.RSSI();  // Obtém o valor RSSI da conexão Wi-Fi atual

  Serial.print("Intensidade do sinal Wi-Fi: ");
  Serial.print(rssi);
  Serial.println(" dBm");

  // Adiciona a intensidade do sinal à tela OLED
  display.setTextSize(1);
  display.setCursor(0, 55);
  display.printf("WiFi RSSI: %ld dBm", rssi);
  display.display();
}
