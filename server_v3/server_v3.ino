#include <WiFi.h>
#include <WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LittleFS.h>
#include <esp_now.h>
#include <esp_wifi.h>


// WiFi
const char* ssid = "dittNett";
const char* password = "dittPassord";

// ----------- ESP-NOW SALINITET -----------

// Sett MAC til salinitets-node
// uint8_t salinityNodeMAC[] = {0xE4, 0x65, 0xB8, 0x7E, 0x31, 0xD8}; // esp med microusb
uint8_t salinityNodeMAC[] = {0x30, 0xC6, 0xF7, 0xB8, 0xF5, 0x04};  // esp med usbc

typedef struct {
  uint8_t type;   // 0=request, 1=response
  float salinity;
} SalinityPacket;

// opretter en instans av salinitetsdataen som sendes frem og tilbake
SalinityPacket salinityPkt;

// Temp
OneWire oneWire(32);
DallasTemperature sensors(&oneWire);
DeviceAddress addr;
bool tempSensorOK = false;

// Trykk
const int pressurePin = 34;
const float OffSet = 0.243; // må også måles og eventuelt endres
const float DIVIDER = (9.98 + 14.8) / 14.8;
const int spenning_trykk = 490; //denne må måles og endres
const float vann_trykk_dybde = 0.102;

// Server
WebServer server(80);

// ----------- Cache -----------
float tempCache = NAN;
float pressureCache = NAN;
float salinityCache = NAN;

// ----------- Sampling -----------
unsigned long lastSample = 0;
const unsigned long sampleInterval = 2000;

// ----------- HISTORIKK -----------
#define HISTORY_LEN 30

float tempHistory[HISTORY_LEN];
float pressureHistory[HISTORY_LEN];
float salinityHistory[HISTORY_LEN];

int histIndex = 0;

// ----------- SETUP -----------
void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());
  delay(100);

  // ESP-NOW init
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init feilet");
    return;
  }
  
  // setter callback funksjonen for når vi mottar data
  esp_now_register_recv_cb(onEspNowReceive);

  // starter koblingen mellom esp32-ene
  salinitet_start();

  // starter sensorer og sjekker dem
  sensors_start();

  // starter opp LittleFS (filsystemet) og sjekker om det funker
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed");
    return;
  }

  // kobler til WIFI og starter html siden
  server_start();
}

// ----------- LOOP -----------
void loop() {
  server.handleClient();

  // -------- Sensor sampling --------
  if (millis() - lastSample >= sampleInterval) {
    lastSample = millis();
    sensor_get_values();
    }
}
