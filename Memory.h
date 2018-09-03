#include <EEPROM.h>

#define CFG_SIZE 28 //int=2 bytes, 14 ints, therefor 28 bytes

#define END_CFG 140//28*5

#define RESTART_COUNT_ADD END_CFG

//#define DEBUG

extern short light1config[] = {
  0, 0, //monday on-time, off time  0,1
  0, 0, //tuesday on-time, off time 2,3
  0, 0,
  0, 0,
  0, 0,
  0, 0,
  0, 0                               //12, 13
};

extern short light2config[] = {
  0, 0, //monday on-time, off time  0,1
  0, 0, //tuesday on-time, off time 2,3
  0, 0,
  0, 0,
  0, 0,
  0, 0,
  0, 0                               //12, 13
};

extern short light3config[] = {
  0, 0, //monday on-time, off time  0,1
  0, 0, //tuesday on-time, off time 2,3
  0, 0,
  0, 0,
  0, 0,
  0, 0,
  0, 0                               //12, 13
};

extern short light4config[] = {
  0, 0, //monday on-time, off time  0,1
  0, 0, //tuesday on-time, off time 2,3
  0, 0,
  0, 0,
  0, 0,
  0, 0,
  0, 0                               //12, 13
};

extern short light5config[] = {
  0, 0, //monday on-time, off time  0,1
  0, 0, //tuesday on-time, off time 2,3
  0, 0,
  0, 0,
  0, 0,
  0, 0,
  0, 0                              //12, 13
};

extern byte sysRestartCount = -1;

void setupMemory() {
  EEPROM.begin(512);
}

void updateSysRestartCount() {
#ifdef UPDATE_BOOT
  sysRestartCount = EEPROM.read(RESTART_COUNT_ADD) + 1;
  EEPROM.write(RESTART_COUNT_ADD, sysRestartCount );
  EEPROM.commit();
#endif
}

int eeAddress = 0;

void getDataSingle(byte initAdd, byte * cfg) {
  for (eeAddress = 0; eeAddress < CFG_SIZE; eeAddress++) {
    cfg[eeAddress] = EEPROM.read(initAdd + eeAddress);

#ifdef DEBUG
    Serial.print("read ");
    Serial.print(initAdd + eeAddress);
    Serial.print(" -> ");
    Serial.println(cfg[eeAddress]);
#endif /* DEBUG */
  }
}

void saveDataSingle(byte initAdd, byte * cfg) {
  for (eeAddress = 0; eeAddress < CFG_SIZE; eeAddress++) {
    EEPROM.write(initAdd + eeAddress, cfg[eeAddress] );
#ifdef DEBUG
    Serial.print("write ");
    Serial.print(initAdd + eeAddress);
    Serial.print(" -> ");
    Serial.println(cfg[eeAddress]);
#endif /* DEBUG */
  }
}

void getData() {
  getDataSingle(CFG_SIZE * 0, (byte *)light1config);
  getDataSingle(CFG_SIZE * 1, (byte *)light2config);
  getDataSingle(CFG_SIZE * 2, (byte *)light3config);
  getDataSingle(CFG_SIZE * 3, (byte *)light4config);
  getDataSingle(CFG_SIZE * 4, (byte *)light5config);
}

void saveData() {
  saveDataSingle(CFG_SIZE * 0, (byte *)light1config);
  saveDataSingle(CFG_SIZE * 1, (byte *)light2config);
  saveDataSingle(CFG_SIZE * 2, (byte *)light3config);
  saveDataSingle(CFG_SIZE * 3, (byte *)light4config);
  saveDataSingle(CFG_SIZE * 4, (byte *)light5config);
  EEPROM.commit();
  getData();
}


void printEEPROM(byte initAdd) {
  for (eeAddress = 0; eeAddress < CFG_SIZE; eeAddress++) {
    Serial.print(initAdd + eeAddress);
    Serial.print(" -> ");
    Serial.println(EEPROM.read(initAdd + eeAddress));
  }
}


void printData() {
  Serial.print("light 1:");
  for (int i = 0; i < 14; i += 2) {
    Serial.print("["); Serial.print(light1config[i]); Serial.print(","); Serial.print(light1config[i + 1]); Serial.print("] ");
  }
  Serial.println();
  Serial.print("light 2:");
  for (int i = 0; i < 14; i += 2) {
    Serial.print("["); Serial.print(light2config[i]); Serial.print(","); Serial.print(light2config[i + 1]); Serial.print("] ");
  }
  Serial.println();
  Serial.print("light 3:");
  for (int i = 0; i < 14; i += 2) {
    Serial.print("["); Serial.print(light3config[i]); Serial.print(","); Serial.print(light3config[i + 1]); Serial.print("] ");
  }
  Serial.println();
  Serial.print("light 4:");
  for (int i = 0; i < 14; i += 2) {
    Serial.print("["); Serial.print(light4config[i]); Serial.print(","); Serial.print(light4config[i + 1]); Serial.print("] ");
  }
  Serial.println();
  Serial.print("light 5:");
  for (int i = 0; i < 14; i += 2) {
    Serial.print("["); Serial.print(light5config[i]); Serial.print(","); Serial.print(light5config[i + 1]); Serial.print("] ");
  }
  Serial.println(); Serial.println();
}

