
#include "TouchData.hpp"

/** 
 * コンストラクタ
 * @brief Sprite作成用親データの取得と、各種シリアルデバッグログの任意許可
 */
TouchData::TouchData (VisualData* vData, LovyanGFX* parent, bool enableErrorLog=false, bool enableInfoLog=false, bool enableSuccessLog=false){
  // シリアルデバッグログの許可・不許可
  debugLog.setDebug(enableErrorLog, enableInfoLog, enableSuccessLog);
  //描画データの取得
  vData = vData;
  // Sprite作成
  judgeSprite = new LGFX_Sprite(parent);
}

/** @fn
 * @brief ページのリストが作成済みかチェックする
 * @param pageName ページ名
 * @return true / false（存在する / 存在しない）
 */
bool TouchData::isExistsProcessPage (String pageName){
  return processList[pageName].is<JsonObject>();
}

/** @fn
 * @brief ページのリストがあり、そのリストの中に指定のプロセスがあるかを返す
 * @param pageName ページ名
 * @param processName プロセス名
 * @return true/false (両方ある / どちらか、または両方が無い)
 */
bool TouchData::isExistsProcess (String pageName, String processName){
  // プロセスのページリストが作成済みか
  if(isExistsProcessPage(pageName)){  // 作成済み
    // ページ内の同プロセス名が存在するかどうか
    return processList[pageName][processName].is<JsonObject>();
  }
  return false;
}

/** @fn
 * @brief 同じページ内オブジェクトの、同じ操作判定を設定するものがあれば確認。あればプロセス名を返す
 * @param pageName ページ名
 * @param objectName オブジェクト名
 * @param type タッチ判定タイプ
 * @return あればプロセス名、無ければ空の文字列
 */
String TouchData::isExistsProcessType(String pageName, String objectName, touchType type){
  JsonObject pageProcessList = processList[pageName];
  for(JsonPair process : pageProcessList){
    if(process.value()["objectName"].as<String>() == objectName 
        && process.value()["type"].as<int>() == static_cast<int>(type)){
      return process.key().c_str();
    }
  }
  return "";
}

JsonObject TouchData::createOrGetProcess(String pageName, String processName){
  // ページリストが無ければ作成
  if(!isExistsProcessPage(pageName)) processList[pageName].to<JsonObject>();  // 新規ページを作成
  // 同プロセス名が無ければ作成
  if(isExistsProcess(pageName, processName)){
    debugLog.printlnLog(debugLog.info, "["+ processName +"] already exists (Modifying the existing process)");
  }else{
    processList[pageName][processName].to<JsonObject>();  // 新規プロセスを作成
  }
  return processList[pageName][processName];  // プロセスを返す
}

/** @fn
 * @brief 各種チェックのち、問題が無ければプロセスを作成
 * @param pageName ページ名
 * @param processName プロセス名
 * @param objectName オブジェクト名
 * @param type タッチ判定タイプ
 * @param enableOverBorder  （スワイプ専用）指が一度範囲外に出ても判定を継続するかどうか
 * @param returnCurrentOver （スワイプ専用）指が範囲外に出ていてもtrueを返すようにするかどうか
 * @return あればプロセス名、無ければ空の文字列
 */
// bool TouchData::setTemplateProcess (String pageName, String processName, String objectName, touchType type, bool enableOverBorder=false, bool returnCurrentOver=false){
//   String createdProcess = isExistsProcessType(pageName, objectName, type);
//   // エラー：同じタイプのプロセスが作成済み
//   if(createdProcess != ""){
//     debugLog.printlnLog(debugLog.error, "Same type process created. (processName:"+ createdProcess +")");
//     return false;
//   }
//   // ページやオブジェクトの作成状態を取得
//   if(!vData->isExistsObjectToPage(pageName, objectName)){
//     // オブジェクトやページが作成されていなかった場合のエラー
//     debugLog.printlnLog(debugLog.error, 
//                           vData->isExistsPage(pageName) ? 
//                             "[" + objectName + "] was not created." : 
//                             "[" + pageName + "] was not created.");
//     return false;
//   }
//   // 作成開始
//   JsonObject process = createOrGetProcess(pageName, processName);
  
//   // 必要なデータをprocessオブジェクトに追加
//   process["type"] = static_cast<int>(touchType::Release);  // touchTypeはintとして設定、例: 1 (Release)
//   process["objectName"] = objectName;
//   // スワイプ専用のプロセスパラメータを設定
//   switch(type){
//     case touchType::Flicking:
//     case touchType::Flicked :
//     case touchType::Dragging:
//     case touchType::Dragged :
//       process["enableOverBorder"] = enableOverBorder;
//       process["returnCurrentOver"] = returnCurrentOver;
//       break;
//     default:
//       break;
//   }
//   return true;
// }

bool TouchData::setReleaseProcess (String pageName, String processName, String objectName){
  return setTemplateProcess(pageName, processName, objectName, touchType::Release);
}
bool TouchData::setReleasingProcess (String pageName, String processName, String objectName){
  return setTemplateProcess(pageName, processName, objectName, touchType::Releasing);
}

bool TouchData::setPressProcess (String pageName, String processName, String objectName){
  return setTemplateProcess(pageName, processName, objectName, touchType::Press);
}
bool TouchData::setPressingProcess (String pageName, String processName, String objectName){
  return setTemplateProcess(pageName, processName, objectName, touchType::Pressing);
}
bool TouchData::setPressedProcess (String pageName, String processName, String objectName){
  return setTemplateProcess(pageName, processName, objectName, touchType::Pressed);
}

bool TouchData::setHoldProcess (String pageName, String processName, String objectName){
  return setTemplateProcess(pageName, processName, objectName, touchType::Hold);
}
bool TouchData::setHoldingProcess (String pageName, String processName, String objectName){
  return setTemplateProcess(pageName, processName, objectName, touchType::Holding);
}
bool TouchData::setHeldProcess (String pageName, String processName, String objectName){
  return setTemplateProcess(pageName, processName, objectName, touchType::Held);
}

bool TouchData::setFlickProcess (String pageName, String processName, String objectName){
  return setTemplateProcess(pageName, processName, objectName, touchType::Flick);
}
bool TouchData::setFlickingProcess (String pageName, String processName, String objectName, bool enableOverBorder, bool returnCurrentOver){
  return setTemplateProcess(pageName, processName, objectName, touchType::Flicking, enableOverBorder, returnCurrentOver);
}
bool TouchData::setFlickedProcess (String pageName, String processName, String objectName, bool enableOverBorder, bool returnCurrentOver){
  return setTemplateProcess(pageName, processName, objectName, touchType::Flicked, enableOverBorder, returnCurrentOver);
}

bool TouchData::setDragProcess (String pageName, String processName, String objectName){
  return setTemplateProcess(pageName, processName, objectName, touchType::Release);
}
bool TouchData::setDraggingProcess (String pageName, String processName, String objectName, bool enableOverBorder, bool returnCurrentOver){
  return setTemplateProcess(pageName, processName, objectName, touchType::Dragging, enableOverBorder, returnCurrentOver);
}
bool TouchData::setDraggedProcess (String pageName, String processName, String objectName, bool enableOverBorder, bool returnCurrentOver){
  return setTemplateProcess(pageName, processName, objectName, touchType::Dragged, enableOverBorder, returnCurrentOver);
}

bool TouchData::setClickedProcess (String pageName, String processName, String objectName){
  return setTemplateProcess(pageName, processName, objectName, touchType::Release);
}
bool TouchData::setMultiClickedProcess (String pageName, String processName, String objectName, int count){
  if(setTemplateProcess(pageName, processName, objectName, touchType::Release)){
    processList[pageName][processName]["count"] = count;
    return true;
  }
  return false;
}

bool TouchData::enableProcess(bool isPress){
  enabledProcessList.clear();
  if(processPage.isNull()) return false;
  enabledProcessList = processPage;
  return true;
}

bool TouchData::disableProcessType(touchType tType){
  if(processPage.isNull()) return false;
  for(JsonPair process : enabledProcessList){
    JsonObject processData = process.value().as<JsonObject>();
    int tTypeInt = processData["type"].as<int>();
    if(tTypeInt == static_cast<int>(tType)){
      enabledProcessList.remove(process.key().c_str());
    }
  }
  return true;
}

bool TouchData::disableProcess(String processName){
  //if()
  enabledProcessList.remove(processName);
  return true;
}

/*
  {
    "page1": {
      "process1": {
        "type": 2,
        "objectName": "objectA"
      },
      "process2": {
        "type": 6,
        "objectName": "objectA",
        "enableOverBorder": true,
        "returnCurrentOver": false
      },
      "process3": {
        "type": 1,
        "objectName": "objectB",
        "count": 3
      }
    }
  }
*/

#define SWITCH_CASE(type, action) case type: action break;

bool TouchData::judgeProcess(uint32_t x, uint32_t y){
  for(JsonPair process : enabledProcessList){
    judgeSprite.clear(BLACK);

    JsonObject processData = process.value().as<JsonObject>();
    String objectName = processData["objectName"].as<String>();
    JsonObject objectData = vData->getObjectData(vData->getDrawingPage(), objectName);

    VisualData::DrawType type = static_cast<VisualData::DrawType>(objectData["type"].as<int>());
    JsonArray args = objectData["args"].as<JsonArray>();

    switch(static_cast<VisualData::DrawType>(objectData["type"].as<int>())){
      SWITCH_CASE(VisualData::DrawType::DrawPixel, {
        debugLog.printlnLog(debugLog.info, "CheckDrawLine");
        judgeSprite.drawPixel(args[0].as<int32_t>(), args[1].as<int32_t>(), 0xfff);
      });
      SWITCH_CASE(VisualData::DrawType::DrawLine, {
        debugLog.printlnLog(debugLog.info, "CheckDrawLine");
        judgeSprite.drawLine(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), 0xfff);
      });
      SWITCH_CASE(VisualData::DrawType::DrawBezier, {
        debugLog.printlnLog(debugLog.info, "CheckDrawBezier");
        judgeSprite.drawBezier(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), 0xfff);
      });
      SWITCH_CASE(VisualData::DrawType::DrawWideLine, {
        debugLog.printlnLog(debugLog.info, "CheckDrawWideLine");
        judgeSprite.drawWideLine(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), 0xfff);
      });

      SWITCH_CASE(VisualData::DrawType::DrawRect, {
        debugLog.printlnLog(debugLog.info, "CheckDrawRect");
        judgeSprite.fillRect(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), 0xfff);
      });
      SWITCH_CASE(VisualData::DrawType::DrawRoundRect, {
        debugLog.printlnLog(debugLog.info, "CheckDrawRoundRect");
        judgeSprite.fillRoundRect(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), 0xfff);
      });
      SWITCH_CASE(VisualData::DrawType::DrawTriangle, {
        debugLog.printlnLog(debugLog.info, "CheckDrawTriangle");
        judgeSprite.fillTriangle(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), 0xfff);
      });
      SWITCH_CASE(VisualData::DrawType::DrawCircle, {
        debugLog.printlnLog(debugLog.info, "CheckDrawCircle");
        judgeSprite.fillCircle(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), 0xfff);
      });
      SWITCH_CASE(VisualData::DrawType::DrawEllipse, {
        debugLog.printlnLog(debugLog.info, "CheckDrawEllipse");
        judgeSprite.fillEllipse(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), 0xfff);
      });
      SWITCH_CASE(VisualData::DrawType::DrawArc, {
        debugLog.printlnLog(debugLog.info, "CheckDrawArc");
        judgeSprite.fillArc(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), 0xfff);
      });
      SWITCH_CASE(VisualData::DrawType::DrawEllipseArc, {
        debugLog.printlnLog(debugLog.info, "CheckDrawEllipseArc");
        judgeSprite.fillEllipseArc(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), args[6].as<int32_t>(), args[7].as<int32_t>(), 0xfff);
      });

      SWITCH_CASE(VisualData::DrawType::FillRect, {
        debugLog.printlnLog(debugLog.info, "CheckFillRect");
        judgeSprite.fillRect(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), 0xfff);
      });
      SWITCH_CASE(VisualData::DrawType::FillRoundRect, {
        debugLog.printlnLog(debugLog.info, "CheckFillRoundRect");
        judgeSprite.fillRoundRect(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), 0xfff);
      });
      SWITCH_CASE(VisualData::DrawType::FillTriangle, {
        debugLog.printlnLog(debugLog.info, "CheckFillTriangle");
        judgeSprite.fillTriangle(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), 0xfff);
      });
      SWITCH_CASE(VisualData::DrawType::FillCircle, {
        debugLog.printlnLog(debugLog.info, "CheckFillCircle");
        judgeSprite.fillCircle(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), 0xfff);
      });
      SWITCH_CASE(VisualData::DrawType::FillEllipse, {
        debugLog.printlnLog(debugLog.info, "CheckFillEllipse");
        judgeSprite.fillEllipse(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), 0xfff);
      });
      SWITCH_CASE(VisualData::DrawType::FillArc, {
        debugLog.printlnLog(debugLog.info, "CheckFillArc");
        judgeSprite.fillArc(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), 0xfff);
      });

      SWITCH_CASE(VisualData::DrawType::DrawJpgFile, {
        debugLog.printlnLog(debugLog.info, "CheckDrawJpgFile");
      });
      SWITCH_CASE(VisualData::DrawType::DrawPngFile, {
        debugLog.printlnLog(debugLog.info, "CheckDrawPngFile");
      });

      SWITCH_CASE(VisualData::DrawType::DrawString, {
        debugLog.printlnLog(debugLog.info, "CheckDrawString");
        judgeSprite.setTextColor(0xfff, 0xfff);
        judgeSprite.setTextDatum(args[5].as<uint8_t>());
        judgeSprite.drawString(args[0].as<String>(), args[1].as<int32_t>(), args[2].as<int32_t>());
      });

      SWITCH_CASE(VisualData::DrawType::ClipArc, {
        debugLog.printlnLog(debugLog.info, "CheckClipArc");
      });
      SWITCH_CASE(VisualData::DrawType::ClipEllipseArc, {
        debugLog.printlnLog(debugLog.info, "CheckClipEllipseArc");
      });
      SWITCH_CASE(VisualData::DrawType::ClipRect, {
        debugLog.printlnLog(debugLog.info, "CheckClipRect");
      });
      SWITCH_CASE(VisualData::DrawType::ClipRoundRect, {
        debugLog.printlnLog(debugLog.info, "CheckClipRoundRect");
      });
      SWITCH_CASE(VisualData::DrawType::ClipCircle, {
        debugLog.printlnLog(debugLog.info, "CheckClipCircle");
      });
      SWITCH_CASE(VisualData::DrawType::ClipEllipse, {
        debugLog.printlnLog(debugLog.info, "CheckClipEllipse");
      });
      SWITCH_CASE(VisualData::DrawType::ClipTriangle, {
        debugLog.printlnLog(debugLog.info, "CheckClipTriangle");
      });

      SWITCH_CASE(VisualData::DrawType::FlexBox, {
        debugLog.printlnLog(debugLog.info, "CheckFlexBox");
      });
      SWITCH_CASE(VisualData::DrawType::TableBox, {
        debugLog.printlnLog(debugLog.info, "CheckTableBox");
      });
      default:
        Serial.println(F("Unknown draw type"));
        break;
    }
    
    int color = judgeSprite.readPixel(x, y);

    if(color == BLACK){
      disableProcess(objectName);
    }
  }
}

bool TouchData::update(){
  if(!M5.Touch.isEnabled()){
    debugLog.printlnLog(debugLog.error, "Touch sensor is disable");
    return false;
  }
  processPage = processList[vData->getDrawingPage()];
  if(!processPage.isNull()){
    auto t = M5.Touch.getDetail();
    
    if(t.isPressed()){
      if(t.wasFlickStart()){
        disableProcessType(touchType::Hold);
        disableProcessType(touchType::Holding);
        disableProcessType(touchType::Held);
        disableProcessType(touchType::Drag);
        disableProcessType(touchType::Dragging);
        disableProcessType(touchType::Dragged);
        disableProcessType(touchType::Clicked);
        disableProcessType(touchType::MultiClicked);
      }else if(t.wasHold()){
        disableProcessType(touchType::Flick);
        disableProcessType(touchType::Flicking);
        disableProcessType(touchType::Flicked);
        disableProcessType(touchType::Clicked);
        disableProcessType(touchType::MultiClicked);
        if(t.wasDragStart()){
          disableProcessType(touchType::Hold);
          disableProcessType(touchType::Holding);
          disableProcessType(touchType::Held);
        }
      }
      judgeProcess(t.x, t.y);
    }
    if(!t.wasReleased()) disableProcessType(touchType::Release);
  }
}

String TouchData::getCurrentProcess(){
  
}

bool TouchData::isOverBorder(){
  
}

bool TouchData::wasOverBorder(){
  
}