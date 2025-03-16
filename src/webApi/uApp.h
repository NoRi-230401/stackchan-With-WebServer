// ---------------------------< sApp.h >------------------------------------
#ifndef _UAPP_H
#define _UAPP_H
// ---------------------------
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SD.h>
#include <SPIFFS.h>
#include <M5Unified.h>

extern AsyncWebServer server;
extern String webpage;
extern String webpage;
extern String IP_ADDR;
extern String SERVER_NAME;
extern uint8_t VOLUME_VALUE;

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
void setupSystemAppHandler();
void setupUserAppHandler();
String processorA05(const String &var);
void processorA02();
void handle_uRcv(AsyncWebServerRequest *request);
#define TONE_BTN 1    // 本体ボタンが押下時に音を出す。
#define TONE_EXTCOM 2 // apiコマンド受信時に音を出す。

extern void serverSend(AsyncWebServerRequest *request);
extern void serverSend3(AsyncWebServerRequest *request);
extern String HTML_Footer();
extern String HTML_Header();
extern String HTML_Footer2();
extern String HTML_Header2();
extern String HTML_Header2Ng();
extern String HTML_Header3();
extern String HTML_Footer3();
extern void Home();
extern bool convIP(const String flname);
extern void tone(int);


// ---- end of < _UAPP_H > --------------------------------------
#endif
