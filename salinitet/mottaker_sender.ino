// ----------- RECEIVE CALLBACK -----------
void onRecieve(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  memcpy(&incoming, data, sizeof(incoming));
  Serial.println("REQUEST MOTTATT");

  if (incoming.type == 0) {

    float sal = getSalinity();

    response.type = 1;
    response.salinity = sal;

    // Viktig: legg til peer hvis ikke finnes
    if (!esp_now_is_peer_exist(info->src_addr)) {
      esp_now_peer_info_t peer = {};
      memcpy(peer.peer_addr, info->src_addr, 6);
      peer.channel = 0;
      peer.encrypt = false;
      esp_now_add_peer(&peer);
    }

    esp_now_send(info->src_addr, (uint8_t*)&response, sizeof(response));
  }
}