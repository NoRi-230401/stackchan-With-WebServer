// ---------------------------< wsUser.cpp >------------------------------------
#include "wsUser.h"

String SERVER_NAME = "stackchan";
// --- System defined App1 to App5 ------
const String sAPP1_HTML = "/sApp1.html";
const String sAPP2_HTML = "/sApp2.html";
const String sAPP3_HTML = "/sApp3.html";
const String sAPP4_HTML = "/sApp4.html";
const String sAPP5_HTML = "/sApp5.html";
const String sSCRIPT_JS = "/sScript.js";
const String sSTYLE_CSS = "/sStyle.css";
const String sICON_GIF = "/sIcon.gif";
const String NAME_sAPP1 = "Setting";
const String NAME_sAPP2 = "Servo";
const String NAME_sAPP3 = "Remote";
const String NAME_sAPP4 = "Chat";
const String NAME_sAPP5 = "Clock";
const String GITHUB_URL = "https://github.com/NoRi-230401/stackchan-With-WebServer";

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

void setupUserHandler()
{
  // ##### System App #################################################
  //  --- sAPP1-5 html -----
  server.on(sAPP1_HTML.c_str(), HTTP_GET, [](AsyncWebServerRequest *request)
            { convIP(sAPP1_HTML);  request->send(200, "text/html", webpage); });

  server.on(sAPP2_HTML.c_str(), HTTP_GET, [](AsyncWebServerRequest *request)
            { convIP(sAPP2_HTML);  request->send(200, "text/html", webpage); });

  server.on(sAPP3_HTML.c_str(), HTTP_GET, [](AsyncWebServerRequest *request)
            { convIP(sAPP3_HTML);  request->send(200, "text/html", webpage); });

  server.on(sAPP4_HTML.c_str(), HTTP_GET, [](AsyncWebServerRequest *request)
            { convIP(sAPP4_HTML);  request->send(200, "text/html", webpage); });

  server.on(sAPP5_HTML.c_str(), HTTP_GET, [](AsyncWebServerRequest *request)
            { convIP(sAPP5_HTML);  request->send(200, "text/html", webpage); });

  // --------------------------------------------------------------------
  server.on(sSCRIPT_JS.c_str(), HTTP_GET, [](AsyncWebServerRequest *request)
            { convIP(sSCRIPT_JS); request->send(200,"application/javascript", webpage); });

  server.on(sSTYLE_CSS.c_str(), HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, sSTYLE_CSS.c_str(), "text/css"); });

  server.on(sICON_GIF.c_str(), HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, sICON_GIF.c_str(), "image/gif"); });

  // ##### User App #####################################################
  //  --- uAPP1-5 html -----
  server.on("/uApp1.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/sAppt1.html", String(), false, processor05); });

  // server.on("/uApp1.html", HTTP_GET, [](AsyncWebServerRequest *request)
  //           { convIP("/uApp1.html");  request->send(200, "text/html", webpage); });

  server.on("/uApp2.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { convIP("/uApp2.html"); processor00();  request->send(200, "text/html", webpage); });

  server.on("/uApp3.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { convIP("/uApp3.html");  request->send(200, "text/html", webpage); });

  // server.on("/uApp4.html", HTTP_GET, [](AsyncWebServerRequest *request)
  //           { convIP("/uApp4.html");  request->send(200, "text/html", webpage); });

  // server.on("/uApp5.html", HTTP_GET, [](AsyncWebServerRequest *request)
  //           { request->send(SPIFFS, "/sAppt5.html", String(), false, processor05); });

  // ----------------------------------------------------------------------
  server.on(uSCRIPT_JS.c_str(), HTTP_GET, [](AsyncWebServerRequest *request)
            { convIP(uSCRIPT_JS); request->send(200,"application/javascript", webpage); });

  server.on(uSTYLE_CSS.c_str(), HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, uSTYLE_CSS.c_str(), "text/css"); });

  server.on(uICON_GIF.c_str(), HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, uICON_GIF.c_str(), "image/gif"); });
  // ######################################################################
}

void serverSend(AsyncWebServerRequest *request)
{
  if (webpage.equals("NG"))
  {
    webpage = HTML_Header2Ng() + webpage + HTML_Footer2();
    request->send(400, "text/html", webpage);
  }
  else if (webpage.equals("OK"))
  {
    Serial.println("send -> OK");
    request->send(200, "text/plain", String("OK"));
  }
  else
  {
    String tmpPage;
    tmpPage = HTML_Header2() + webpage + HTML_Footer2();
    webpage = tmpPage;
    request->send(200, "text/html", webpage);
  }
}

void serverSend3(AsyncWebServerRequest *request)
{
  if (webpage.equals("NG"))
  {
    webpage = HTML_Header2Ng() + webpage + HTML_Footer2();
    request->send(400, "text/html", webpage);
  }
  else
  {
    String tmpPage;
    tmpPage = HTML_Header3() + webpage + HTML_Footer3();
    webpage = tmpPage;
    request->send(200, "text/html", webpage);
  }
}

bool convIP(const String flname)
{
  // *************************************************************
  // ファイル中の  "http://stackchan/"  を実際のIPアドレスに変換
  const char *findStr = "http://stackchan/";
  // *************************************************************
  webpage = "";

  File fl = SPIFFS.open(flname.c_str(), "r");
  if (!fl)
  {
    fl.close();
    String msg = "Error handleRoot : cannot open " + flname;
    webpage = msg;
    Serial.println(msg);
    return false;
  }

  // *** Buffer確保 ******
  size_t sz = fl.size();
  Serial.println(flname + " :  file size = " + String(sz, DEC));

  char *buff;
  buff = (char *)malloc(sz + 1);
  if (!buff)
  {
    String msg = "ERROR:  Unable to malloc " + String(sz, DEC) + " bytes for app";
    webpage = String(msg);
    Serial.println(msg);
    fl.close();
    return false;
  }

  fl.read((uint8_t *)buff, sz);
  buff[sz] = 0;
  fl.close();

  webpage = String(buff);
  free(buff);

  // ** 本体のIP_ADDRに変換 **
  String replacedStr = "http://" + IP_ADDR + "/";
  webpage.replace(findStr, (const char *)replacedStr.c_str());

  return true;
}

void processor00()
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


String processor05(const String &var)
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

// #############################################################################################
void Home()
{
  webpage = HTML_Header();
  webpage += "<br>";
  webpage += "<img src='sIcon.gif' alt='icon'>";
  webpage += "<h3>[&nbsp;Home&nbsp;]　" + SERVER_NAME + "　IP=" + IP_ADDR + "</h3>";
  webpage += HTML_Footer();
}

// #############################################################################################
String HTML_Header()
{
  String page;
  page = "<!DOCTYPE html>";
  page += "<html lang = 'ja'>";
  page += "<head>";
  page += "<title>Home</title>";
  page += "<base target='_self'>";
  page += "<meta charset='UTF-8'>";
  page += "<meta name='viewport' content='width=device-width,initial-scale=1.0'>";
  page += "<style>";
  page += "@media screen and (max-width: 480px) {img{width:100%;height:auto;}}";
  page += "html {font-size: 62.5%;}";
  page += "body {width:100%;margin-left:auto;margin-right:auto;font-family:Arial,Helvetica,sans-serif;font-size:1.4rem;color:#2f4f4f;background-color:#fffacd;text-align:center;}";
  page += "footer {padding:1.0rem;background-color:cyan;font-size:1.4rem;}";
  page += "table {font-family:arial,sans-serif;border-collapse:collapse;width:80%;}";
  page += "table.center {margin-left:auto;margin-right:auto;}";
  page += "td, th {border:1px solid #dddddd;text-align:left;padding:0.8rem;}";
  page += "tr:nth-child(even) {background-color:#dddddd;}";
  page += "h3 {color:#6ecf12;font-size:1.8rem;font-style:normal;text-align:center;}";
  page += "h4 {color:slateblue;font-size:1.5rem;text-align:left;font-style:oblique;text-align:center;}";
  page += ".center {margin-left:auto;margin-right:auto;}";

  // TOPNAV
  page += ".topnav {overflow: visible;background-color:lightPink;}";
  page += ".topnav a {float:center;color:blue;text-align:center;padding:1.0rem 1.0rem;text-decoration:none;font-size:1.5rem;}";
  page += ".topnav a:hover {background-color:deepskyblue;color:white;}";
  page += ".topnav a.active {background-color:lightblue;color:blue;}";

  // TOPNAV2
  page += ".topnav2 {overflow: visible;background-color:lightcyan;}";
  page += ".topnav2 a {float:center;color:blue;text-align:center;padding:1.0rem 1.0rem;text-decoration:none;font-size:1.5rem;}";
  page += ".topnav2 a:hover {background-color:deepskyblue;color:white;}";
  page += ".topnav2 a.active {background-color:lightblue;color:blue;}";
  page += ".notfound {padding:0.8rem;text-align:center;font-size:1.3rem;}";
  page += ".left {text-align:left;}";
  page += ".medium {font-size:1.9rem;padding:0;margin:0}";
  page += ".ps {font-size:1.4rem;padding:0;margin:0}";
  page += ".sp {background-color:silver;white-space:nowrap;width:2%;}";

  // TOPNAV3
  page += ".topnav3 {overflow: visible;background-color:lightcyan;}";
  page += ".topnav3 a {float:center;color:blue;text-align:center;padding:1.0rem 1.0rem;text-decoration:none;font-size:1.5rem;}";
  page += ".topnav3 a:hover {background-color:deepskyblue;color:white;}";
  page += ".topnav3 a.active {background-color:lightblue;color:blue;}";
  page += ".notfound {padding:0.8rem;text-align:center;font-size:1.3rem;}";
  page += ".left {text-align:left;}";
  page += ".medium {font-size:1.9rem;padding:0;margin:0}";
  page += ".ps {font-size:1.4rem;padding:0;margin:0}";
  page += ".sp {background-color:silver;white-space:nowrap;width:2%;}";

  // --- end of style ---
  page += "</style></head><body>";

  // ---- TOPNAV ----
  page += "<div class = 'topnav'>";
  page += "<a href='/' target='Home'>Home</a>";
  page += "<a href='/dir'>Files</a> ";
  page += "<a href='/upload'>Upload</a> ";
  page += "<a href='/delete'>Delete</a> ";
  page += "<a href='/rename'>Rename</a> ";
  // page += "<a href='/download'>Download</a> ";
  // page += "<a href='/stream'>Stream</a> ";
  // page += "<a href='/system'>Status</a>";
  // page += "<a href='/fileSystem?mode=toggle'>Spiffs/Sd</a>:" + FLS_NAME[isSPIFFS];
  page += "</div>";

  // if (!isSPIFFS)
  // { // directory func to SD --- by NoRi -----
  //   page += "<br><br>";
  //   page += "<div class = 'topnav'>";
  //   page += "<a href='/root_sd'>Root</a>CurrentDir = " + SdPath;
  //   page += "<a href='/chdir'>Chdir</a> ";
  //   page += "<a href='/mkdir'>Mkdir</a> ";
  //   page += "<a href='/rmdir'>Rmdir</a>";
  //   page += "</div>";
  // }

  // ---- TOPNAV2 ----
  page += "<br><br>";
  // page += "<br>";
  page += "<div class = 'topnav2'>";
  page += "<a href=" + sAPP1_HTML + " target=" + NAME_sAPP1 + "'>" + NAME_sAPP1 + "</a> ";
  page += "<a href=" + sAPP2_HTML + " target=" + NAME_sAPP2 + "'>" + NAME_sAPP2 + "</a> ";
  page += "<a href=" + sAPP3_HTML + " target=" + NAME_sAPP3 + "'>" + NAME_sAPP3 + "</a> ";
  page += "<a href=" + sAPP4_HTML + " target=" + NAME_sAPP4 + "'>" + NAME_sAPP4 + "</a> ";
  page += "<a href=" + sAPP5_HTML + " target=" + NAME_sAPP5 + "'>" + NAME_sAPP5 + "</a> ";
  page += "</div>";

  // ---- TOPNAV3 ----
  page += "<br><br>";
  // page += "<br>";
  page += "<div class = 'topnav3'>";
  page += "<a href=" + uAPP1_HTML + " target=" + NAME_uAPP1 + "'>" + NAME_uAPP1 + "</a> ";
  page += "<a href=" + uAPP2_HTML + " target=" + NAME_uAPP2 + "'>" + NAME_uAPP2 + "</a> ";
  page += "<a href=" + uAPP3_HTML + " target=" + NAME_uAPP3 + "'>" + NAME_uAPP3 + "</a> ";
  // page += "<a href=" + uAPP4_HTML + " target=" + NAME_uAPP4 + "'>" + NAME_uAPP4 + "</a> ";
  // page += "<a href=" + uAPP5_HTML + " target=" + NAME_uAPP5 + "'>" + NAME_uAPP5 + "</a> ";
  page += "<a href=" + GITHUB_URL + " target='Support'>Support</a>";
  page += "</div>";
  page += "<br><br>";
  // page += "<br>";

  return page;
}

// #############################################################################################
String HTML_Footer()
{
  String page;
  page += "<br>";
  page += "<footer>";
  page += "<p class='medium'>" + SERVER_NAME + " is a super-kawaii robot.</p>";
  page += "<p class='ps'><i> " + SWW_VERSION + "</i></p>";
  page += "</footer>";
  page += "<br>";
  page += "</body>";
  page += "</html>";
  return page;
}

// #############################################################################################
String HTML_Header2()
{
  String page;
  page = "<!DOCTYPE html>";
  page += "<html lang = 'ja'>";
  page += "<head>";
  page += "<meta charset='UTF-8'>";
  page += "<meta name='viewport' content='width=device-width,initial-scale=1.0'>";
  page += "<title>StackChan</title>";
  page += "<base target='StackChanSub'>";
  page += "<style>";
  page += "html {font-size: 62.5%;}";
  page += "body {font-size:1.6rem;background-color:#fffde7;text-align:left;}";
  page += "div {font-size:1.6rem;text-align:center;}";
  page += "@media screen and (max-width: 480px) {body{font-size:1.4rem;} img{width:100%;height:auto;}}";
  page += "</style>";
  page += "</head>";
  page += "<body><pre>";
  return page;
}

// #############################################################################################
String HTML_Header2Ng()
{
  String page;
  page = "<!DOCTYPE html>";
  page += "<html lang = 'ja'>";
  page += "<head>";
  page += "<meta charset='UTF-8'>";
  page += "<title>StackChan</title>";
  page += "<base target='StackChanSub'>";
  page += "<meta name='viewport' content='width=device-width,initial-scale=1.0'>";
  page += "<style>";
  page += "html {font-size: 62.5%;}";
  page += "body {font-size:1.6rem;background-color:#ffccff;text-align:left;}";
  page += "div {font-size:1.6rem;text-align:center;}";
  page += "@media screen and (max-width: 480px) {body{font-size:1.4rem;} img{width:100%;height:auto;}}";
  page += "</style>";
  page += "</head>";
  page += "<body><pre>";
  return page;
}

// #############################################################################################
String HTML_Footer2()
{
  String page;
  page += "</pre><br><br>";
  page += "<div><form><input type='button' name='button' value='このウィンドウを閉じる' onclick='window.close();'></form><div>";
  page += "</body></html>";
  return page;
}

// #############################################################################################
String HTML_Header3()
{
  String page;
  page = "<!DOCTYPE html>";
  page += "<html lang = 'ja'>";
  page += "<head>";
  page += "<meta charset='UTF-8'>";
  page += "<meta name='viewport' content='width=device-width,initial-scale=1.0'>";
  page += "<title>StackChan</title>";
  page += "<base target='StackChanSub'>";
  page += "<style>";
  page += "html {font-size: 62.5%;}";
  page += "body {font-size:1.6rem;background-color:#fffde7;text-align:left;}";
  page += "div {font-size:1.6rem;text-align:center;}";
  page += "@media screen and (max-width: 480px) {body{font-size:1.4rem;} img{width:100%;height:auto;}}";
  page += "</style>";
  page += "</head>";
  page += "<body>";
  return page;
}

String HTML_Footer3()
{
  String page;
  page += "<br><br>";
  page += "<div><form><input type='button' name='button' value='このウィンドウを閉じる' onclick='window.close();'></form><div>";
  page += "</body></html>";
  return page;
}
