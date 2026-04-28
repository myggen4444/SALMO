// ----------- LIVE API -----------
void handleData() {
  String json = "{";

  json += "\"temp\":";
  if (isnan(tempCache)) json += "null";
  else json += String(tempCache, 2);
  json += ",";

  json += "\"pressure\":";
  if (isnan(pressureCache)) json += "null";
  else json += String(pressureCache, 2);
  json += ",";

  json += "\"salinity\":";
  if (isnan(salinityCache)) json += "null";
  else json += String(salinityCache, 2);
  json += ",";

  json += "\"time\":" + String(millis());

  json += "}";

  server.send(200, "application/json", json);
}

// ----------- HISTORY API -----------
void handleHistory() {
  String json = "{";

  json += "\"temp\":[";
  for (int i = 0; i < HISTORY_LEN; i++) {
    int idx = (histIndex + i) % HISTORY_LEN;
    if (i > 0) json += ",";
    if (isnan(tempHistory[idx])) json += "null";
    else json += String(tempHistory[idx], 2);
  }
  json += "],";

  json += "\"pressure\":[";
  for (int i = 0; i < HISTORY_LEN; i++) {
    int idx = (histIndex + i) % HISTORY_LEN;
    if (i > 0) json += ",";
    if (isnan(pressureHistory[idx])) json += "null";
    else json += String(pressureHistory[idx], 2);
  }
  json += "],";

  json += "\"salinity\":[";
  for (int i = 0; i < HISTORY_LEN; i++) {
    int idx = (histIndex + i) % HISTORY_LEN;
    if (i > 0) json += ",";
    if (isnan(salinityHistory[idx])) json += "null";
    else json += String(salinityHistory[idx], 2);
  }
  json += "]";

  json += "}";

  server.send(200, "application/json", json);
}

// ----------- FILES -----------
bool sendFile(const char* path, const char* type) {
  if (!LittleFS.exists(path)) {
    server.send(404, "text/plain", "Missing file: " + String(path));
    return false;
  }

  File f = LittleFS.open(path, "r");
  server.streamFile(f, type);
  f.close();
  return true;
}

// -------- SERVERSTART ---------
void server_start() {
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

  server.on("/", []() { sendFile("/index.html", "text/html"); });
  server.on("/script.js", []() { sendFile("/script.js", "application/javascript"); });
  server.on("/style.css", []() { sendFile("/style.css", "text/css"); });

  server.on("/data", handleData);
  server.on("/history", handleHistory);

  server.begin();
}