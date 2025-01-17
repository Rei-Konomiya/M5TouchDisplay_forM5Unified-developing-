
/*
  検討：
  ・エラーログだけでなく作成ログ、削除ログなどの出力
  ・オブジェクト削除時、接続が完全に消えたかの確認
*/
#include "VisualData.hpp"

// コンストラクタ
VisualData::VisualData (bool enableErrorLog, bool enableInfoLog, bool enableSuccessLog){
  debugLog.setDebug(enableErrorLog, enableInfoLog, enableSuccessLog);
}

/** @fn
 * @brief 描画データの全容を返す
 * @return 全体描画データ
 */
JsonDocument VisualData::getVisualData (){
  String jsonString;
  serializeJson(visualData, jsonString);
  debugLog.printLog(debugLog.info, jsonString);
  return visualData;
};

/** @fn
 * @brief 指定のページがあるかどうかを返す
 * @param pageName 検索するページ名
 * @return ある=true : ない=false
 */
bool VisualData::isExistsPage (String pageName){
  return visualData[pageName].is<JsonObject>();
}

/** @fn
 * @brief 指定のオブジェクトが現在編集中のページ内にあるかどうかを返す
 * @param pageName 検索するオブジェクト名
 * @return ある=true : ない=false
 */
bool VisualData::isExistsObject (String objectName){
  return visualData[objectName].is<JsonObject>();
}

/** @fn
 * @brief 指定のオブジェクトが現在編集中のページ内にあるかどうかを返す
 * @param objectName 検索するオブジェクト名
 * @param keyName 検索するオブジェクト名
 * @return ある=true : ない=false
 */
bool VisualData::isExistsKey (String objectName, String keyName){
  if(isExistsObject(objectName)){
  (*editingPage)[objectName].is<JsonObject>();
    return false;
  }
  return (*editingPage)[objectName][keyName].is<JsonObject>();
}

/** @fn
 * @brief 編集中のページがあるかどうかを返す
 * @return ある=true : ない=false
 */
bool VisualData::isSetEditingPage (){
  return editingPage != nullptr;
}


/** @fn
 * @brief 新たにページを作成する
 * @param pageName ページ名
 * @return 成功=true : 失敗=false
 */
bool VisualData::addPage (String pageName) {
  JsonObject page = visualData[pageName].to<JsonObject>();
  *editingPage = page;
  objectNum = 0;
  return true; // 成功時はtrueを返す
}

/** @fn
 * @brief 編集するページを切り替える
 * @param pageName ページ名
 * @return 成功=true : 失敗=false
 */
bool VisualData::changeSettingPage (String pageName){
  if(visualData[pageName].is<JsonObject>()){
    (*editingPage) = visualData[pageName];
  }
  return (*editingPage) == visualData[pageName];
};

// /** @fn
//  * @brief 親オブジェクトを設定する
//  * @param pageName ページ名
//  * @return 成功=true : 失敗=false
//  */
// bool VisualData::setParentObject(String objectName, String parentName){
//   if(!isExistsObject(objectName)){
//     debugLog.printLog(objectName +" does not exist. Please create the object first.");
//     return false;
//   }
//   if(!isExistsObject(parentName)){
//     debugLog.printLog(parentName +" does not exist. Please create the object first.");
//     return false;
//   }
//   if(!(*editingPage)[parentName].containsKey("child")){
// (*editingPage)[parentName]["child"].to<JsonArray>();
//   }
// (*editingPage)[parentName]["child"].add(objectName);
// (*editingPage)[objectName]["parent"] = parentName;
// }
/*
  親があるか確認、なければfalse
  子があるか確認、なければfalse
  あれば循環参照をチェック
  問題なければ親に子設定、子に親設定
  親の基準座標を「描画範囲を矩形に変換した右上」に置換
  子の各座標に親基準座標を足す
*/


uint8_t VisualData::checkCreatable (String objectName){
  if(!isSetEditingPage()){
    debugLog.printLog(debugLog.error, "[Editing Page] does not exist (Please set the page)");
    return 0;
  }
  if(isExistsObject(objectName)){
    debugLog.printLog(debugLog.info, "["+ objectName +"] already exists (Modifying the existing object)");
    return 2;
  }
  return 1;
}

/*
  deleteKey(objectName, key)
    オブジェクトがあるか確認し、無ければfalse
    あればオブジェクトを参照
    オブジェクトにキーがあるか確認、無ければtrue(「なかった」ログ)
    あればキーを削除
    trueを返す
  deleteArrayItem(objectName, key, value)
    オブジェクトがあるか確認し、無ければfalse
    あればオブジェクトを参照
    オブジェクトにキーがあるか確認、無ければtrue(「なかった」ログ)
    あれば配列を参照
    線形探索し、なければtrue(「なかった」ログ)
    あればvalueを削除
    trueを返す
  deletePage(pageName)
    ページがあるか確認し、なければfalse
    あればページを消す
    trueを返す
  deleteObject(objectName)
    オブジェクトがあるか確認し、なければfalse
    あればオブジェクトを参照
    オブジェクトの親オブジェクトがあるか確認
    親要素をチェックし、親要素から子設定を解除
    オブジェクトを削除
    trueを返す
*/
bool VisualData::deletePage (String pageName){
  if(!isExistsPage(pageName)){
    debugLog.printLog(debugLog.error, "["+ pageName +"] does not exist.");
    return false;
  }
  editingPage = nullptr;
  visualData.remove(pageName);
  if((*editingPage)[pageName].is<JsonObject>()){
    debugLog.printLog(debugLog.error, "Unexpected error (page cannot be deleted)");
    return false;
  }
  return true;
}

bool VisualData::deleteObject (String objectName){
  if(!isExistsObject(objectName)){
    debugLog.printLog(debugLog.error, "["+ objectName +"] does not exist.");
    return false;
  }
  /*
  if(!isExistsKey(objectName, "child")){
    debugLog.printLog(debugLog.error, "["+ objectName +"] does not exist.");
    JsonArray childArray = (*editingPage)["child"].as<JsonArray>();
    for (JsonVariant value : childArray) {
      if(isExistsKey(value, "parent")){
        
      }
    }
  }
  */
  (*editingPage).remove("objectName");
  if((*editingPage)[objectName].is<JsonObject>()){
    debugLog.printLog(debugLog.error, "Unexpected error (page cannot be deleted)");
    return false;
  }
  return true;
}

bool VisualData::createTemplateObject (String objectName, int drawType, std::initializer_list<int> argsList){
  uint8_t mode = checkCreatable(objectName);
  if(mode){
    JsonObject obj;

    // オブジェクト作成処理
    switch(mode){
      case 1: // 新規オブジェクト作成
        if(objectName == ""){
          objectName = "obj-" + String(++objectNum);
        }
        obj = (*editingPage)[objectName].to<JsonObject>();
        break;

      case 2: // 既存オブジェクト取得
        obj = (*editingPage)[objectName].to<JsonObject>();
        break;
    }

    // データ設定
    obj["type"] = drawType;
    JsonArray objArgs = (*editingPage)[objectName]["args"].to<JsonArray>(); // 新規配列を作成
    for (auto value : argsList) {
      objArgs.add(value);
    }
    return true; // 成功時はtrueを返す
  }
  return false; // モードが0の場合など、失敗時はfalseを返す
}

bool VisualData::setDrawPixelObject (String objectName, int32_t x, int32_t y, int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::DrawPixel), {x, y, color});
}

bool VisualData::setDrawLineObject (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::DrawLine), {x0, y0, x1, y1, color});
}

bool VisualData::setDrawBezierObject (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2  , int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::DrawBezier), {x0, y0, x1, y1, x2, y2, color});
}

bool VisualData::setDrawRectObject (String objectName, int32_t x, int32_t y, int32_t w, int32_t h, int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::DrawRect), {x, y, w, h, color});
}

bool VisualData::setFillRectObject (String objectName, int32_t x, int32_t y, int32_t w, int32_t h, int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::FillRect), {x, y, w, h, color});
}

bool VisualData::setDrawRoundRectObject (String objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::DrawRoundRect), {x, y, w, h, r, color});
}

bool VisualData::setFillRoundRectObject (String objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::FillRoundRect), {x, y, w, h, r, color});
}


bool VisualData::setDrawCircleObject (String objectName, int32_t x, int32_t y, int32_t r, int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::DrawCircle), {x, y, r, color});
}

bool VisualData::setFillCircleObject (String objectName, int32_t x, int32_t y, int32_t r, int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::FillCircle), {x, y, r, color});
}

bool VisualData::setDrawEllipseObject (String objectName, int32_t x, int32_t y, int32_t rx, int32_t ry, int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::DrawEllipse), {x, y, rx, ry, color});
}

bool VisualData::setFillEllipseObject (String objectName, int32_t x, int32_t y, int32_t rx, int32_t ry, int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::FillEllipse), {x, y, rx, ry, color});
}

bool VisualData::setDrawTriangleObject (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2 , int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::DrawTriangle), {x0, y0, x1, y1, x2, y2, color});
}

bool VisualData::setFillTriangleObject (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2 , int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::FillTriangle), {x0, y0, x1, y1, x2, y2, color});
}


bool VisualData::setDrawArcObject (String objectName, int32_t x, int32_t y, int32_t r0, int32_t r1, int32_t angle0, int32_t angle1, int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::FillTriangle), {x, y, r0, r1, angle0, angle1, color});
}

bool VisualData::setFillArcObject (String objectName, int32_t x, int32_t y, int32_t r0, int32_t r1, int32_t angle0, int32_t angle1, int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::FillTriangle), {x, y, r0, r1, angle0, angle1, color});
}

bool VisualData::setDrawEllipseArcObject (String objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, int32_t angle0, int32_t angle1, int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::FillTriangle), {x, y, r0x, r1x, r0y, r1y, angle0, angle1, color});
}

bool VisualData::setFillEllipseArcObject (String objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, int32_t angle0, int32_t angle1, int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::FillTriangle), {x, y, r0x, r1x, r0y, r1y, angle0, angle1, color});
}


bool VisualData::setDrawJpgFileObject (String objectName,dataType dataSource, const char *path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, lgfx::v1::jpeg_div::jpeg_div_t scale){
  uint8_t mode = checkCreatable(objectName);
  if(mode){
    JsonObject obj;

    // オブジェクト作成処理
    switch(mode){
      case 1: // 新規オブジェクト作成
        if(objectName == ""){
          objectName = "obj-" + String(++objectNum);
        }
        obj = (*editingPage)[objectName].to<JsonObject>();
        break;

      case 2: // 既存オブジェクト取得
        obj = (*editingPage)[objectName].to<JsonObject>();
        break;
    }

    // データ設定
    obj["type"] = static_cast<int>(DrawType::DrawJpgFile);
    JsonArray objArgs = (*editingPage)["args"].to<JsonArray>(); // 新規配列を作成
    // String dataSource, const char *path, 
    // int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, 
    // int32_t offX, int32_t offY, float scale_x, float scale_y
    objArgs.add(static_cast<int>(dataSource));
    objArgs.add(path);
    objArgs.add(x);
    objArgs.add(y);
    objArgs.add(maxWidth);
    objArgs.add(maxHeight);
    objArgs.add(offX);
    objArgs.add(offY);
    objArgs.add(static_cast<int>(scale));
    return true; // 成功時はtrueを返す
  }
  return false; // モードが0の場合など、失敗時はfalseを返す
}

bool VisualData::setDrawPngFileObject (String objectName, dataType dataSource, const char *path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, float scale_x, float scale_y){
  uint8_t mode = checkCreatable(objectName);
  if(mode){
    JsonObject obj;

    // オブジェクト作成処理
    switch(mode){
      case 1: // 新規オブジェクト作成
        if(objectName == ""){
          objectName = "obj-" + String(++objectNum);
        }
        obj = (*editingPage)[objectName].to<JsonObject>();
        break;

      case 2: // 既存オブジェクト取得
        obj = (*editingPage)[objectName].to<JsonObject>();
        break;
    }

    // データ設定
    obj["type"] = static_cast<int>(DrawType::DrawPngFile);
    JsonArray objArgs = (*editingPage)["args"].to<JsonArray>(); // 新規配列を作成
    // String dataSource, const char *path, 
    // int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, 
    // int32_t offX, int32_t offY, float scale_x, float scale_y
    objArgs.add(static_cast<int>(dataSource));
    objArgs.add(path);
    objArgs.add(x);
    objArgs.add(y);
    objArgs.add(maxWidth);
    objArgs.add(maxHeight);
    objArgs.add(offX);
    objArgs.add(offY);
    objArgs.add(scale_x);
    objArgs.add(scale_y);
    return true; // 成功時はtrueを返す
  }
  return false; // モードが0の場合など、失敗時はfalseを返す
}


// bool VisualData::setDrawStringObject (String objectName, String text, int32_t x, int32_t y, int color, int bgcolor, uint8_t datum){
//   return createTemplateObject(objectName, static_cast<int>(DrawType::FillTriangle), {x0, y0, x1, y1, x2, y2, color});
// }


// bool VisualData::setFlexBoxObject(String objectName, int32_t x, int32_t y, int32_t w, int32_t h);
// bool VisualData::setTableBoxObject(String objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t grid_x, int32_t grid_y);

// 並べるときの余白
/*
  0 autoSize        : 最小値 0 で自動調整
  10 autoSize       : 最小値 10 で自動調整
  10 10             : 最小値最大値を 10 で固定
  autoSize 10       : 最大値 10 で自動調整
  autoSize autoSize : 残りの余白を均等分割
*/
//bool setFlexMargin (int32_t min_x, int32_t max_x, int32_t min_y, int32_t max_y);
// 並べるときの基準点
//bool setContentPosition (uint8_t datum);

String VisualData::getDrawingPage (){
  return drawingPageName;
}

#define SWITCH_CASE(type, action) case type: { action; } break;

bool VisualData::drawObject(LGFX_Sprite &sprite, DrawType type, JsonArray args){
  Serial.print("\ttype:");
  switch (type) {
    SWITCH_CASE(DrawType::DrawPixel, Serial.println(F("DrawPixel")));
    SWITCH_CASE(DrawType::DrawLine, Serial.println(F("DrawLine")));
    SWITCH_CASE(DrawType::DrawBezier, Serial.println(F("DrawBezier")));
    SWITCH_CASE(DrawType::DrawWideLine, Serial.println(F("DrawWideLine")));

    SWITCH_CASE(DrawType::DrawRect, Serial.println(F("DrawRect")));
    SWITCH_CASE(DrawType::DrawRoundRect, Serial.println(F("DrawRoundRect")));
    SWITCH_CASE(DrawType::DrawTriangle, Serial.println(F("DrawTriangle")));
    SWITCH_CASE(DrawType::DrawCircle, Serial.println(F("DrawCircle")));
    SWITCH_CASE(DrawType::DrawEllipse, Serial.println(F("DrawEllipse")));
    SWITCH_CASE(DrawType::DrawArc, Serial.println(F("DrawArc")));
    SWITCH_CASE(DrawType::DrawEllipseArc, Serial.println(F("DrawEllipseArc")));

    SWITCH_CASE(DrawType::FillRect, Serial.println(F("FillRect")));
    SWITCH_CASE(DrawType::FillRoundRect, Serial.println(F("FillRoundRect")));
    SWITCH_CASE(DrawType::FillTriangle, Serial.println(F("FillTriangle")));
    SWITCH_CASE(DrawType::FillCircle, Serial.println(F("FillCircle")));
    SWITCH_CASE(DrawType::FillEllipse, Serial.println(F("FillEllipse")));
    SWITCH_CASE(DrawType::FillArc, Serial.println(F("FillArc")));

    SWITCH_CASE(DrawType::DrawJpgFile, Serial.println(F("DrawJpgFile")));
    SWITCH_CASE(DrawType::DrawPngFile, Serial.println(F("DrawPngFile")));

    SWITCH_CASE(DrawType::DrawString, Serial.println(F("DrawString")));

    SWITCH_CASE(DrawType::ClipArc, Serial.println(F("ClipArc")));
    SWITCH_CASE(DrawType::ClipEllipseArc, Serial.println(F("ClipEllipseArc")));
    SWITCH_CASE(DrawType::ClipRect, Serial.println(F("ClipRect")));
    SWITCH_CASE(DrawType::ClipRoundRect, Serial.println(F("ClipRoundRect")));
    SWITCH_CASE(DrawType::ClipCircle, Serial.println(F("ClipCircle")));
    SWITCH_CASE(DrawType::ClipEllipse, Serial.println(F("ClipEllipse")));
    SWITCH_CASE(DrawType::ClipTriangle, Serial.println(F("ClipTriangle")));

    SWITCH_CASE(DrawType::FlexBox, Serial.println(F("FlexBox")));
    SWITCH_CASE(DrawType::TableBox, Serial.println(F("TableBox")));
    default:
      Serial.println(F("Unknown draw type"));
      break;
  }
  Serial.print("\targs:");
  String jsonString;
  serializeJson(args, jsonString);
  Serial.println(jsonString);
  return true;
}

bool VisualData::drawPage (LGFX_Sprite &sprite, String pageName){
  if(visualData[pageName].is<JsonObject>()){
    drawingPageName = pageName;
  }else{
    return false;
  }

  JsonObject pageObjects = visualData[drawingPageName].as<JsonObject>();
  for(JsonPair object : pageObjects){
    JsonObject objData = object.value().as<JsonObject>();
    
    DrawType type = static_cast<DrawType>(objData["type"].as<int>());
    JsonArray args = objData["args"].as<JsonArray>();

    debugLog.printLog(debugLog.info, "drawObject:");
    Serial.print("\tobjectName:");
    Serial.println(object.key().c_str());
    drawObject(sprite, type, args);
  }

    // String jsonString;
    // serializeJson(objData, jsonString);
    // debugLog.printLog(debugLog.info, jsonString);
  Serial.println();
  return true;
}