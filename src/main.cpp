// ---- <main.cpp>-------------------------------------------------------
#include "main.h"

const String SWS_NAME = "Stackchan-with-WebServer";
const String SWS_VER = "v401e-250316a";
const String SWS_VERSION = SWS_NAME + " " + SWS_VER;
const String SWS_SD_BIN = "/03_StackchanWS.bin";

// ---------------------------------------------------------------------
//  *** Extended from ***
// WebServer-with-stackchan              : NoRi
// AI_StackChan2                         : robo8080さん
// m5stack-avatar                        : ししかわさん/タカオさん
// ESPAsynch_Server_v1.1　　　　　　　　　: David Bird 2022
// M5Stack_Stack-chan_another_dimension  : つゆきぱぱさん
// ----------------------------------------------------------------------


void setup()
{
  WST = WST_SETUP_start;
  // ** initial Setting **
  M5StackConfig01();
  SDU_lobby();
  M5StackConfig02();
  
  Serial.println(SWS_VERSION);
  M5LedBegin();
  ledRed();
  M5SpeakerConfig();
  M5FileSystemBegin();

  // ** Setting files **
  startupSetting();
  apikeySetting();
  servoSetting();

  // *** Network  ***
  ledGreen();
  wifiSetup();
  serverSetup();
  clockSetup();

  // *** Servo, chatGpt ***
  servoSetup();
  chatGptSetup();
  ledBlue();

  delay(2000);

  //*** Wake UP! STACKCHAN ***
  avatarSTART();
  statusLineSetup();
  BoxTouchSetup();

  muteOff();
  ledClearAll();
  WST = WST_SETUP_done;
  Serial.println("*** End of SETUP ***");
}

void loop()
{
  stateManage();
  buttonManage();
  requestManage();
  statusLineManage();

  chatGptManage();
  timerManage();
}
