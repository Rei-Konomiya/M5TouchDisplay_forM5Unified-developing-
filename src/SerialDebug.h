#pragma once

#include <Arduino.h>

class Debug{
private:
  bool debugEnabled = false; // デフォルトはOFF

public:
  // デバッグON/OFFを切り替える関数
  void setDebug(bool enabled){
    debugEnabled = enabled;
  }

  // デバッグメッセージ出力関数
  void printDebug(String message){
    if (debugEnabled){
      Serial.println(message);
    }
  }
};