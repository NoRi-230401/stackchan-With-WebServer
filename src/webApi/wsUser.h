// ---------------------------< wsUser.h >------------------------------------
#ifndef _WS_USER_H
#define _WS_USER_H
// ---------------------------
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SD.h>
#include <SPIFFS.h>
#include <M5Unified.h>

extern AsyncWebServer server;
extern String IP_ADDR;
extern String SERVER_NAME;
extern uint8_t VOLUME_VALUE;
extern const String SWW_VERSION;
extern String webpage;
extern const String FLS_NAME[];
extern int isSPIFFS;
extern String SdPath;

// ------------------------------------------------------------
void setupUserHandler();

String processor05(const String &var);
void processor00();

void serverSend(AsyncWebServerRequest *request);
void serverSend3(AsyncWebServerRequest *request);
String HTML_Footer();
String HTML_Header();
String HTML_Footer2();
String HTML_Header2();
String HTML_Header2Ng();
String HTML_Header3();
String HTML_Footer3();
void Home();
bool convIP(const String flname);

// ---- end of < WS_USER_H > --------------------------------------
#endif
