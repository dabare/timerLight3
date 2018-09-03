#include "Switch.h"

#define RELAY_ON 0

#define R1 32
#define R2 33
#define R3 25
#define R4 26
#define R5 27
#define INDI 2

#define NUM_RELAYS 5

extern byte r1 = 0;
extern byte r2 = 0;
extern byte r3 = 0;
extern byte r4 = 0;
extern byte r5 = 0;

extern byte inidi = 0;

extern byte* relays[5] = {&r1, &r2, &r3, &r4, &r5};

void setLights();

void setupRelay() {
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(R3, OUTPUT);
  pinMode(R4, OUTPUT);
  pinMode(R5, OUTPUT);

  pinMode(INDI, OUTPUT);

  for (byte i = 0; i < NUM_RELAYS; i++) {
    *relays[i] = 0;
  }
  setLights();
}

void setLights() {
  digitalWrite(R1, !(r1 ^ RELAY_ON));
  digitalWrite(R2, !(r2 ^ RELAY_ON));
  digitalWrite(R3, !(r3 ^ RELAY_ON));
  digitalWrite(R4, !(r4 ^ RELAY_ON));
  digitalWrite(R5, !(r5 ^ RELAY_ON));

  digitalWrite(INDI, !inidi);
}


