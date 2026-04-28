// ----------- TRYKK -----------
float readPressure() {
  const int N = 15;

  float minVal = 1e9;
  float maxVal = -1e9;
  float sum = 0;

  // stabiliserer signalet ved å ta gjennomsnittet av maks og min og totaltsnittet
  for (int i = 0; i < N; i++) {
    float raw = analogRead(pressurePin);
    float V = raw * (3.3 / 4095.0);
    V = V * DIVIDER;
    float P = (V - OffSet) * spenning_trykk;
    float value = P * vann_trykk_dybde;

    sum += value;

    if (value < minVal) minVal = value;
    if (value > maxVal) maxVal = value;

    delay(5); // liten stabilisering mellom målinger
  }

  float avg = sum / N;

  // gjennomsnitt av ekstremverdiene + stabilisering via middelverdi
  float filtered = (avg + (minVal + maxVal) / 2.0) / 2.0;

  return round(filtered * 10.0) / 10.0;}

// -------- VERDI-HENTING -----------
void sensor_get_values() {

  // request salinitet!
  salinityPkt.type = 0; // sendetype
  salinityPkt.salinity = 0; // nullverdi siden forespørsel
  esp_now_send(salinityNodeMAC, (uint8_t*)&salinityPkt, sizeof(salinityPkt));

  // temperatur
  if (tempSensorOK) {
      sensors.requestTemperatures();
      tempCache = sensors.getTempC(addr);
   }
  
  // trykk
  pressureCache = readPressure();

  // historiske verdier
  tempHistory[histIndex] = tempCache;
  pressureHistory[histIndex] = pressureCache;
  salinityHistory[histIndex] = salinityCache;

  histIndex = (histIndex + 1) % HISTORY_LEN;

  // printing
  Serial.print("Temp: ");
  Serial.print(tempCache);
  Serial.print("  Pressure: ");
  Serial.print(pressureCache);
  Serial.print("  Salinity: ");
  Serial.println(salinityCache);
}

// --------SENSORSTART ---------
void sensors_start() {  
  // sjekker om temp sensoren funker
  sensors.begin();
  if (sensors.getDeviceCount() > 0) {
    sensors.getAddress(addr, 0);
    tempSensorOK = true;
  }

  // setter riktig lese format for trykksensoren
  analogReadResolution(12);
  analogSetPinAttenuation(pressurePin, ADC_11db);
}