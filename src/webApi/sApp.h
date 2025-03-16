// ---------------------------< sApp.h >------------------------------------
#ifndef _SAPP_H
#define _SAPP_H
// ---------------------------
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SD.h>
#include <SPIFFS.h>
#include <M5Unified.h>

extern AsyncWebServer server;
extern String IP_ADDR;
extern const String SWS_VERSION;
extern String webpage;
extern String SdPath;

extern const String uAPP1_HTML;
extern const String uAPP2_HTML;
extern const String uAPP3_HTML;
extern const String uAPP4_HTML;
extern const String uAPP5_HTML;
extern const String NAME_uAPP1;
extern const String NAME_uAPP2;
extern const String NAME_uAPP3;
extern const String NAME_uAPP4;
extern const String NAME_uAPP5;

// ------------------------------------------------------------
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

// ---- end of < _SAPP_H > --------------------------------------
#endif
