
#include "TouchData.hpp"

touchData::touchData(LovyanGFX* parent, bool enableErrorLog, bool enableInfoLog, bool enableSuccessLog){
  debugLog.setDebug(enableErrorLog, enableInfoLog, enableSuccessLog);
  judgeSprite = new LGFX_Sprite(parent);
}

bool touchData::isExistsProcessPage(String pageName){
  return processList[pageName].is<JsonObject>();
}

int touchData::isExistsProcess(String pageName, String processName, String objectName, touchType type){
  if(isExistsProcessPage(pageName)){
    if(processList[pageName][processName].is<JsonObject>()){
      return 1;
    }
    JsonObject pageProcess = processList[pageName];
    for(JsonPair process : pageProcess){
      if(process.value()["objectName"].as<String>() == processName 
          && process.value()["type"].as<int>() == static_cast<int>(type)){
        return 2;
      }
    }
    return 0;
  }
  return 3;
}

bool touchData::setTemplateProcess     (String pageName, String processName, String objectName, touchType type, bool enableOverBorder=false, bool returnCurrentOver=false){
  JsonObject process;
  if(isExistsProcess(processName, objectName, type)){
    // JSONオブジェクトを作成
    process = processList[processName].to<JsonObject>();
  }else if(isExistsProcess(processName, objectName, type) == 1){

  }
  // 必要なデータをprocessオブジェクトに追加
  process["type"] = static_cast<int>(touchType::Release);  // touchTypeはintとして設定、例: 1 (Release)
  process["objectName"] = objectName;
  return true;
}

bool touchData::setReleaseProcess      (String pageName, String processName, String objectName){
}
bool touchData::setReleasingProcess    (String pageName, String processName, String objectName){
  JsonObject process = processList[processName].to<JsonObject>();
  process["type"] = 1;
  process["objectName"] = objectName;
  return true;
  
}

bool touchData::setPressProcess        (String pageName, String processName, String objectName){
  
}
bool touchData::setPressingProcess     (String pageName, String processName, String objectName){
  
}
bool touchData::setPressedProcess      (String pageName, String processName, String objectName){
  
}

bool touchData::setHoldProcess         (String pageName, String processName, String objectName){
  
}
bool touchData::setHoldingProcess      (String pageName, String processName, String objectName){
  
}
bool touchData::setHeldProcess         (String pageName, String processName, String objectName){
  
}

bool touchData::setFlickProcess        (String pageName, String processName, String objectName){
  
}
bool touchData::setFlickingProcess     (String pageName, String processName, String objectName, bool enableOverBorder){
  
}
bool touchData::setFlickedProcess      (String pageName, String processName, String objectName, bool enableOverBorder, bool returnCurrentOver){
  // JSONオブジェクトを作成
  JsonObject process = processList[processName].to<JsonObject>();

  // 必要なデータをprocessオブジェクトに追加
  process["type"] = 1;  // touchTypeはintとして設定、例: 1 (Flick)
  process["objectName"] = objectName;
  process["enableOverBorder"] = enableOverBorder;
  process["returnCurrentOver"] = returnCurrentOver;

  return true;  // 成功した場合はtrueを返す
}

bool touchData::setDragProcess         (String pageName, String processName, String objectName){
  
}
bool touchData::setDraggingProcess     (String pageName, String processName, String objectName, bool enableOverBorder){
  
}
bool touchData::setDraggedProcess      (String pageName, String processName, String objectName, bool enableOverBorder, bool returnCurrentOver){
  
}

bool touchData::setClickedProcess      (String pageName, String processName, String objectName){
  
}
bool touchData::setMultiClickedProcess (String pageName, String processName, String objectName, int count){
  
}

String touchData::getCurrentProcess(){
  
}
bool touchData::isOverBorder(){
  
}
bool touchData::wasOverBorder(){
  
}