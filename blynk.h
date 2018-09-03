#include <BlynkSimpleEsp32.h>

const char* host = "blynk-cloud.com";
const char* auth = "9264ce164737490ebbfa591990388136";
unsigned int port = 80;


extern char sysTime[30];
BLYNK_READ(V6) {
  // This command writes Arduino's uptime in seconds to Virtual Pin (5)
  Blynk.virtualWrite(V1, r1);
  Blynk.virtualWrite(V2, r2);
  Blynk.virtualWrite(V3, r3);
  Blynk.virtualWrite(V4, r4);
  Blynk.virtualWrite(V5, r5);
  Blynk.virtualWrite(V6, sysTime);
  Serial.println("sent responce to Blynk");
}

// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin V1
BLYNK_WRITE(V1) {
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  Serial.print("got responce from Blynk");
  r1 = pinValue;
}
BLYNK_WRITE(V2) {
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  Serial.print("got responce from Blynk");
  r2 = pinValue;
}
BLYNK_WRITE(V3) {
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  Serial.print("got responce from Blynk");
  r3 = pinValue;
}
BLYNK_WRITE(V4) {
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  Serial.print("got responce from Blynk");
  r4 = pinValue;
}
BLYNK_WRITE(V5) {
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  Serial.print("got responce from Blynk");
  r5 = pinValue;
}

BLYNK_WRITE(V7) {
  TimeInputParam t(param);
  //  int getStartHour()    const { return mStart.hour(); }
  //  int getStartMinute()  const { return mStart.minute(); }
  // int getStartSecond()  const { return mStart.second(); }
  Serial.println(t.getStartSecond());
  Serial.println(t.isWeekdaySelected(0));
}

void setupBlynk(){
  Blynk.config(auth, host, port);
}



