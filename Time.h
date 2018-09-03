#include "Globals.h"
#include "time.h"
#include "DS3231RTC.h"

const char* ntpServer = "pool.ntp.org";

const long  gmtOffset_sec = 55 * 6 * 60;
const int   daylightOffset_sec = 0;

struct tm timeinfo;

short fullTime = 0;
bool timeUpdated = false;

void readTime();

char sysTime[30];
char workingSince[30];

char *days[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

byte DOW = 0, HOUR = 0, MINUTE = 0;

tmElements_t t = {};
/*
   typedef struct  {
  uint8_t Second;
  uint8_t Minute;
  uint8_t Hour;
  uint8_t Wday;   // day of week, sunday is day 1
  uint8_t Day;
  uint8_t Month;
  uint8_t Year;   // offset from 1970;
  }  tmElements_t, TimeElements, *tmElementsPtr_t;

*/
void printTime();
void readTime(char * copyto);

void setupTime() {
#ifdef DISABLE_TIME
  return;
#endif
  readTime(workingSince);
  readTime();
}

void updateLocalTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  if (getLocalTime(&timeinfo)) {
    if (t.Hour != timeinfo.tm_hour || t.Minute != timeinfo.tm_min || t.Day != timeinfo.tm_mday || t.Month != timeinfo.tm_mon || t.Year != timeinfo.tm_year) {
      t.Hour = timeinfo.tm_hour;
      t.Minute = timeinfo.tm_min;
      t.Second = timeinfo.tm_sec;
      t.Day = timeinfo.tm_mday;
      t.Month = timeinfo.tm_mon;
      t.Year = timeinfo.tm_year;
      t.Wday = timeinfo.tm_wday;
      timeUpdated = true;

      Serial.println("Time synced");
    }
  }
}

void readTime() {
  readTime(sysTime);
  displayMessage(sysTime);
}

void readTime(char copyto[]) {
#ifdef DISABLE_TIME
  return;
#endif
  if (timeUpdated) {
    RTC.write(t);
    printTime();
    timeUpdated = false;
  } else {
    RTC.read(t);
  }
  HOUR = t.Hour;
  MINUTE = t.Minute;
  DOW = t.Wday;
  fullTime = HOUR * 100 + MINUTE;
  sprintf(copyto, "%d-%d-%d %d:%d:%d , %s \0",
          t.Year, t.Month, t.Day,
          HOUR,   //get hour method
          MINUTE, //get minute method
          t.Second,  //get second method
          days[DOW]
         );
}

void setTime(byte h, byte m, byte d) {
  t.Hour = h;
  t.Minute = m;
  t.Wday = d;
  timeUpdated = true;
}


void printTime() {
  Serial.println(sysTime); //print the string to the serial port
}
