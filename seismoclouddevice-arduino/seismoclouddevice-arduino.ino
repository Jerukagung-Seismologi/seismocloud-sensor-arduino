
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
  
  LED::init(LED_GREEN, LED_YELLOW, LED_RED);
  LED::startupBlink();
  LED::green(false);
  LED::red(false);
  LED::yellow(false);

  Serial.print(F("SeismoCloud-Arduino version "));
  Serial.println(getVersionAsString());

  checkEEPROM();

  Serial.println(F("Init seismometer"));
  seismometerInit();
  firstTimeThresholdCalculation();

  Serial.println(F("Loading config"));
  // Check config, load MAC and lat/lon
  loadConfig();

  byte ethernetMac[6];
  getMACAddress(ethernetMac);

  Serial.print(F("MAC Address: "));
  Serial.println(macToString(ethernetMac).c_str());

  // give the ethernet module time to boot up:
  delay(1000);

  Serial.println(F("Enabling Ethernet"));
  Ethernet.begin(ethernetMac);
  
  // Check Ethernet link
  if(Ethernet.localIP() == INADDR_NONE) {
    Serial.println(F("Ethernet failed to load"));
    while(true) {
      LED::green(true);
      LED::red(true);
      LED::yellow(true);
      delay(400);
    }
  } else {
    Serial.print(F("My IP address: "));
    Serial.println(Ethernet.localIP());
  }

  Serial.println(F("Updating NTP Time"));

  do {
    updateNTP();
    setBootTime(getUNIXTime());
    if(getBootTime() == 0) {
      Serial.println(F("NTP failed, retrying in 5s"));
      delay(5 * 1000);
    }
  } while(getBootTime() == 0);

  Serial.print(F("Local time: "));
  printUNIXTime();
  Serial.println();

  Serial.println(F("Init cmd interface"));
  commandInterfaceInit();

  Serial.println(F("Send keep-alive"));
  httpAliveRequest();
  lastAliveMs = millis();

  if(getLatitude() == 0 && getLongitude() == 0) {
    LED::green(false);
    LED::red(false);
    LED::yellow(false);
    LED::setLedBlinking(LED_YELLOW);

    Serial.println(F("No position set, waiting GPS from phone"));
    do {
      commandInterfaceTick();
      LED::tick();
    } while(getLatitude() == 0 && getLongitude() == 0);
    LED::clearLedBlinking();
    LED::green(true);
    LED::red(true);
    LED::yellow(true);
  }
  
  Serial.println(F("GPS: "));
  Serial.println(getLatitudeAsString());
  Serial.println(getLongitudeAsString());

  Serial.print(F("Boot completed at "));
  printUNIXTime();
  Serial.println();
  LED::startupBlink();
  LED::green(true);
}

void loop() {
  // Update NTP (if necessary)
  updateNTP();

  commandInterfaceTick();
  LED::tick();

  // Calling alive every 14 minutes
  if((millis() - lastAliveMs) >= 840000) {
    Serial.print(F("Keepalive at "));
    printUNIXTime();
    Serial.println();
    
    httpAliveRequest();
    lastAliveMs = millis();
    
    Serial.print(F("ACK at "));
    printUNIXTime();
    Serial.println();
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

