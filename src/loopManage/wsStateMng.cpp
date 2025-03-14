// ----------------------------<wsStateMng.cpp>------------------------------------
#include "wsStateMng.h"

uint16_t WST = WST_INIT;

void stateManage()
{
  if (WST == WST_LOOP || WST == WST_TTS_talking)
    return;

  switch (WST)
  {
  case WST_SETUP_done:
    log_free_size("setup() : --- END --- ");
    showExeTime("setup() : --- END --- ");
    WST = WST_LOOP;
    return;

  case WST_chatGPT_exit:
    log_free_size("chatGPT exit ");
    showExeTime("chatGPT exit : ");
    if (SELF_TALK_STATE == true)
      setNextSelfTalkTime();
    WST = WST_LOOP;
    return;

  case WST_chatGPT_done:
    return;

  case WST_TTS_talkStart:
    WST = WST_TTS_talking;
    return;

  case WST_TTS_exit:
    log_free_size("TTS exit : ");
    showExeTime("TTS exit : ");
    if (SELF_TALK_STATE == true)
      setNextSelfTalkTime();
    WST = WST_LOOP;
    return;

  case WST_TTS_talkDone:
    avatar.setMouthOpenRatio(0);
    avatar.setSpeechText("");

    if (REQ_EXPR_AFTER >= 0 && REQ_EXPR_AFTER <= 5)
      avatar.setExpression(expr_table[REQ_EXPR_AFTER]);
    REQ_EXPR_AFTER = -1;

    if (SELF_TALK_STATE == true)
      setNextSelfTalkTime();

    log_free_size("VOICEVOX : OUT");
    showExeTime("VOICEVOX : end of speaking");
    WST = WST_LOOP;
    return;

  default:
    Serial.println("WST state value get : WST = " + String(WST, HEX));
    // WST = WST_LOOP;
    return;
  }

  return;
}
