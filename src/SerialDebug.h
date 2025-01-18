#pragma once

#include <Arduino.h>

class Debug{
private:
  bool enableDebugLog, enableErrorLog, enableInfoLog, enableSuccessLog;

public:
  enum LogType{
    none,
    error,
    info,
    success
  };

  Debug(bool enableError = false, bool enableInfo = false, bool enableSuccess = false)
    : enableErrorLog(enableError), enableInfoLog(enableInfo), enableSuccessLog(enableSuccess) {
      if(enableError || enableInfo || enableSuccess) enableDebugLog = true;
      else enableDebugLog = false;
    }

  // デバッグON/OFFを切り替える関数
  void setDebug(bool enableError, bool enableInfo, bool enableSuccess){
    enableErrorLog = enableError;
    enableInfoLog = enableInfo;
    enableSuccessLog = enableSuccess;
  }

  // デバッグメッセージ出力関数
  void printLog(LogType type, String message){
    if(type == none && enableErrorLog){
      Serial.print(message);
    }else if(type == error && enableErrorLog){
      Serial.print(F("error : "));
      Serial.print(message);
    }else if(type == info && enableInfoLog){
      Serial.print(F("info : "));
      Serial.print(message);
    }else if(type == success && enableSuccessLog){
      Serial.print(F("success : "));
      Serial.print(message);
    }
  }

  // デバッグメッセージ出力関数
  void printlnLog(LogType type, String message){
    if(type == none && enableErrorLog){
      Serial.println(message);
    }else if(type == error && enableErrorLog){
      Serial.print(F("error : "));
      Serial.println(message);
    }else if(type == info && enableInfoLog){
      Serial.print(F("info : "));
      Serial.println(message);
    }else if(type == success && enableSuccessLog){
      Serial.print(F("success : "));
      Serial.println(message);
    }
  }
};