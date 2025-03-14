// ----------------------------<wsChatGPT.cpp>------------------------------------
#include "wsChatGPT.h"

// const String chatStrIData =
//     "{\"model\": \"gpt-3.5-turbo-0613\",\"max_tokens\":512,\"messages\": [{\"role\": \"user\", \"content\": \""
//     "\"}]}";

// const String chatStrIData =
//     "{\"model\": \"gpt-4o-mini\",\"max_tokens\":512,\"messages\": [{\"role\": \"user\", \"content\": \""
//     "\"}]}";

const String chatStrIData =
    "{\"model\": \"gpt-4o-mini\",\"temperature\":0.7,\"max_tokens\":512,\"messages\": [{\"role\": \"user\", \"content\": \""
    "\"}]}";

const String charaStrIData = "{\"character\":[{\"name\":\"\",\"vSpkNo\":\"3\",\"role\":\"\"},{\"name\":\"\",\"vSpkNo\":\"3\",\"role\":\"\"},{\"name\":\"\",\"vSpkNo\":\"3\",\"role\":\"\"},{\"name\":\"\",\"vSpkNo\":\"3\",\"role\":\"\"},{\"name\":\"\",\"vSpkNo\":\"3\",\"role\":\"\"},{\"name\":\"\",\"vSpkNo\":\"3\",\"role\":\"\"},{\"name\":\"\",\"vSpkNo\":\"3\",\"role\":\"\"},{\"name\":\"\",\"vSpkNo\":\"3\",\"role\":\"\"},{\"name\":\"\",\"vSpkNo\":\"3\",\"role\":\"\"}]}";

const String CHATDOC_SPI = "/data.json"; // chatDoc in SPIFFS
const String CHARA_SPIFFS = "/wsCharacter.json";
const int MAX_HISTORY = 5; // 保存する質問と回答の最大数

String CHATDOC_INIT_BUF = "";

DynamicJsonDocument CHAT_DOC(1024 * 10);
std::deque<String> chatHistory; // 過去の質問と回答を保存するデータ構造

// 「独り言モード」
const String random_words[] = {"あなたは誰", "楽しい", "怒った", "可愛い", "悲しい", "眠い", "ジョークを言って", "泣きたい", "怒ったぞ", "こんにちは", "お疲れ様", "詩を書いて", "疲れた", "お腹空いた", "嫌いだ", "苦しい", "俳句を作って", "歌をうたって"};
bool RANDOM_SPEAK_STATE = false; // 独り言モード　true -> on  false -> off
bool RANDOM_SPEAK_ON_GET = false;
bool RANDOM_SPEAK_OFF_GET = false;
uint32_t RANDOM_TM = 0;      // 「独り言モード」待ち時間
uint32_t RANDOM_TM_LAST = 0; // 「独り言モード」前回の実行時刻
bool REQ_chatGPT_GET = false;

// 「わかりません」対策
int WK_CNT = 0;
int WK_ERR_NO = 0;
int WK_ERR_CODE = 0;
int WK_LAST_ERR_NO = 0;
int WK_LAST_ERR_CODE = 0;

void chatGptManage()
{
  // --  RandomSpeakManage ---
  // if (RANDOM_SPEAK_ON_GET)
  if (RANDOM_SPEAK_ON_GET && statusMode != STM_SYSINFO)
  {
    RANDOM_SPEAK_ON_GET = false;

    // if (SYSINFO_DISP_STATE)
    //   sysInfoDispEnd();

    timerStop2();
    if (!RANDOM_SPEAK_STATE)
      randomSpeak(true);
  }

  if (RANDOM_SPEAK_OFF_GET)
  {
    RANDOM_SPEAK_OFF_GET = false;

    if (RANDOM_SPEAK_STATE)
      randomSpeak(false);
  }

  if (RANDOM_SPEAK_STATE && (millis() > (RANDOM_TM_LAST + RANDOM_TM)))
  {
    RANDOM_TM_LAST = millis();               // 今回のRandomSpeak起動した時刻
    RANDOM_TM = 1000 * (40 + random(0, 30)); // 次回のRandomSpeak起動までの時間

    if (!isTalking())
    {
      exec_chatGPT(random_words[random(18)]);
    }
  }

  // --- chatGPT REQ ----------
  // if (REQ_chatGPT_GET)
  if (REQ_chatGPT_GET && statusMode != STM_SYSINFO)
  {
    // if (SYSINFO_DISP_STATE)
    //   sysInfoDispEnd();

    randomSpeakStop2();
    timerStop2();

    REQ_chatGPT_GET = false;

    exec_chatGPT(REQ_MSG);
  }
}

void wsHandleRandomSpeak(String modeS)
{
  if (modeS == "")
    return;

  String mode = modeS;
  mode.toUpperCase();

  if (mode == "ON")
  {
    if (!RANDOM_SPEAK_STATE)
      RANDOM_SPEAK_ON_GET = true;
  }
  else if (mode == "OFF")
  {
    if (RANDOM_SPEAK_STATE)
      RANDOM_SPEAK_OFF_GET = true;
  }

  webpage = "randomSpeak : mode = " + modeS;
  Serial.println(webpage);
}

void wsHandelChat(String textS, String voiceS)
{
  if (textS == "")
    return;

  if (voiceS != "")
    TTS_vSpkNo = (uint8_t)voiceS.toInt();

  REQ_chatGPT_GET = true;
  REQ_MSG = textS;

  webpage = "chat : text  = " + textS + "<br>";
  webpage += "chat : voice = " + String(TTS_vSpkNo, DEC);
}

void wsHandelChatCharacter(String ch_NoS, String ch_nameS, String ch_voiceS, String ch_roleS)
{
  Serial.println("no = " + ch_NoS);
  Serial.println("name = " + ch_nameS);
  Serial.println("voice = " + ch_voiceS);
  Serial.println("role = " + ch_roleS);

  DynamicJsonDocument charaJson(CHARA_SIZE);
  if (!jsonRead(FLTYPE_SPIFFS, charaJson, CHARA_SPIFFS))
  {
    Serial.println("faile to Read from SPIFFS in wsHandleChatGpt func");
    initCharaJson(charaJson);
    return;
  }

  if ((ch_NoS == "") && (ch_nameS == "") && (ch_voiceS == "") && (ch_roleS == ""))
  {
    // HTMLデータを出力する
    String html = "";
    serializeJsonPretty(charaJson, html);
    // Serial.println(html);
    webpage = html;
    return;
  }

  int charaNo = 0;
  String charaName = "";
  String charaVoiceNo = "3";
  String charaRole = "";

  if (ch_NoS == "0" || ch_NoS == "1" || ch_NoS == "2" || ch_NoS == "3" || ch_NoS == "4" || ch_NoS == "5" || ch_NoS == "6" || ch_NoS == "7" || ch_NoS == "8")
  {
    charaNo = ch_NoS.toInt();
  }
  else
    return;

  if (ch_nameS == "")
    charaName = ch_NoS + "　ばん";
  else
    charaName = ch_nameS;

  int tmpVoiceNo = ch_voiceS.toInt();
  if ((tmpVoiceNo >= 0) && (tmpVoiceNo <= 66))
  {
    charaVoiceNo = ch_voiceS;
  }

  if (ch_roleS != "")
    charaRole = ch_roleS;

  JsonArray jsonArray = charaJson["character"];
  JsonObject object = jsonArray[charaNo];
  object["name"] = charaName;
  object["vSpkNo"] = charaVoiceNo;
  object["role"] = charaRole;

  bool success = jsonDocSave(charaJson, CHARA_SPIFFS);
  if (!success)
  {
    return;
  }

  webpage = "Character No. = " + ch_NoS + "  modified";
}

bool initCharaJson(DynamicJsonDocument &charaJson)
{
  DeserializationError error = deserializeJson(charaJson, charaStrIData);
  if (error)
  {
    Serial.println("DeserializationError in initCharaJson func");
    return false;
  }
  jsonDocSave(charaJson, CHARA_SPIFFS);
  return true;
}

int CHARA_NO = -1;

void wsHandelChatGpt(String historyS, String charaS)
{
  if (historyS == "" && charaS == "")
    return;

  if (charaS == "0" || charaS == "1" || charaS == "2" || charaS == "3" || charaS == "4" || charaS == "5" || charaS == "6" || charaS == "7" || charaS == "8")
  {
    DynamicJsonDocument charaJson(CHARA_SIZE);

    if (!jsonRead(FLTYPE_SPIFFS, charaJson, CHARA_SPIFFS))
    {
      Serial.println("faile to Read from SPIFFS in wsHandleChatGpt func");
      initCharaJson(charaJson);
      return;
    }

    // --- New Character Set -------------------------
    JsonArray jsonArray = charaJson["character"];
    CHARA_NO = charaS.toInt();
    JsonObject chara = jsonArray[CHARA_NO];
    String chara_name = chara["name"];
    String chara_vSpkNoS = chara["vSpkNo"];
    String chara_role = chara["role"];

    Serial.println("charaNo = " + String(CHARA_NO, DEC));
    Serial.println("name = " + chara_name);
    Serial.println("vSpkNo = " + chara_vSpkNoS);
    Serial.println("role = " + chara_role);

    setChatDoc(chatStrIData); // 初期データの登録

    JsonArray messages = CHAT_DOC["messages"];
    JsonObject systemMessage1 = messages.createNestedObject();
    systemMessage1["role"] = "system";
    systemMessage1["content"] = chara_role;

    chatHistory.clear();
    CHATDOC_INIT_BUF = "";
    serializeJson(CHAT_DOC, CHATDOC_INIT_BUF);
    saveChatDoc();

    // ---- Speaker -------
    size_t speaker_no;
    if (chara_vSpkNoS != "")
    {
      uint32_t nvs_handle;

      speaker_no = chara_vSpkNoS.toInt();
      if (speaker_no > 66)
      {
        speaker_no = 3;
      }
      TTS_vSpkNo = (uint8_t)speaker_no;

      if (ESP_OK == nvs_open(SETTING_NVS, NVS_READWRITE, &nvs_handle))
      {
        nvs_set_u32(nvs_handle, "vSpkNo", speaker_no);
        Serial.println("NVS Write : vSpkNo = " + String(speaker_no, DEC));
      }
      nvs_close(nvs_handle);
    }

    webpage = "character changed<br><br>";
    webpage += "character No. = " + String(CHARA_NO, DEC) + "<br>";
    webpage += "name = " + chara_name + "<br>";
    webpage += "vSpkNo = " + chara_vSpkNoS + "<br>";
    webpage += "role = " + chara_role + "<br><br>";

    if (chara_name != "")
    {
      String spkMsg = chara_name + " です。";
      Serial.println(spkMsg);
      stackchanReq(spkMsg);
    }
    return;
  }

  if (historyS.equalsIgnoreCase("all"))
  {
    webpage = "<br>";
    int max_history = chatHistory.size();
    for (int i = 0; i < max_history; i++)
    {
      if ((i % 2) == 0)
      {
        webpage += "<font color='blue'>(You)</font>　" + chatHistory[i] + "<br>";
      }
      else
      {
        webpage += "<font color='#ff0000'>(chatGPT)</font>　" + chatHistory[i] + "<br><br>";
      }
    }
  }
}

void wsHandleRoleSet(String roleS)
{
  String role = roleS;

  if (role != "")
  {
    setChatDoc(chatStrIData);
    JsonArray messages = CHAT_DOC["messages"];
    JsonObject systemMessage1 = messages.createNestedObject();
    systemMessage1["role"] = "system";
    systemMessage1["content"] = role;
    webpage = "chatGPT : set role data ";
  }
  else
  {
    setChatDoc(chatStrIData);
    webpage = "chatGPT : clear role data ";
  }
  chatHistory.clear();
  CHATDOC_INIT_BUF = "";
  serializeJson(CHAT_DOC, CHATDOC_INIT_BUF);
  saveChatDoc();
}

void wsHandleRoleGet()
{
  webpage = "";
  serializeJsonPretty(CHAT_DOC, webpage);
  Serial.println("\n****** role_get ******");
  Serial.println(webpage);
  Serial.println("**********************\n");
}

bool chatDocInit()
{
  File fl_SPIFFS = SPIFFS.open(CHATDOC_SPI, "r");
  if (!fl_SPIFFS)
  {
    fl_SPIFFS.close();
    String errorMsg1 = "*** Failed to open file for reading *** ";
    String errorMsg2 = "*** FATAL ERROR : cannot READ CHAT_DOC FILE !!!! ***";
    Serial.println(errorMsg1);
    Serial.println(errorMsg2);
    M5.Lcd.print(errorMsg1);
    M5.Lcd.print(errorMsg2);
    setChatDoc(chatStrIData);
    return false;
  }

  DeserializationError error = deserializeJson(CHAT_DOC, fl_SPIFFS);
  fl_SPIFFS.close();

  if (error)
  { // ファイルの中身が壊れていた時の処理 ---------
    Serial.println("Failed to deserialize JSON");
    // if (!setChatDoc(chatStrIData.c_str()))
    if (!setChatDoc(chatStrIData))
    {
      String errorMsg1 = "*** Failed to init chat_doc JSON in SPIFFS *** ";
      String errorMsg2 = "*** FATAL ERROR : cannot READ/WRITE CHAT_DOC FILE !!!! ***";
      Serial.println(errorMsg1);
      Serial.println(errorMsg2);
      M5.Lcd.print(errorMsg1);
      M5.Lcd.print(errorMsg2);
      return false;
    }
    else
    { // JSONファイルをSPIFF に保存
      fl_SPIFFS = SPIFFS.open(CHATDOC_SPI, "w");
      if (!fl_SPIFFS)
      {
        fl_SPIFFS.close();
        String errorMsg1 = "*** Failed to open file for writing *** ";
        String errorMsg2 = "*** FATAL ERROR : cannot WRITE CHAT_DOC FILE !!!! ***";
        Serial.println(errorMsg1);
        Serial.println(errorMsg2);
        M5.Lcd.print(errorMsg1);
        M5.Lcd.print(errorMsg2);
        return false;
      }
      serializeJson(CHAT_DOC, fl_SPIFFS);
      fl_SPIFFS.close();
      Serial.println("initial chat_doc data store in SPIFFS");
    }
  }

  serializeJson(CHAT_DOC, CHATDOC_INIT_BUF);
  return true;
}

void randomSpeakStop2()
{
  RANDOM_SPEAK_STATE = false;
  RANDOM_SPEAK_ON_GET = false;
}

void randomSpeak(bool mode)
{
  String speakMsg;

  if (mode)
  {
    speakMsg = "独り言始めます。";
    RANDOM_TM_LAST = millis();
    RANDOM_TM = 1000 * (40 + random(0, 30));
    RANDOM_SPEAK_STATE = true;
  }
  else
  {
    speakMsg = "独り言やめます。";
    RANDOM_SPEAK_STATE = false;
  }

  RANDOM_SPEAK_ON_GET = false;
  RANDOM_SPEAK_OFF_GET = false;
  stackchanReq(speakMsg);
}

bool setChatDoc(const String &data)
{
  DeserializationError error = deserializeJson(CHAT_DOC, data.c_str());
  if (error)
  {
    Serial.println("DeserializationError");
    return false;
  }
  return true;
}

String https_post_json(const char *url, const char *json_string, const char *root_ca)
{
  WK_ERR_NO = 0;
  WK_ERR_CODE = 0;

  String payload = "";
  WiFiClientSecure *client = new WiFiClientSecure;
  if (client)
  {
    client->setCACert(root_ca);
    {
      HTTPClient https;
      https.setTimeout(UINT16_MAX); // 最大値の約65秒にタイムアウトを設定

      if (https.begin(*client, url))
      {
        https.addHeader("Content-Type", "application/json");
        https.addHeader("Authorization", String("Bearer ") + OPENAI_API_KEY);
        int httpCode = https.POST((uint8_t *)json_string, strlen(json_string));

        WK_ERR_CODE = httpCode;

        if (httpCode > 0)
        {
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
          {
            payload = https.getString();
            if (payload == "")
            {
              Serial.println("CODE_OK or CODE_MOVED_PERMANENTLY and payload is void ");
              WK_ERR_NO = 1;
            }
          }
          else
          { // エラーコードを取得できた場合
            Serial.println("httpCode other error code number get ");
            WK_ERR_NO = 2;
          }
        }
        else
        {
          Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
          WK_ERR_NO = 3;
        }
        https.end();
      }
      else
      {
        Serial.printf("[HTTPS] Unable to connect\n");
        WK_ERR_NO = 4;
      }
    }
    delete client;
  }
  else
  {
    Serial.println("Unable to create client");
    WK_ERR_NO = 5;
  }
  return payload;
}


String chatGpt(String json_string)
{
  String response = "";
  stackchanNow(EXPR_DOUBT,"考え中…");
  String ret = https_post_json("https://api.openai.com/v1/chat/completions", json_string.c_str(), root_ca_openai);
  stackchanNow(EXPR_NEUTRAL,"");

  if (ret != "")
  {// ret が正常である場合
    WK_CNT = 0;
    DynamicJsonDocument doc(DOC_SIZE);
    DeserializationError error = deserializeJson(doc, ret.c_str());

    if (!error)
    { //------ 正常処理 --------------
      const char *data = doc["choices"][0]["message"]["content"];
      response = String(data);
      std::replace(response.begin(), response.end(), '\n', ' ');
      return response;
    }
    else
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      stackchanNow(EXPR_SAD,"エラーです");
      Serial.println("chatGPT err : desirialization ");
      response = "";
      return response;
    }
  }
  else
  {// ret が　エラーだった場合
    String msg3 = "";
    if (WK_ERR_CODE == 0)
      msg3 = "わかりません、番号 " + String(WK_ERR_NO,DEC)  + "です。";
    else if (WK_ERR_CODE < 0)
      msg3 = "わかりません、番号 " + String(WK_ERR_NO,DEC) + "コード・マイナス" + String(abs(WK_ERR_CODE),DEC) + "です。";
    else
      msg3 = "わかりません、番号 " + String(WK_ERR_NO,DEC) + "コード" + String(WK_ERR_CODE,DEC) + "です。";

    WK_LAST_ERR_NO = WK_ERR_NO;
    WK_LAST_ERR_CODE = WK_ERR_CODE;
    WK_ERR_NO = 0;
    WK_ERR_CODE = 0;
    WK_CNT++;
    Serial.println("WK_CNT = " + String(WK_CNT, DEC));
    stackchanNow(EXPR_SAD,msg3);
    Serial.println(msg3);
    response = "";
    return response;    
  }
  return response;
}



void exec_chatGPT(String toChatGptText)
{
  showExeTime("", EXE_TM_MD_START); // timer start
  log_free_size("\nChatGPT  : IN");
  WST = WST_chatGPT_start;

  Serial.println("----- [ talk to chatGPT ] -----");
  Serial.println(toChatGptText);
  Serial.println("--------------------------------");

  // ----   chatGPTに送るデータを作成　-----------------------------
  setChatDoc(CHATDOC_INIT_BUF); // CHATDOC_INIT_BUF のデータをセットする。
  chatHistory.push_back(toChatGptText);
  if (chatHistory.size() > MAX_HISTORY * 2)
  {
    chatHistory.pop_front();
    chatHistory.pop_front();
  }
  for (int i = 0; i < chatHistory.size(); i++)
  {
    JsonArray messages = CHAT_DOC["messages"];
    JsonObject systemMessage1 = messages.createNestedObject();
    if (i % 2 == 0)
    {
      systemMessage1["role"] = "user";
    }
    else
    {
      systemMessage1["role"] = "assistant";
    }
    systemMessage1["content"] = chatHistory[i];
  }
  String chatDocJson;
  String chatResponse = "";
  serializeJson(CHAT_DOC, chatDocJson);
  // -----------------------------------------------------------------

  if (isTalking())
  {
    Serial.println("chatGPT exit : 話し中");
    WST = WST_chatGPT_exit;
    return;
  }
  else
  {
    chatResponse = chatGpt(chatDocJson);
    if (chatResponse != "")
    { // chatGPT応答が正常な場合
      chatHistory.push_back(chatResponse);
      stackchanReq(chatResponse, EXPR_HAPPY, "$$SKIP$$", EXPR_NEUTRAL);
      showExeTime("ChatGPT  : chatResponse get, then move to VOICEVOX");
      log_free_size("ChatGPT  : OUT");
      WST = WST_chatGPT_done;
      return;
    }
    else
    {
      Serial.println("chatGPT exit : エラー");
      WST = WST_chatGPT_exit;
      return;
    }
  }
}

bool saveChatDoc()
{
  File fl_SPIFFS = SPIFFS.open(CHATDOC_SPI, "w");
  if (!fl_SPIFFS)
  {
    Serial.println("Failed to open file for writing");
    return false;
  }

  serializeJson(CHAT_DOC, fl_SPIFFS);
  fl_SPIFFS.close();
  return true;
}

void chatGptSetup()
{
  // ***  chat_doc initialize  ****
  bool success = chatDocInit();
  if (!success)
  {
    String msg = "cannnot init chat_doc! ";
    Serial.println(msg);
    M5.Display.println();
    M5.Display.println(msg);
    Serial.println(" *** Reboot ***");
    REBOOT();
  }

  // *** final msg in setup() ****
  Serial.println("HTTP server started");
  M5.Lcd.println("HTTP server started");
  Serial.println("_  to control the chatGpt Server.");
  M5.Lcd.println("_  to control the chatGpt Server.");
}

void chatHistoryCls()
{ // 会話履歴クリア
  chatHistory.clear();
}
