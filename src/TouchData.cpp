#include "TouchData.hpp"

/** * コンストラクタ
 * @brief Sprite作成用親データの取得と、各種シリアルデバッグログの任意許可
 */
TouchData::TouchData (VisualData* vData, LovyanGFX* parent, bool enableErrorLog, bool enableInfoLog, bool enableSuccessLog){
  debugLog.setDebug(enableErrorLog, enableInfoLog, enableSuccessLog); 
  this->vData = vData;
  this->parent = parent;
  
  // ★ 修正: JsonDocument のメモリ確保 (サイズは十分に確保すること)
  // 動的なサイズ割り当てになるため、データを追加するたびにメモリ再割り当てが発生する可能性があります。
  // 必要に応じて、setup() の early_init() などで事前に reserve() を呼ぶと良いかもしれません。
  // 例: processListDoc.setCapacity(4096);
  processListDoc = JsonDocument(); // 新しい空のJsonDocumentを生成
  processList = processListDoc.to<JsonObject>(); // JsonDocumentにJsonObjectを紐付け
  
  enabledDoc = JsonDocument(); // 新しい空のJsonDocumentを生成
  enabledProcessList = enabledDoc.to<JsonObject>(); // JsonDocumentにJsonObjectを紐付け

  judgeSprite = new LGFX_Sprite(parent);
  judgeSprite->setPsram(true);
  judgeSprite->setColorDepth(1);
  bool created = judgeSprite->createSprite(320, 240);
  if (!created) {
    debugLog.printlnLog(Debug::error, "[ERROR] judgeSprite createSprite failed"); // DebugLogを使用
  }
}

// ★ 追加: setProcessPage メソッド
void TouchData::setProcessPage(String pageName){
    if (vData == nullptr) {
        debugLog.printlnLog(Debug::error, "setProcessPage: vData is nullptr. Cannot set process page.");
        return;
    }
    
    // processListが有効かチェック
    if (processList.isNull()) {
        debugLog.printlnLog(Debug::error, "setProcessPage: processList is null. This should not happen after constructor.");
        return;
    }

    // processListに指定されたページ名が存在するか確認
    if(processList[pageName].is<JsonObject>()){
        // 存在すれば、そのページをprocessPageとして設定
        processPage = processList[pageName].as<JsonObject>();
        debugLog.printlnLog(Debug::info, "TouchData::setProcessPage - Process page set to: " + pageName + " (Found existing page)."); // ログ修正
    } else {
        // 存在しない場合は、新しくそのページのエントリをprocessListに作成し、それをprocessPageとして設定
        debugLog.printlnLog(Debug::info, "TouchData::setProcessPage - Page '" + pageName + "' not found in processList. Creating new empty page entry."); // ログ修正
        // ★ 修正: createNestedObject の代わりに to<JsonObject>() を使用
        JsonObject newPage = processList[pageName].to<JsonObject>(); 
        if (newPage.isNull()) {
            debugLog.printlnLog(Debug::error, "TouchData::setProcessPage - Failed to create new page object for: " + pageName + ". Memory issue?"); // ログ修正
            return; // 作成に失敗したら処理を中断
        }
        processPage = newPage; // 作成した新しいオブジェクトを processPage に設定
    }
}


/** @fn
 * @brief 指定されたページ名を持つプロセスリストが存在するかどうかをチェックする
 * @param pageName プロセスリストをチェックするページの識別名
 * @return 存在すればtrue、しなければfalse
 */
bool TouchData::isExistsProcessPage (String pageName){
  bool exists = processList[pageName].is<JsonObject>();
  return exists;
}

/** @fn
 * @brief ページのリストがあり、そのリストの中に指定のプロセスがあるかを返す
 * @param pageName ページ名
 * @param processName プロセス名
 * @return true/false (両方ある / どちらか、または両方が無い)
 */
bool TouchData::isExistsProcess (String pageName, String processName){
  if(isExistsProcessPage(pageName)){
    bool exists = processList[pageName][processName].is<JsonObject>();
    return exists;
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
  if (!isExistsProcessPage(pageName)) {
      return "";
  }
  JsonObject pageProcessList = processList[pageName];
  if (pageProcessList.isNull()) {
      return "";
  }
  for(JsonPair process : pageProcessList){
    if(process.value()["objectName"].as<String>() == objectName 
        && process.value()["type"].as<int>() == static_cast<int>(type)){
      String key = process.key().c_str();
      return key;
    }
  }
  return "";
}

JsonObject TouchData::createOrGetProcess(String pageName, String processName){
  // ページリストが無ければ作成
  if(!isExistsProcessPage(pageName)) {
    // ★ 修正: createNestedObject の代わりに to<JsonObject>() を使用
    JsonObject newPage = processList[pageName].to<JsonObject>();
    if (newPage.isNull()) {
        debugLog.printlnLog(Debug::error, "[ERROR] Failed to create new page object for: " + pageName + " (createOrGetProcess)"); // DebugLogを使用
        return JsonObject(); // 失敗したらnullオブジェクトを返す
    }
  } else {
  }

  // 同プロセス名が無ければ作成
  if(isExistsProcess(pageName, processName)){
    debugLog.printlnLog(Debug::info, "["+ processName +"] already exists (Modifying the existing process)");
  }else{
    JsonObject targetPage = processList[pageName].as<JsonObject>();
    if (targetPage.isNull()) {
        debugLog.printlnLog(Debug::error, "[ERROR] Target page object is null when trying to create nested process: " + pageName); // DebugLogを使用
        return JsonObject();
    }
    // ★ 修正: createNestedObject の代わりに to<JsonObject>() を使用
    JsonObject newProcess = targetPage[processName].to<JsonObject>();
    if (newProcess.isNull()) {
        debugLog.printlnLog(Debug::error, "[ERROR] Failed to create new process object for: " + processName + " (createOrGetProcess)"); // DebugLogを使用
        return JsonObject();
    }
  }
  JsonObject result = processList[pageName][processName].as<JsonObject>();
  return result;
}

/** @fn
 * @brief 各種チェックのち、問題が無ければプロセスを作成
 * @param pageName ページ名
 * @param processName プロセス名
 * @param objectName オブジェクト名
 * @param type タッチ判定タイプ
 * @param enableOverBorder   （スワイプ専用）指が一度範囲外に出ても判定を継続するかどうか
 * @param returnCurrentOver （スワイプ専用）指が範囲外に出ていてもtrueを返すようにするかどうか
 * @return true / false （成功 / 失敗）
 */
bool TouchData::setTemplateProcess (String pageName, String processName, String objectName, touchType type, bool enableOverBorder, bool returnCurrentOver){
  if (vData == nullptr) {
    debugLog.printlnLog(Debug::error, "vData is nullptr");
    return false;
  }
  String createdProcess = isExistsProcessType(pageName, objectName, type);
  if(createdProcess != ""){
    debugLog.printlnLog(Debug::error, "Same type process created. (processName:"+ createdProcess +")");
    return false;
  }
  if(!vData->isExistsObjectToPage(pageName, objectName)){
    debugLog.printlnLog(Debug::error, 
                        vData->isExistsPage(pageName) ? 
                          "[" + objectName + "] was not created." : 
                          "[" + pageName + "] was not created.");
    return false;
  }
  JsonObject process = createOrGetProcess(pageName, processName);
  if (process.isNull()) {
      return false;
  }
  process["type"] = static_cast<int>(type);
  process["objectName"] = objectName;
  switch(type){
    case touchType::Flicking:
    case touchType::Flicked :
    case touchType::Dragging:
    case touchType::Dragged :
      process["enableOverBorder"] = enableOverBorder;
      process["returnCurrentOver"] = returnCurrentOver;
      break;
    default:
      break;
  }
  return true;
}

// ... (setReleaseProcessなどの各種 setXxxProcess 関数は変更なし) ...
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
  return setTemplateProcess(pageName, processName, objectName, touchType::Drag);
}
bool TouchData::setDraggingProcess (String pageName, String processName, String objectName, bool enableOverBorder, bool returnCurrentOver){
  return setTemplateProcess(pageName, processName, objectName, touchType::Dragging, enableOverBorder, returnCurrentOver);
}
bool TouchData::setDraggedProcess (String pageName, String processName, String objectName, bool enableOverBorder, bool returnCurrentOver){
  return setTemplateProcess(pageName, processName, objectName, touchType::Dragged, enableOverBorder, returnCurrentOver);
}

bool TouchData::setClickedProcess (String pageName, String processName, String objectName){
  return setTemplateProcess(pageName, processName, objectName, touchType::Clicked);
}
bool TouchData::setMultiClickedProcess (String pageName, String processName, String objectName, int count){
  if(setTemplateProcess(pageName, processName, objectName, touchType::MultiClicked)){
    // ここで processList[pageName][processName] が有効か確認
    JsonObject procObj = processList[pageName][processName].as<JsonObject>();
    if (procObj.isNull()) {
        debugLog.printlnLog(Debug::error, "[ERROR] MultiClickedProcess: Proc object is null after setTemplateProcess. Cannot set count."); // DebugLogを使用
        return false;
    }
    procObj["count"] = count;
    return true;
  }
  return false;
}


bool TouchData::enableProcess(bool isPress) {
  enabledProcessList.clear();
  enabledDoc.clear();
  enabledProcessList = enabledDoc.to<JsonObject>(); // 再度紐付け (念のため)

  if (processPage.isNull()) {
      debugLog.printlnLog(Debug::error, "[ERROR] enableProcess: processPage is null. Returning false."); // DebugLogを使用
      return false;
  }

  debugLog.printlnLog(Debug::info, "TouchData::enableProcess - Start checking processes for " + String(isPress ? "Press" : "Release") + " events.");

  for (JsonPair kv : processPage) {
    String procName = kv.key().c_str();
    JsonObject procData = kv.value().as<JsonObject>();
    if (procData.isNull()) {
        debugLog.printlnLog(Debug::error, String("[ERROR] enableProcess: procData is null for key: ") + procName); 
        continue; // 無効なデータはスキップ
    }
    int type = procData["type"].as<int>();
    String objectName = procData["objectName"].as<String>(); // オブジェクト名も表示

    debugLog.printlnLog(Debug::info, "TouchData::enableProcess - Checking process: " + procName + ", Object: " + objectName + ", Type: " + String(type));

    if (isPress) {
      if (type == static_cast<int>(touchType::Press)
        || type == static_cast<int>(touchType::Pressing)
        || type == static_cast<int>(touchType::Pressed)
        || type == static_cast<int>(touchType::Hold)
        || type == static_cast<int>(touchType::Holding)
        || type == static_cast<int>(touchType::Held)
        || type == static_cast<int>(touchType::Drag)
        || type == static_cast<int>(touchType::Dragging)
        || type == static_cast<int>(touchType::Dragged)
        || type == static_cast<int>(touchType::Flick)
        || type == static_cast<int>(touchType::Flicking)
        || type == static_cast<int>(touchType::Flicked)
        || type == static_cast<int>(touchType::Clicked)
        || type == static_cast<int>(touchType::MultiClicked)) {
        enabledProcessList[procName] = procData;
        debugLog.printlnLog(Debug::success, "TouchData::enableProcess - Enabled for Press: " + procName); // 有効化されたログ
      }
    } else { // isPress == false (つまりRelease時)
      if (type == static_cast<int>(touchType::Release)
        || type == static_cast<int>(touchType::Releasing)
        || type == static_cast<int>(touchType::Clicked) // Release時にClicked/MultiClickedも有効化
        || type == static_cast<int>(touchType::MultiClicked)
        || type == static_cast<int>(touchType::Flicked) // Flicked/DraggedもRelease時に有効化
        || type == static_cast<int>(touchType::Dragged)) {
        enabledProcessList[procName] = procData;
        debugLog.printlnLog(Debug::success, "TouchData::enableProcess - Enabled for Release: " + procName); // 有効化されたログ
      }
    }
  }
  debugLog.printlnLog(Debug::info, "TouchData::enableProcess - Finished checking processes.");
  return true;
}

bool TouchData::disableProcessType(touchType tType){
  if(processPage.isNull()) {
    debugLog.printlnLog(Debug::error, "[ERROR] disableProcessType: processPage is null. Returning false."); // DebugLogを使用
    return false;
  }
  
  std::vector<String> keysToRemove;
  for (JsonPair kv : enabledProcessList) {
      JsonObject processData = kv.value().as<JsonObject>();
      if (processData.isNull()) {
          debugLog.printlnLog(Debug::error, String("[ERROR] disableProcessType: processData is null for key: ") + kv.key().c_str()); 
          continue;
      }
      int tTypeInt = processData["type"].as<int>();
      if(tTypeInt == static_cast<int>(tType)){
          keysToRemove.push_back(kv.key().c_str());
          debugLog.printlnLog(Debug::info, "TouchData::disableProcessType - Marking for disable: " + String(kv.key().c_str()) + " (Type: " + String(tTypeInt) + ")"); // ログ追加
      }
  }

  for (const String& key : keysToRemove) {
      enabledProcessList.remove(key);
      debugLog.printlnLog(Debug::success, "TouchData::disableProcessType - Disabled process: " + key); // ログ追加
  }
  return true;
}

bool TouchData::disableProcess(String processName){
  enabledProcessList.remove(processName);
  debugLog.printlnLog(Debug::success, "TouchData::disableProcess - Disabled specific process: " + processName); // ログ追加
  return true;
}

#define SWITCH_CASE(type, action) case type: action break;

bool TouchData::judgeProcess(uint32_t x, uint32_t y){
  debugLog.printlnLog(Debug::info, "TouchData::judgeProcess - Judging touch at X: " + String(x) + ", Y: " + String(y));

  if (processPage.isNull()) {
      debugLog.printlnLog(Debug::error, "[ERROR] judgeProcess: processPage is null. Cannot judge process. Returning false."); 
      return false;
  }
  if (enabledProcessList.isNull()) {
      debugLog.printlnLog(Debug::error, "[ERROR] judgeProcess: enabledProcessList is null. Cannot judge process. Returning false."); 
      return false;
  }
  if (enabledProcessList.size() == 0) {
      debugLog.printlnLog(Debug::info, "TouchData::judgeProcess - No enabled processes to check. currentProcessName cleared."); // ログ追加
      currentProcessName = "";
      return false;
  }

  struct HitProcessInfo {
    String processName;
    String objectName;
    int zIndex;
    int type; // プロセスタイプも保持
  };
  std::vector<HitProcessInfo> hitProcesses;

  for(JsonPair process : enabledProcessList){
    judgeSprite->clear(BLACK);

    String processName = process.key().c_str();
    JsonObject processData = process.value().as<JsonObject>();
    if (processData.isNull()) {
        debugLog.printlnLog(Debug::error, String("[ERROR] judgeProcess: processData is null for key: ") + processName); 
        continue;
    }
    String objectName = processData["objectName"].as<String>();
    int processType = processData["type"].as<int>(); // プロセスタイプ取得
    
    // vData->getObjectDataの戻り値もチェック
    JsonObject objectData = vData->getObjectData(vData->getDrawingPage(), objectName);
    if (objectData.isNull()) {
        debugLog.printlnLog(Debug::error, String("[ERROR] judgeProcess: objectData is null for object: ") + objectName + String(" on page: ") + vData->getDrawingPage()); 
        continue;
    }

    VisualData::DrawType type = static_cast<VisualData::DrawType>(objectData["type"].as<int>());
    JsonArray args = objectData["args"].as<JsonArray>();
    if (args.isNull()) {
        debugLog.printlnLog(Debug::error, String("[ERROR] judgeProcess: args is null for object: ") + objectName); 
        continue;
    }

    int zIndex = objectData["zIndex"].is<int>() ? objectData["zIndex"].as<int>() : 0;

    debugLog.printlnLog(Debug::info, "TouchData::judgeProcess - Drawing object '" + objectName + "' for process '" + processName + "' (ZIndex: " + String(zIndex) + ")"); // ログ追加

    switch(type){
      SWITCH_CASE(VisualData::DrawType::DrawPixel, {
        judgeSprite->drawPixel(args[0].as<int32_t>(), args[1].as<int32_t>(), WHITE);
      });
      SWITCH_CASE(VisualData::DrawType::DrawLine, {
        judgeSprite->drawLine(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), WHITE);
      });
      SWITCH_CASE(VisualData::DrawType::DrawBezier, {
        judgeSprite->drawBezier(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), WHITE);
      });
      SWITCH_CASE(VisualData::DrawType::DrawWideLine, {
        judgeSprite->drawWideLine(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), WHITE);
      });

      SWITCH_CASE(VisualData::DrawType::DrawRect, {
        judgeSprite->fillRect(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), WHITE);
      });
      SWITCH_CASE(VisualData::DrawType::DrawRoundRect, {
        judgeSprite->fillRoundRect(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), WHITE);
      });
      SWITCH_CASE(VisualData::DrawType::DrawTriangle, {
        judgeSprite->fillTriangle(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), WHITE);
      });
      SWITCH_CASE(VisualData::DrawType::DrawCircle, {
        judgeSprite->fillCircle(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), WHITE);
      });
      SWITCH_CASE(VisualData::DrawType::DrawEllipse, {
        judgeSprite->fillEllipse(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), WHITE);
      });
      SWITCH_CASE(VisualData::DrawType::DrawArc, {
        judgeSprite->fillArc(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), WHITE);
      });
      SWITCH_CASE(VisualData::DrawType::DrawEllipseArc, {
        judgeSprite->fillEllipseArc(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), args[6].as<int32_t>(), args[7].as<int32_t>(), WHITE);
      });

      SWITCH_CASE(VisualData::DrawType::FillRect, {
        judgeSprite->fillRect(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), WHITE);
      });
      SWITCH_CASE(VisualData::DrawType::FillRoundRect, {
        judgeSprite->fillRoundRect(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), WHITE);
      });
      SWITCH_CASE(VisualData::DrawType::FillTriangle, {
        judgeSprite->fillTriangle(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), WHITE);
      });
      SWITCH_CASE(VisualData::DrawType::FillCircle, {
        judgeSprite->fillCircle(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), WHITE);
      });
      SWITCH_CASE(VisualData::DrawType::FillEllipse, {
        judgeSprite->fillEllipse(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), WHITE);
      });
      SWITCH_CASE(VisualData::DrawType::FillArc, {
        judgeSprite->fillArc(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), WHITE);
      });

      SWITCH_CASE(VisualData::DrawType::DrawJpgFile, {
        debugLog.printlnLog(Debug::info, "TouchData::judgeProcess - Skipping DrawJpgFile for touch check.");
      });
      SWITCH_CASE(VisualData::DrawType::DrawPngFile, {
        debugLog.printlnLog(Debug::info, "TouchData::judgeProcess - Skipping DrawPngFile for touch check.");
      });

      SWITCH_CASE(VisualData::DrawType::DrawString, {
        debugLog.printlnLog(Debug::info, "TouchData::judgeProcess - Skipping DrawString for touch check.");
        // judgeSprite->setTextColor(WHITE, WHITE); 
        // judgeSprite->setTextDatum(args[5].as<uint8_t>()); 
        // judgeSprite->drawString(args[0].as<String>(), args[1].as<int32_t>(), args[2].as<int32_t>());
      });

      SWITCH_CASE(VisualData::DrawType::ClipArc, {}); 
      SWITCH_CASE(VisualData::DrawType::ClipEllipseArc, {});
      SWITCH_CASE(VisualData::DrawType::ClipRect, {});
      SWITCH_CASE(VisualData::DrawType::ClipRoundRect, {});
      SWITCH_CASE(VisualData::DrawType::ClipCircle, {});
      SWITCH_CASE(VisualData::DrawType::ClipEllipse, {});
      SWITCH_CASE(VisualData::DrawType::ClipTriangle, {});

      SWITCH_CASE(VisualData::DrawType::FlexBox, { debugLog.printlnLog(Debug::info, "TouchData::judgeProcess - Skipping FlexBox for touch check."); });
      SWITCH_CASE(VisualData::DrawType::TableBox, { debugLog.printlnLog(Debug::info, "TouchData::judgeProcess - Skipping TableBox for touch check."); });
      default:
        debugLog.printlnLog(Debug::error, "[ERROR] Unknown or unimplemented draw type for touch check.");
        break;
    }

    // ★ ここでタッチがヒットしたかチェック
    int color = judgeSprite->readPixel(x, y);
    if (color != BLACK) {
      hitProcesses.push_back({processName, objectName, zIndex, processType}); // processTypeも保存
      debugLog.printlnLog(Debug::info, "TouchData::judgeProcess - HIT! Process: " + processName + ", Object: " + objectName + ", ZIndex: " + String(zIndex)); // ヒットログ
    } else {
      debugLog.printlnLog(Debug::info, "TouchData::judgeProcess - MISS. Process: " + processName + ", Object: " + objectName); // ミスログ
    }
  }
  
  if (!hitProcesses.empty()) {

    std::sort(hitProcesses.begin(), hitProcesses.end(), [&](const HitProcessInfo& a, const HitProcessInfo& b) {
        if (a.zIndex != b.zIndex) {
            return a.zIndex < b.zIndex; // ZIndexが大きい方が優先（降順）
        }
        return false; // ZIndexが同じ場合は順序を維持（または変更しない）
    });

    currentProcessName = hitProcesses.back().processName; 
    debugLog.printlnLog(Debug::success, "TouchData::judgeProcess - Found active process: " + currentProcessName + " (Object: " + hitProcesses[0].objectName + ", ZIndex: " + String(hitProcesses[0].zIndex) + ")"); // 最終判定ログ
    return true;
  }

  currentProcessName = "";
  debugLog.printlnLog(Debug::info, "TouchData::judgeProcess - No process found for current touch. currentProcessName cleared."); // ログ追加
  return false;
}

bool TouchData::update(){
  debugLog.printlnLog(Debug::info, "TouchData::update - Starting touch update cycle.");

  if(!M5.Touch.isEnabled()){
    debugLog.printlnLog(Debug::error, "Touch sensor is disabled. Returning false.");
    return false;
  }
  
  String drawingPageName = vData->getDrawingPage();

  setProcessPage(drawingPageName); 

  if(processPage.isNull()){
    debugLog.printlnLog(Debug::error, "[ERROR] TouchData::update - processPage is null after setProcessPage. This is the source of the error.");
    return false;
  }

  auto t = M5.Touch.getDetail(); // M5.Touch.getDetail() を使用して詳細なタッチ情報を取得
  
  // タッチの状態に基づいてプロセスを有効化
  // 例えば、指が押されている間はPress/Holding系のプロセスを有効化し、
  // 指が離された瞬間はRelease/Clicked/Flicked系のプロセスを有効化
  if (t.wasPressed()) {
      enableProcess(true); // Pressイベント時に有効になるプロセスを有効化
      debugLog.printlnLog(Debug::info, "TouchData::update - M5.Touch isPressed: true. Enabling Press-related processes.");
  } else if (t.wasReleased()) {
      enableProcess(false); // Releaseイベント時に有効になるプロセスを有効化
      debugLog.printlnLog(Debug::info, "TouchData::update - M5.Touch isReleased: true. Enabling Release-related processes.");
  } else {
      // タッチがない、またはホールド中だが新しいイベントが発生していない場合
      debugLog.printlnLog(Debug::info, "TouchData::update - No new touch event (isPressed/isReleased). Current touch state: Holding=" + String(t.isHolding()) + ", Pressed=" + String(t.isPressed()));
      currentProcessName = ""; // イベントがない場合はプロセス名をクリア
      return false; // アクティブなプロセスがないのでfalseを返す
  }


  if(t.isPressed()){ // isPressed が true の場合のみ、Flick/Hold/Drag の開始判定を行う
    if(t.wasFlickStart()){
      debugLog.printlnLog(Debug::info, "TouchData::update - Flick start detected. Disabling Hold/Drag/Click processes.");
      disableProcessType(touchType::Hold);
      disableProcessType(touchType::Holding);
      disableProcessType(touchType::Held);
      disableProcessType(touchType::Drag);
      disableProcessType(touchType::Dragging);
      disableProcessType(touchType::Dragged);
      disableProcessType(touchType::Clicked);
      disableProcessType(touchType::MultiClicked);
    }else if(t.wasHold()){
      debugLog.printlnLog(Debug::info, "TouchData::update - Hold detected.");
      disableProcessType(touchType::Flick);
      disableProcessType(touchType::Flicking);
      disableProcessType(touchType::Flicked);
      disableProcessType(touchType::Clicked);
      disableProcessType(touchType::MultiClicked);
      if(t.wasDragStart()){
        debugLog.printlnLog(Debug::info, "TouchData::update - Drag start detected. Disabling Hold processes.");
        disableProcessType(touchType::Hold); 
        disableProcessType(touchType::Holding);
        disableProcessType(touchType::Held);
      }
    }
  }
  
  // M5.Touch.getDetail() で取得した座標を使用
  bool judged = judgeProcess(t.x, t.y);
  
  // リリース時の追加処理は、enableProcess(false) で既にカバーされている
  // ここで currentProcessName がセットされるので、呼び出し元はそれを見れば良い
  
  debugLog.printlnLog(Debug::info, "TouchData::update - Finished touch update cycle. Judged: " + String(judged));
  return judged; // judgeProcessの結果を直接返す
}

String TouchData::getCurrentProcess(){
  return currentProcessName;
}

bool TouchData::isOverBorder(){
  // 現状では未実装または常にfalseを返す
  return false; 
}

bool TouchData::wasOverBorder(){
  // 現状では未実装または常にfalseを返す
  return false; 
}