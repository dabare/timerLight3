#include "Memory.h"
#include "Relay.h"
#include "Time.h"
#include "blynk.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


const char* local_host = "home";
const char *ssid = "Dabare LAN";
const char *password = "@310DqcrgHm64^^";
//const char *password = "thereisnospoon";

byte wifiStat ;
//const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
//DNSServer dnsServer;
AsyncWebServer webServer(80);

//http://blynk-cloud.com/9264ce164737490ebbfa591990388136/update/V1?value=255
//http://blynk-cloud.com/9264ce164737490ebbfa591990388136/get/V1

void setupOTA() {
  //ArduinoOTA.setHostname("dabarehome");

  // No authentication by default
  ArduinoOTA.setPassword(password);

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
}

void listenOTA() {
  ArduinoOTA.handle();
}

void startServing() {
  //dnsServer.processNextRequest();
}

void connectTOWifi() {
  //return true;
  if (WiFi.status() != WL_CONNECTED) {
    //WiFi.reconnect();
    WiFi.disconnect(true);
    delay(1000);
    WiFi.begin(ssid, password);
    WiFi.mode(WIFI_STA);
    wifiStat = WIFI_OFF;
  } else {
    wifiStat = WIFI_STA;
    return;
  }
  for (byte i = 0; i < 40 && (WiFi.status() != WL_CONNECTED); i++) {
    //Serial.print(".");
    delay(500);
  }
  if (WiFi.status() == WL_CONNECTED) {
    MDNS.begin(local_host);
  }
}

void startWiFi() {
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  return;
  Serial.println("Trying to connect to AP");
  for (byte i = 0; i < 20 && (WiFi.status() != WL_CONNECTED); i++) {
    Serial.print(".");
    delay(500);
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected :)");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    wifiStat = WIFI_STA;
  } else {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    Serial.println("Failed to connect :(");
    Serial.println("AccessPoint created");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    wifiStat = WIFI_AP;
  }
}
void stopWiFi() {
  //WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void sleepWiFi() {
  //  WiFi.forceSleepBegin(0);
}
void wakeWiFi() {
  // WiFi.forceSleepWake();
}

void redirectRoot(AsyncWebServerRequest *request) {
  //request->sendHeader("Location", String("/"), true);
  //request->send(302, "text/plain", "");
  request->redirect("/");
}

void handleRoot(AsyncWebServerRequest *request) {
  request->send(200, "text/html", "<!DOCTYPE html><html><head><title>Dabare Home Automation</title> <style>.onoffswitch{position: relative; width: 90px; -webkit-user-select:none; -moz-user-select:none; -ms-user-select: none;}.onoffswitch-checkbox{display: none;}.onoffswitch-label{display: block; overflow: hidden; cursor: pointer; border: 2px solid #999999; border-radius: 20px;}.onoffswitch-inner{display: block; width: 200%; margin-left: -100%; transition: margin 0.3s ease-in 0s;}.onoffswitch-inner:before, .onoffswitch-inner:after{display: block; float: left; width: 50%; height: 22px; padding: 0; line-height: 22px; font-size: 14px; color: white; font-family: Trebuchet, Arial, sans-serif; font-weight: bold; box-sizing: border-box;}.onoffswitch-inner:before{content: 'ON'; padding-left: 10px; background-color: #3EC245; color: #FFFFFF;}.onoffswitch-inner:after{content: 'OFF'; padding-right: 10px; background-color: #EEEEEE; color: #999999; text-align: right;}.onoffswitch-switch{display: block; width: 18px; margin: 2px; background: #FFFFFF; position: absolute; top: 0; bottom: 0; right: 64px; border: 2px solid #999999; border-radius: 20px; transition: all 0.3s ease-in 0s;}.onoffswitch-checkbox:checked + .onoffswitch-label .onoffswitch-inner{margin-left: 0;}.onoffswitch-checkbox:checked + .onoffswitch-label .onoffswitch-switch{right: 0px;}</style></head><body><h1>Time is in 24h form.</h1><form action='ST'><h2>Current Time</h2><h3><input type='number' id='H' min='0' max='23'>:<input type='number' id='M' min='0' max='59'><select id='D'><option value='1'>MONDAY</option><option value='2'>TUESDAY</option><option value='3'>WEDNESDAY</option><option value='4'>THURSDAY</option><option value='5'>FRIDAY</option><option value='6'>SATURDAY</option><option value='7'>SUNDAY</option></select><input type='submit' value='Save'></h3></form><form action='ST'><h2>Remote</h2><table><thead><tr><td><center><b>Switch</b></center></td><td><center><b>Status</b></center></td></tr></thead><tbody><tr><td><center><b>Switch 01</b></center></td><td><center><div class='onoffswitch'><input type='checkbox' name='onoffswitch' class='onoffswitch-checkbox' id='S1' onclick='SW(this);'> <label class='onoffswitch-label' for='S1'><span class='onoffswitch-inner'></span><span class='onoffswitch-switch'></span></label></div></center></td></tr><tr><td><center><b>Switch 02</b></center></td><td><center><div class='onoffswitch'><input type='checkbox' name='onoffswitch' class='onoffswitch-checkbox' id='S2' onclick='SW(this);'> <label class='onoffswitch-label' for='S2'><span class='onoffswitch-inner'></span><span class='onoffswitch-switch'></span></label></div></center></td></tr><tr><td><center><b>Switch 03</b></center></td><td><center><div class='onoffswitch'><input type='checkbox' name='onoffswitch' class='onoffswitch-checkbox' id='S3' onclick='SW(this);'> <label class='onoffswitch-label' for='S3'><span class='onoffswitch-inner'></span><span class='onoffswitch-switch'></span></label></div></center></td></tr><tr><td><center><b>Switch 04</b></center></td><td><center><div class='onoffswitch'><input type='checkbox' name='onoffswitch' class='onoffswitch-checkbox' id='S4' onclick='SW(this);'> <label class='onoffswitch-label' for='S4'><span class='onoffswitch-inner'></span><span class='onoffswitch-switch'></span></label></div></center></td></tr><tr><td><center><b>Switch 05</b></center></td><td><center><div class='onoffswitch'><input type='checkbox' name='onoffswitch' class='onoffswitch-checkbox' id='S5' onclick='SW(this);'> <label class='onoffswitch-label' for='S5'><span class='onoffswitch-inner'></span><span class='onoffswitch-switch'></span></label></div></center></td></tr></tbody></table> </form><br><br><form action='SC'><h2>Lights Configuration</h2><h4>Select Light To Configure <select id='L' onchange='C()'><option value='1'>Light 1</option><option value='2'>Light 2</option><option value='3'>Light 3</option><option value='4'>Light 4</option><option value='5'>Light 5</option></select></h4><br><table><thead><tr><td><center><b>DAY</b></center></td><td><center><b>ON Time</b></center></td><td><center><b>OFF Time</b></center></td></tr></thead><tbody><tr></tr><tr><td>MONDAY</td><td><input type='number' id='MNH' min='0' max='23'>:<input type='number' id='MNM' min='0' max='59'></td><td><input type='number' id='MFH' min='0' max='23'>:<input type='number' id='MFM' min='0' max='59'></td></tr><tr><td>TUESDAY</td><td><input type='number' id='TNH' min='0' max='23'>:<input type='number' id='TNM' min='0' max='59'></td><td><input type='number' id='TFH' min='0' max='23'>:<input type='number' id='TFM' min='0' max='59'></td></tr><tr><td>WEDNESDAY</td><td><input type='number' id='WNH' min='0' max='23'>:<input type='number' id='WNM' min='0' max='59'></td><td><input type='number' id='WFH' min='0' max='23'>:<input type='number' id='WFM' min='0' max='59'></td></tr><tr><td>THURSDAY</td><td><input type='number' id='HNH' min='0' max='23'>:<input type='number' id='HNM' min='0' max='59'></td><td><input type='number' id='HFH' min='0' max='23'>:<input type='number' id='HFM' min='0' max='59'></td></tr><tr><td>FRIDAY</td><td><input type='number' id='FNH' min='0' max='23'>:<input type='number' id='FNM' min='0' max='59'></td><td><input type='number' id='FFH' min='0' max='23'>:<input type='number' id='FFM' min='0' max='59'></td></tr><tr><td>SATURDAY</td><td><input type='number' id='ANH' min='0' max='23'>:<input type='number' id='ANM' min='0' max='59'></td><td><input type='number' id='AFH' min='0' max='23'>:<input type='number' id='AFM' min='0' max='59'></td></tr><tr><td>SUNDAY</td><td><input type='number' id='SNH' min='0' max='23'>:<input type='number' id='SNM' min='0' max='59'></td><td><input type='number' id='SFH' min='0' max='23'>:<input type='number' id='SFM' min='0' max='59'></td></tr><tr></tr><tr><td></td><td></td><td><input type='submit' value='Save'></td></tr></tbody></table></form><form action='S' id='X'><input name='S' id='S' type='hidden'></form></body><script>var J; function K(){var L=document.getElementById('L'); L.name=L.id; J=null; var xhttp=new XMLHttpRequest(); xhttp.onreadystatechange=function(){if (this.readyState==4){J=JSON.parse(this.responseText); for (var p in J){var z=document.getElementById(p); z.name=z.id; z.value=J[p];}}};xhttp.open('GET', 'L'+L.value, true);xhttp.send();}K();function C(){for (var p in J){document.getElementById(p).value=0;}K();}function T(){var xhttp=new XMLHttpRequest(); xhttp.onreadystatechange=function(){if (this.readyState==4){var T=JSON.parse(this.responseText); for (var p in T){var z=document.getElementById(p); z.value=T[p]; z.name=z.id; z.selectedIndex=T[p] - 1;}}};xhttp.open('GET', 'T', true);xhttp.send();}T();setInterval(T, 10000); function LI(){var xhttp=new XMLHttpRequest(); xhttp.onreadystatechange=function(){if (this.readyState==4){var T=JSON.parse(this.responseText);for (var p in T){document.getElementById(p).checked=T[p];}}};xhttp.open('GET', 'L', true);xhttp.send();}LI();setInterval(LI, 3000);function SW(c){var r=c.id.split('');document.getElementById('S').value=r[1];document.getElementById('X').submit();}</script></html>");
}


short * confg;
byte hsct = 0, hsct2 = 0;
void handleSaveConfig(AsyncWebServerRequest *request) {
  hsct = request->arg("L").toInt();
  hsct --;

  switch (hsct) {
    case 0:
      confg = light1config;
      break;
    case 1:
      confg = light2config;
      break;
    case 2:
      confg = light3config;
      break;
    case 3:
      confg = light4config;
      break;
    case 4:
      confg = light5config;
      break;
    default:
      return;
  }

  hsct2 = 0;

  confg[hsct2++] = request->arg("MNH").toInt() * 100 +  request->arg("MNM").toInt();
  confg[hsct2++] = request->arg("MFH").toInt() * 100 +  request->arg("MFM").toInt();
  confg[hsct2++] = request->arg("TNH").toInt() * 100 +  request->arg("TNM").toInt();
  confg[hsct2++] = request->arg("TFH").toInt() * 100 +  request->arg("TFM").toInt();
  confg[hsct2++] = request->arg("WNH").toInt() * 100 +  request->arg("WNM").toInt();
  confg[hsct2++] = request->arg("WFH").toInt() * 100 +  request->arg("WFM").toInt();
  confg[hsct2++] = request->arg("HNH").toInt() * 100 +  request->arg("HNM").toInt();
  confg[hsct2++] = request->arg("HFH").toInt() * 100 +  request->arg("HFM").toInt();
  confg[hsct2++] = request->arg("FNH").toInt() * 100 +  request->arg("FNM").toInt();
  confg[hsct2++] = request->arg("FFH").toInt() * 100 +  request->arg("FFM").toInt();
  confg[hsct2++] = request->arg("ANH").toInt() * 100 +  request->arg("ANM").toInt();
  confg[hsct2++] = request->arg("AFH").toInt() * 100 +  request->arg("AFM").toInt();
  confg[hsct2++] = request->arg("SNH").toInt() * 100 +  request->arg("SNM").toInt();
  confg[hsct2++] = request->arg("SFH").toInt() * 100 +  request->arg("SFM").toInt();

  saveDataSingle(CFG_SIZE * hsct, (byte *)confg);
  EEPROM.commit();
  redirectRoot(request);
  printData();
}
String out = "";
int handleGetTimeTmp = 0;
void handleGetTime(AsyncWebServerRequest *request) {
  out = "{\"H\":";
  handleGetTimeTmp = HOUR;
  out += String(handleGetTimeTmp);
  out += ",\"M\":";
  handleGetTimeTmp = MINUTE;
  out += String(handleGetTimeTmp);
  out += ",\"D\":";
  handleGetTimeTmp = DOW;
  if (handleGetTimeTmp == 0) handleGetTimeTmp = 7;
  out += String(handleGetTimeTmp);
  out += "}";
  request->send(200, "text/html", out);
}

void handleSaveTime(AsyncWebServerRequest *request) {
  setTime(request->arg("H").toInt(), request->arg("M").toInt(), request->arg("D").toInt());
  redirectRoot(request);
}

int getLTmp = 0;
void getL(AsyncWebServerRequest *request, byte p) {

  switch (p) {
    case 0:
      confg = light1config;
      break;
    case 1:
      confg = light2config;
      break;
    case 2:
      confg = light3config;
      break;
    case 3:
      confg = light4config;
      break;
    case 4:
      confg = light5config;
      break;
    default:
      return;
  }
  p = 0;

  out = "{\"MNH\":";
  out += String(confg[p] / 100);
  out += ",\"MNM\":";
  out += String(confg[p++] % 100);
  out += ",\"MFH\":";
  out += String(confg[p] / 100);
  out += ",\"MFM\":";
  out += String(confg[p++] % 100);
  out += ",\"TNH\":";
  out += String(confg[p] / 100);
  out += ",\"TNM\":";
  out += String(confg[p++] % 100);
  out += ",\"TFH\":";
  out += String(confg[p] / 100);
  out += ",\"TFM\":";
  out += String(confg[p++] % 100);
  out += ",\"WNH\":";
  out += String(confg[p] / 100);
  out += ",\"WNM\":";
  out += String(confg[p++] % 100);
  out += ",\"WFH\":";
  out += String(confg[p] / 100);
  out += ",\"WFM\":";
  out += String(confg[p++] % 100);
  out += ",\"HNH\":";
  out += String(confg[p] / 100);
  out += ",\"HNM\":";
  out += String(confg[p++] % 100);
  out += ",\"HFH\":";
  out += String(confg[p] / 100);
  out += ",\"HFM\":";
  out += String(confg[p++] % 100);
  out += ",\"FNH\":";
  out += String(confg[p] / 100);
  out += ",\"FNM\":";
  out += String(confg[p++] % 100);
  out += ",\"FFH\":";
  out += String(confg[p] / 100);
  out += ",\"FFM\":";
  out += String(confg[p++] % 100);
  out += ",\"ANH\":";
  out += String(confg[p] / 100);
  out += ",\"ANM\":";
  out += String(confg[p++] % 100);
  out += ",\"AFH\":";
  out += String(confg[p] / 100);
  out += ",\"AFM\":";
  out += String(confg[p++] % 100);
  out += ",\"SNH\":";
  out += String(confg[p] / 100);
  out += ",\"SNM\":";
  out += String(confg[p++] % 100);
  out += ",\"SFH\":";
  out += String(confg[p] / 100);
  out += ",\"SFM\":";
  out += String(confg[p] % 100);
  out += "}";

  request->send(200, "text/html", out);
}
//0-27 28-55 56-83 84-111 112-140
void handleGetL1(AsyncWebServerRequest *request) {
  getL(request, 0);
}
void handleGetL2(AsyncWebServerRequest *request) {
  getL(request, 1);
}
void handleGetL3(AsyncWebServerRequest *request) {
  getL(request, 2);
}
void handleGetL4(AsyncWebServerRequest *request) {
  getL(request, 3);
}
void handleGetL5(AsyncWebServerRequest *request) {
  getL(request, 4);
}
void handleGetLightStat(AsyncWebServerRequest *request) {
  out = "{\"S1\":";
  if (r1) {
    out += "true";
  } else {
    out += "false";
  }

  out += ",\"S2\":";
  if (r2) {
    out += "true";
  } else {
    out += "false";
  }

  out += ",\"S3\":";
  if (r3) {
    out += "true";
  } else {
    out += "false";
  }

  out += ",\"S4\":";
  if (r4) {
    out += "true";
  } else {
    out += "false";
  }

  out += ",\"S5\":";
  if (r5) {
    out += "true}";
  } else {
    out += "false}";
  }
  request->send(200, "text/html", out);
}

void handleSwitch(AsyncWebServerRequest *request) {
  switch (request->arg("S").toInt()) {
    case 1://here srats from 1 ;)
      r1 ^= 1;
      break;
    case 2:
      r2 ^= 1;
      break;
    case 3:
      r3 ^= 1;
      break;
    case 4:
      r4 ^= 1;
      break;
    case 5:
      r5 ^= 1;
      break;
  }
  setLights();
  redirectRoot(request);
}

void bootCount(AsyncWebServerRequest *request) {
  request->send(200, "text/html", String(sysRestartCount));
}

void lastReset(AsyncWebServerRequest *request) {
  request->send(200, "text/html", workingSince);
}

void setupWiFi() {
  WiFi.softAP(ssid, password);
  WiFi.mode(WIFI_AP);
  WiFi.begin(ssid, password);
  /*
    Serial.print("Trying to connect to:");
    Serial.println(ssid);
    Serial.print("With Password:");
    Serial.println(password);


    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Failed to connect :(");
      Serial.println("Creating AccessPoint");
      Serial.print("SSID:");
      Serial.println(ssid);
      Serial.print("Password:");
      Serial.println(password);

      WiFi.mode(WIFI_AP);
      // WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
      WiFi.softAP(ssid, password);
    } else {
      Serial.println("WiFi connected :)");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    }
  */

  //dnsServer.setTTL(300);
  //dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  //dnsServer.start(DNS_PORT, "dabare.home.lk", apIP);
  //MDNS.begin("dabarehome");
  //MDNS.addService("http", "tcp", 80);
  Serial.print("Open http://");
  Serial.print(local_host);
  Serial.println(".local/edit to see the file browser");

  webServer.on("/", handleRoot);
  webServer.on("/ST", handleSaveTime);
  webServer.on("/SC", handleSaveConfig);
  webServer.on("/T", handleGetTime);
  webServer.on("/L1", handleGetL1);
  webServer.on("/L2", handleGetL2);
  webServer.on("/L3", handleGetL3);
  webServer.on("/L4", handleGetL4);
  webServer.on("/L5", handleGetL5);
  webServer.on("/L", handleGetLightStat);
  webServer.on("/S", handleSwitch);
  webServer.on("/B", bootCount);
  webServer.on("/W", lastReset);
  webServer.begin();
  setupOTA();
}


