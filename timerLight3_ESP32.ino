#include "WiFi.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("booting device -> done.");

  setupDisplay();
  Serial.println("initlializing display -> done.");
  setupMemory();
  Serial.println("initlializing memory -> done.");
  setupSwitch();
  Serial.println("initlializing switchs -> done.");
  setupRelay();
  Serial.println("initlializing relays -> done.");
  setupWiFi();
  Serial.println("initlializing wifi -> done.");
  setupRadio();
  Serial.println("initlializing radio -> done.");
  getData();
  Serial.println("reading memory -> done.");
  setupTime();
  Serial.println("initlializing time deamon-> done.");
  Serial.print("Current System Time: ");
  printTime();

  Serial.println("saved config:");
  printData();

  updateSysRestartCount();

  Serial.printf("System restarted %d times\n\n", sysRestartCount);

  xTaskCreate(blynkDeamon, "blynkDeamon", 10000,  NULL, 1, NULL);
  Serial.println("initlializing blynk deamon-> done.");

  xTaskCreate(WiFiDeamon, "WiFiDeamon", 10000,  NULL, 1, NULL);
  Serial.println("initlializing WiFi deamon-> done.");

  xTaskCreate(mainTask, "mainTask", 10000,  NULL, 1, NULL);
  Serial.println("initlializing mainTask deamon-> done.");

  xTaskCreate(ntpDeamon, "ntpDeamon", 10000,  NULL, 1, NULL);
  Serial.println("initlializing ntp deamon-> done.");
}

int timeDeamonCount = 0;
void loop() {
  while (1) {
    if (timeDeamonCount++ == 90000) {
      readTime();
      printTime();
      timeDeamonCount = 0;
    }
    listenButton();
    //delay(1);
  }
}

void mainTask(void *p ) {
  resetRuleAppliedLight();
  while (1) {
    wifiMode();
    manualMode();
    autoMode();
  }
}

void ntpDeamon(void * p) {
  while (1) {
    if (wifiStat == WIFI_STA) {
      updateLocalTime();
    }
    delay(2000);
  }
}

void blynkDeamon(void * p) {
  setupBlynk();
  while (1) {
    if (wifiStat == WIFI_STA) {
      Blynk.run();
    }
    delay(2000);
  }
}

void manualMode() {
  if (getSysMode() == 0) {
    Serial.println("\nManual Mode");
    stopWiFi();
  }
  while (getSysMode() == 0) {
    inidi = LOW;
    delay(1000);
  }
}


bool setRuleLight1 = HIGH, setRuleLight2 = HIGH, setRuleLight3 = HIGH, setRuleLight4 = HIGH, setRuleLight5 = HIGH;
bool ruleAppliedLight1 = LOW; bool ruleAppliedLight2 = LOW; bool ruleAppliedLight3 = LOW; bool ruleAppliedLight4 = LOW; bool ruleAppliedLight5 = LOW;

byte setRulesDOW = 0; // day of week temp variable
void setRules(short * conf, bool * ruleApplied , bool * setRule, byte * light) {
  //currentTime.DayOfWeek() returns 1-mon 2-tue ..... 0-sun , but we have monday first and sunday last
  setRulesDOW = DOW;
  if (setRulesDOW == 0) setRulesDOW = 7;
  setRulesDOW--;

  if (!(*ruleApplied)) {
    if (conf[(setRulesDOW) * 2] < conf[(setRulesDOW) * 2 + 1]) { //on-time < off-time
      if (fullTime >= conf[(setRulesDOW) * 2] && fullTime < conf[(setRulesDOW) * 2 + 1]) {
        *light = HIGH;
      } else {
        *light = LOW;
      }
    } else if (conf[(setRulesDOW) * 2] != conf[(setRulesDOW) * 2 + 1]) {
      if (fullTime >= conf[(setRulesDOW) * 2] || fullTime < conf[(setRulesDOW) * 2 + 1]) {
        *light = HIGH;
      } else {
        *light = LOW;
      }
    }
    *ruleApplied = HIGH;
  }

  //reset rule when time comes
  if (conf[(setRulesDOW) * 2] == fullTime | conf[(setRulesDOW) * 2 + 1] == fullTime) {
    if (*setRule) {
      *setRule = LOW;
      *ruleApplied = LOW;
    }
  } else {
    *setRule = HIGH;
    *ruleApplied = HIGH;
  }
}

void listenButton() {
  //user interrupt using button
  btn_1.Update(); btn_2.Update(); btn_3.Update(); btn_4.Update(); btn_5.Update();
  if (btn_1.quickPressed) {
    r1 ^= 1;
  }
  if (btn_2.quickPressed) {
    r2 ^= 1;
  }
  if (btn_3.quickPressed) {
    r3 ^= 1;
  }
  if (btn_4.quickPressed) {
    r4 ^= 1;
  }
  if (btn_5.quickPressed) {
    r5 ^= 1;
  }

  //user interrupt using remote radio
  checkRadio(relays, NUM_RELAYS);

  //set values
  setLights();
}

void checkAndSetLights() {
  if (timeUpdated) {
    resetRuleAppliedLight();
  }

  //apply rules
  setRules(light1config, &ruleAppliedLight1, &setRuleLight1, &r1);
  setRules(light2config, &ruleAppliedLight2, &setRuleLight2, &r2);
  setRules(light3config, &ruleAppliedLight3, &setRuleLight3, &r3);
  setRules(light4config, &ruleAppliedLight4, &setRuleLight4, &r4);
  setRules(light5config, &ruleAppliedLight5, &setRuleLight5, &r5);
}

void resetRuleAppliedLight() {
  ruleAppliedLight1 = LOW; ruleAppliedLight3 = LOW; ruleAppliedLight4 = LOW; ruleAppliedLight5 = LOW; ruleAppliedLight2 = LOW;
}

void autoMode() {
  if (getSysMode() == 1) {
    Serial.println("\nAuto Mode");
    stopWiFi();
    resetRuleAppliedLight();
  }
  while (getSysMode() == 1) {
    inidi = HIGH;
    checkAndSetLights();
  }
}

int wifiModeBlink = 0;
void wifiMode() {
  if (getSysMode() == 2) {
    Serial.println("\nWiFi Mode");
    startWiFi();
  }
  while (getSysMode() == 2) {
    if (wifiStat == WIFI_OFF && wifiModeBlink % 2000 == 0) {
      inidi ^= 1;
    } else if (wifiStat == WIFI_STA && wifiModeBlink % 20000 == 0) {
      inidi ^= 1;
    }

    checkAndSetLights();
    listenOTA();
    wifiModeBlink++;
  }
}


void WiFiDeamon(void * p) {
  while (1) {
    if (getSysMode() == 2) {
      connectTOWifi();
      delay(1);
    } else {
      delay(1000);
    }
  }
}

