#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Arduino.h>
#include <driver/dac.h>

// Pins
#define SENSE_ADC_PIN 34
#define REF_ADC_PIN 32

// Settings
const int SAMPLES_PER_BURST = 150; // How many points to "hunt" for a peak
const int AVERAGING_COUNT = 20;    // How many peaks to average together
const float DC_OFFSET = 2048.0;    // The 1.65V center point (adjust if needed)

// WiFi
const char* ssid = "dittNett";
const char* password = "dittPassord";

// definerer ration gloabtl slik at den kan brukes av mottaker/sender
float ratio;

// ----------- STRUCT -----------
typedef struct {
  uint8_t type;   // 0=request, 1=response
  float salinity;
} SalinityPacket;

SalinityPacket incoming;
SalinityPacket response;

// ----------- SETUP -----------
void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  delay(100);
  Serial.println(WiFi.macAddress());

  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // sjekker hjemmekanalen til esp32, må være samme på begge
  Serial.print("Wi-Fi Channel set to: ");
  Serial.println(WiFi.channel());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init feilet");
    return;
  }
//-------------- SINUSGENERATOREN ------------
  // Hardware 50kHz Sine Generator
  dac_cw_config_t cw_config = {
    .en_ch = DAC_CHANNEL_1,
    .scale = DAC_CW_SCALE_2,
    .phase = DAC_CW_PHASE_0,
    .freq = 50000
  };

  dac_cw_generator_config(&cw_config);
  dac_cw_generator_enable();
  dac_output_enable(DAC_CHANNEL_1);

  Serial.println("Peak Amplitude Logger Active");
  Serial.println("Avg_Input_V,Avg_Output_V,Gain_Ratio");
  

  esp_now_register_recv_cb(onRecieve);

  Serial.println("suksess med setup");
}

// ----------- LOOP -----------
void loop() {
// ------------ SKRIV UNDER HER ---------------
// her skriver du koden du trenger for å drifte sensoren din
  float totalInputAmp = 0;
  float totalOutputAmp = 0;
  for (int a = 0; a < AVERAGING_COUNT; a++) {
    int maxIn = 0, minIn = 4095;
    int maxOut = 0, minOut = 4095;

    // 1. Capture the "Swing" (Peak-to-Peak)
    for (int i = 0; i < SAMPLES_PER_BURST; i++) {
      int vI = analogRead(REF_ADC_PIN);
      int vO = analogRead(SENSE_ADC_PIN);

      if (vI > maxIn)  maxIn = vI;
      if (vI < minIn)  minIn = vI;

      if (vO > maxOut) maxOut = vO;
      if (vO < minOut) minOut = vO;
    }

    // 2. Amplitude = (Max - Min) / 2
    totalInputAmp += (maxIn - minIn) / 2.0;
    totalOutputAmp += (maxOut - minOut) / 2.0;
  }

  float avgIn = totalInputAmp / AVERAGING_COUNT;
  float avgOut = totalOutputAmp / AVERAGING_COUNT;

  // 3. Convert to Volts
  float vIn = (avgIn * 3.3) / 4095.0;
  float vOut = (avgOut * 3.3) / 4095.0;

  ratio = (vIn > 0.01) ? (vOut / vIn) : 0;
  Serial.println(0.46683 + 98.735867 * ratio);

  delay(1000);
// ------------ SKRIV OVER HER ----------------
}
