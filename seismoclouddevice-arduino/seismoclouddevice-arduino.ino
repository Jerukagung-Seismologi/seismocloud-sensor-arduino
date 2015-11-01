// ********** CONFIGURABLE PART

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte ethernetMac[] = {
//  0, 0, 0, 0, 0, 0
  0x02, 0xAB, 0xCD, 0x02, 0xAB, 0xCD
};


// ********** END CONFIGURABLE PART

// DO NOT EDIT BEYOND THIS LINE WHEN CONFIGURING DEVICE
// (unless you're contibuting ;-) )


#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include "common.h"

unsigned long lastAliveMs = 0;
unsigned long lastProbeMs = 0;
uint32_t probeCount = 0;

void setup() {
  // start serial port:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println(F("Booting SeismoCloudDevice-Arduino sketch"));

  if(isZero(ethernetMac, 6)) {
    Serial.println(F("No MAC Address configured - please edit sketch file"));
    while(true);
  }

  // Check config and set UUID and lat/lon
  loadConfig();

  if(isZero(getUuidNumber(), 16)) {
    byte uuidNumber[16];
    TrueRandom.uuid(uuidNumber);
    // Save UUID
    setUuidNumber(uuidNumber);
    Serial.print(F("No device id - generating a new one: "));
    printUuid(uuidNumber);
    Serial.println("");
  } else {
    Serial.print(F("Using device id: "));
    printUuid(getUuidNumber());
    Serial.println("");
  }

  // give the ethernet module time to boot up:
  delay(1000);
  
  Ethernet.begin(ethernetMac);
  
  // print the Ethernet board/shield's IP address:
  Serial.print(F("My IP address: "));
  Serial.println(Ethernet.localIP());

  Serial.println(F("Updating NTP Time"));
  do {
    updateNTP();
    setBootTime(getUNIXTime());
    if(getBootTime() == 0) {
      Serial.println(F("NTP update failed, retrying in 5 seconds..."));
      delay(5 * 1000);
    }
  } while(getBootTime() == 0);

  Serial.print(F("Local time is "));
  Serial.println(getBootTime());

  Serial.println(F("Init command interface"));
  commandInterfaceInit();

  Serial.println(F("Send first keep-alive to server..."));
  httpAliveRequest();
  lastAliveMs = millis();
  Serial.println(F("Keep-alive sent"));

  if(getLatitude() == 0 && getLongitude() == 0) {
    Serial.println(F("No position available, waiting for GPS from phone App"));
    do {
      commandInterfaceTick();
    } while(getLatitude() == 0 && getLongitude() == 0);
    Serial.print(F("New position: lat:"));
    Serial.print(getLatitude());
    Serial.print(F(" lon:"));
    Serial.println(getLongitude());
  }

  Serial.println(F("Init seismometer and calibrate"));
  seismometerInit();

  Serial.println(F("Send keep-alive to server..."));
  httpAliveRequest();
  lastAliveMs = millis();
  Serial.println(F("Keep-alive sent, boot completed"));
}

void loop() {
  // Update NTP (if necessary)
  updateNTP();

  commandInterfaceTick();

  // Calling alive every 14 minutes
  if(millis() - lastAliveMs >= 1000 * 60 * 14) {
    Serial.println(F("Keepalive"));
    httpAliveRequest();
    lastAliveMs = millis();
  }

  // Detection
  seismometerTick();
  if(millis() - lastProbeMs >= 1000) {
    lastProbeMs = millis();
    setProbeSpeedStatistic(probeCount);
    probeCount = 0;
  }
  probeCount++;
}

