// ---------------------------< uApp.cpp >------------------------------------
// --------------------------------------
// define user application in this file
// --------------------------------------
#include "uApp.h"

// --- User App1 to App5 ------
const String uAPP1_HTML = "/uApp1.html";
const String uAPP2_HTML = "/uApp2.html";
const String uAPP3_HTML = "/uApp3.html";
const String uAPP4_HTML = "/uApp4.html";
const String uAPP5_HTML = "/uApp5.html";
const String uSCRIPT_JS = "/uScript.js";
const String uSTYLE_CSS = "/uStyle.css";
const String uICON_GIF = "/uIcon.gif";
const String NAME_uAPP1 = "App1";
const String NAME_uAPP2 = "App2";
const String NAME_uAPP3 = "App3";
const String NAME_uAPP4 = "App4";
const String NAME_uAPP5 = "App5";

void setupUserAppHandler()
{
  // ##### User App #####################################################
  //  --- uAPP1-5 html -----
  server.on("/uApp1.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { convIP("/uApp1.html");  request->send(200, "text/html", webpage); });

  server.on("/uApp2.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { convIP("/uApp2.html"); processorA02();  request->send(200, "text/html", webpage); });

  server.on("/uApp3.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { convIP("/uApp3.html");  request->send(200, "text/html", webpage); });

  server.on("/uApp4.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { convIP("/uApp4.html");  request->send(200, "text/html", webpage); });

  server.on("/uApp5.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/sAppt5.html", String(), false, processorA05); });

  // ----------------------------------------------------------------------
  server.on(uSCRIPT_JS.c_str(), HTTP_GET, [](AsyncWebServerRequest *request)
            { convIP(uSCRIPT_JS); request->send(200,"application/javascript", webpage); });

  server.on(uSTYLE_CSS.c_str(), HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, uSTYLE_CSS.c_str(), "text/css"); });

  server.on(uICON_GIF.c_str(), HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, uICON_GIF.c_str(), "image/gif"); });

  // ----------------------------------------------------------------------
  // ##################### User Button ############################
  server.on("/uRcv", HTTP_GET, [](AsyncWebServerRequest *request)
            { handle_uRcv(request); serverSend(request); });

}

void processorA02()
{
  String findStr1 = "%IP_ADDR%";
  String replacedStr1 = IP_ADDR;
  String findStr2 = "%SERVER_NAME%";
  String replacedStr2 = SERVER_NAME;
  String findStr3 = "%VOLUME_VALUE%";
  String replacedStr3 = String(VOLUME_VALUE, DEC);

  webpage.replace(findStr1, replacedStr1);
  webpage.replace(findStr2, replacedStr2);
  webpage.replace(findStr3, replacedStr3);

  // Serial.println(webpage);
}

String processorA05(const String &var)
{
  // ********************* SAMPLE : 本体の内容をWEBに表示　********************
  // Htmlファイル内の %IP_ADDR% , %SERVER_NAME%, %VOLUME_VALUE% に指定された箇所
  // を書き換える。本体の状態をWEB上で表示することができます。
  // **************************************************************************
  Serial.println(var);
  if (var.equalsIgnoreCase("IP_ADDR"))
  {
    Serial.println(IP_ADDR);
    return IP_ADDR;
  }
  else if (var.equalsIgnoreCase("SERVER_NAME"))
  {
    Serial.println(SERVER_NAME);
    return SERVER_NAME;
  }
  if (var.equalsIgnoreCase("VOLUME_VALUE"))
  {
    int vol = (int)VOLUME_VALUE;
    String vol_str = String(vol, DEC);
    return vol_str;
  }
  return String();
}

void handle_uRcv(AsyncWebServerRequest *request)
{
  Serial.println("handle_uRcv");
  tone(TONE_EXTCOM);
  webpage = "NG";
  int argNo = -1;
  String retMsg;

  String argStr = request->arg("No");
  argNo = argStr.toInt();
  if (argNo <= 0)
  {
    retMsg = "uRcv Error : No = " + argStr;
    Serial.println(retMsg);
    webpage = retMsg;
    return;
  }  
  retMsg = "uRcv : No = " + String(argNo);

  String getMsg;
  String argMsg = request->arg("msg");
  if (argMsg.length() > 0)
  {
    getMsg=argMsg;
    Serial.println(getMsg);
    retMsg += "\n Msg = " + getMsg;
  }

  Serial.println(retMsg);
  webpage = retMsg;
  return;
}

