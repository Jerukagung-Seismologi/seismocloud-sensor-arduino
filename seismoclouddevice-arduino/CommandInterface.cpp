
#include <Arduino.h>
#include "common.h"

byte udpPacketBuffer[PACKET_SIZE];
EthernetUDP cmdsock;

void commandInterfaceInit() {
  cmdsock.begin(62001);
}

void commandInterfaceTick() {
  int packetSize = cmdsock.parsePacket();
  if(cmdsock.available()) {
    
    // read the packet into packetBufffer
    cmdsock.read(udpPacketBuffer, PACKET_SIZE);

    if(memcmp("INGV\0", udpPacketBuffer, 5) != 0) {
      Serial.println(F("Invalid magic, dropping packet"));
      return;
    }

    bool reboot = false;
    unsigned long unixTimeM = getUNIXTime();
    unsigned long uptime = getUNIXTime() - getBootTime();
    byte macaddress[6] = { 0 };
    getMACAddress(macaddress);
    uint32_t probeSpeed = getProbeSpeedStatistic();
    uint32_t freeramkb = freeMemory();

    float longitude = 0;
    float latitude = 0;

    switch(udpPacketBuffer[5]) {
      case PKTTYPE_DISCOVERY:
        // Reply to discovery
        udpPacketBuffer[5] = PKTTYPE_DISCOVERY_REPLY;

        memcpy(udpPacketBuffer + 6, macaddress, 6);
        
        memcpy(udpPacketBuffer + 12, "1.00", 4);
        memcpy(udpPacketBuffer + 16, "uno", 3);
        break;
      case PKYTYPE_PING:
        // Reply to ping
        udpPacketBuffer[5] = PKYTYPE_PONG;
        break;
      case PKTTYPE_SENDGPS:
        // Get coords
        udpPacketBuffer[5] = PKTTYPE_OK;

        memcpy(&latitude, udpPacketBuffer + 12, 4);
        memcpy(&longitude, udpPacketBuffer + 16, 4);
        reverse4bytes((byte*)&latitude);
        reverse4bytes((byte*)&longitude);
        
        Serial.print(F("Settings GPS coordinates: lat "));
        Serial.print(latitude);
        Serial.print(F(" lon "));
        Serial.println(longitude);
        
        break;
      case PKTTYPE_REBOOT:
        // Reboot
        // Reply with OK
        udpPacketBuffer[5] = PKTTYPE_OK;
        reboot = true;
        break;
      case PKTTYPE_GETINFO:
        udpPacketBuffer[5] = PKTTYPE_GETINFO_REPLY;

        memcpy(udpPacketBuffer + 6, macaddress, 6);
        memcpy(udpPacketBuffer + 28, &uptime, 4);
        memcpy(udpPacketBuffer + 32, &unixTimeM, 4);
        memcpy(udpPacketBuffer + 36, "1.00", 4);
        memcpy(udpPacketBuffer + 40, &freeramkb, 4);
        memcpy(udpPacketBuffer + 53, "uno", 3);
        memcpy(udpPacketBuffer + 57, "MMA7361", 7);
        memcpy(udpPacketBuffer + 65, &probeSpeed, 4);

        break;
      default:
        Serial.println(F("Unknown command"));
        // Unknown packet or invalid command
        return;
    }

    if(longitude != 0 && latitude != 0) {
      setLongitude(longitude);
      setLatitude(latitude);
    }

    cmdsock.beginPacket(cmdsock.remoteIP(), cmdsock.remotePort());
    cmdsock.write(udpPacketBuffer, PACKET_SIZE);
    cmdsock.endPacket();
    cmdsock.flush();

    if(reboot) {
      soft_restart();
    }
  }
}

