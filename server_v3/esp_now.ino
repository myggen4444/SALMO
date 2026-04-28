// ----------- ESP-NOW RECEIVE -----------
void onEspNowReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  SalinityPacket incoming;
  memcpy(&incoming, data, sizeof(incoming));

  Serial.println("SVAR MOTTATT");

  if (incoming.type == 1) {
    salinityCache = incoming.salinity;
  }
}

// legger til salinitetsarduinoen med adresse og mer
void salinitet_start() {
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, salinityNodeMAC, 6);
  peer.channel = 0; // bruker wifi kanalen, må gjøre dette og sørge for at den andre også gjøre det
  peer.encrypt = false;

  if (esp_now_add_peer(&peer) != ESP_OK) {
    Serial.println("Kunne ikke legge til peer");
  }
}