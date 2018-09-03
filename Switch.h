#include "clickButton.h"
#include "radio.h"

#define BTN1 14
#define BTN2 12
#define BTN3 13
#define BTN4 15
#define BTN5 4

#define mode1pin 36//16 make this high using a resister, hardware pullup wont work
#define mode2pin 39//17 make this high using a resister, hardware pullup wont work

extern ClickButton btn_1(BTN1, LOW, HIGH);
extern ClickButton btn_2(BTN2, LOW, HIGH);
extern ClickButton btn_3(BTN3, LOW, HIGH);
extern ClickButton btn_4(BTN4, LOW, HIGH);
extern ClickButton btn_5(BTN5, LOW, HIGH);

void setupSwitch() {
  pinMode(mode2pin, INPUT_PULLUP);
  pinMode(mode1pin, INPUT_PULLUP);
  //pinMode(A0, INPUT);
}

int getSysModeTmp = 0;
byte getSysMode() {
#ifdef TEST_WIFI_MODE
  return 2;
#endif
#ifdef TEST_AUTO_MODE
  return 1;
#endif
#ifdef TEST_MANUAL_MODE
  return 0;
#endif

  return ((!(digitalRead(mode2pin))) << 1) | (!digitalRead(mode1pin));
}

