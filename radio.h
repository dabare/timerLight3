#include "cc1101.h"
#include "ccpacket.h"
#include "Globals.h"

#define CC1101Interrupt 34 //esp32


CC1101 radio;

byte syncWord[2] = {199, 10};
bool packetWaiting;

unsigned long lastSend = 0;
unsigned int sendDelay = 50;


CCPACKET packet;

void messageReceived() {
  packetWaiting = true;
}

void setupRadio() {
  radio.init();
  radio.setSyncWord(syncWord);
  radio.setCarrierFreq(CFREQ_433);
  radio.setDevAddress(DEVICE_ID);
  //radio.disableAddressCheck();
  radio.enableAddressCheck();
  radio.setTxPowerAmp(PA_LongDistance);

  Serial.print(F("CC1101_PARTNUM "));
  Serial.println(radio.readReg(CC1101_PARTNUM, CC1101_STATUS_REGISTER));
  Serial.print(F("CC1101_VERSION "));
  Serial.println(radio.readReg(CC1101_VERSION, CC1101_STATUS_REGISTER));
  Serial.print(F("CC1101_MARCSTATE "));
  Serial.println(radio.readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1f);

  Serial.println(F("CC1101 radio initialized."));
  attachInterrupt(CC1101Interrupt, messageReceived, FALLING);
}

// Get signal strength indicator in dBm.
// See: http://www.ti.com/lit/an/swra114d/swra114d.pdf
int rssi(char raw) {
  uint8_t rssi_dec;
  // TODO: This rssi_offset is dependent on baud and MHz; this is for 38.4kbps and 433 MHz.
  uint8_t rssi_offset = 74;
  rssi_dec = (uint8_t) raw;
  if (rssi_dec >= 128)
    return ((int)( rssi_dec - 256) / 2) - rssi_offset;
  else
    return (rssi_dec / 2) - rssi_offset;
}

// Get link quality indicator.
int lqi(char raw) {
  return 0x3F - raw;
}


void sendPacket(CCPACKET packet) {
  unsigned long now = millis();
  if (now > lastSend + sendDelay) {
    detachInterrupt(CC1101Interrupt);
    lastSend = now;
    radio.sendData(packet);
    attachInterrupt(CC1101Interrupt, messageReceived, FALLING);
  }
}

bool checkRadio(byte * relay[], int len) {
  bool ret = LOW;
  if (packetWaiting) {
    detachInterrupt(CC1101Interrupt);
    packetWaiting = false;
    if (radio.receiveData(&packet) > 0) {
      if (packet.crc_ok && packet.length > 0) {
        ret = HIGH;
        Serial.println("Radio received a packet");
        //data[0] is my id
        //data[1] is remote id

        packet.data[0] = packet.data[1];//when transmitting to the remote data[0] is destination id
        packet.data[1] = DEVICE_ID;
        //packet.data[2] is opperation
        packet.length = len + 3;
        switch (packet.data[2]) {
          case 0://read current config
            for (int i = 0; i < len; i++) {
              packet.data[3 + i] = *relay[i];
            }
            sendPacket(packet);
            break;
          case 1://invert light
            *relay[packet.data[3]] ^= 1;
            for (int i = 0; i < len; i++) {
              packet.data[3 + i] = *relay[i];
            }
            sendPacket(packet);
            break;
          case 2:
            for (int i = 0; i < len; i++) {
              *relay[i] = packet.data[3 + i];
            }
            sendPacket(packet);
            break;
          default:
            break;
        }
      }
    }
    attachInterrupt(CC1101Interrupt, messageReceived, FALLING);
  }
  return ret;
}



