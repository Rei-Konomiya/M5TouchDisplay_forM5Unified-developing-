
/*
  検討：
  ・エラーログだけでなく作成ログ、削除ログなどの出力
  ・オブジェクト削除時、接続が完全に消えたかの確認
*/
#include "VisualData.hpp"

// コンストラクタ
VisualData::VisualData (LovyanGFX* parent, bool enableErrorLog, bool enableInfoLog, bool enableSuccessLog){
  debugLog.setDebug(enableErrorLog, enableInfoLog, enableSuccessLog);
  clipSprite = new LGFX_Sprite(parent);
}

/** @fn
 * @brief 描画データの全容を返す
 * @return 全体描画データ
 */
JsonDocument VisualData::getVisualData (){
  String jsonString;
  serializeJson(visualData, jsonString);
  debugLog.printlnLog(debugLog.info, jsonString);
  return visualData;
};


JsonObject VisualData::getObjectData(String pageName, String objectName){
  return visualData[pageName][objectName];
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
 * @param objectName 検索するオブジェクト名
 * @return ある=true : ない=false
 */
bool VisualData::isExistsObject (String objectName){
  return (*editingPage)[objectName].is<JsonObject>();
}

/** @fn
 * @brief 指定のページ内に指定のオブジェクトがあるかどうかを返す
 * @param pageName 検索するページ名
 * @param objectName 検索するオブジェクト名
 * @return 0/1/2 (ページ無 / ページ有・オブジェクト無 / ページ有・オブジェクト有)
 */
bool VisualData::isExistsObjectToPage (String pageName, String objectName){
  if(isExistsPage(pageName)){
    return visualData[pageName][objectName].is<JsonObject>();
  }
  return false;
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
//     debugLog.printlnLog(objectName +" does not exist. Please create the object first.");
//     return false;
//   }
//   if(!isExistsObject(parentName)){
//     debugLog.printlnLog(parentName +" does not exist. Please create the object first.");
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
    debugLog.printlnLog(debugLog.error, "[Editing Page] does not exist (Please set the page)");
    return 0;
  }
  if(isExistsObject(objectName)){
    debugLog.printlnLog(debugLog.info, "["+ objectName +"] already exists (Modifying the existing object)");
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
    debugLog.printlnLog(debugLog.error, "["+ pageName +"] does not exist.");
    return false;
  }
  editingPage = nullptr;
  visualData.remove(pageName);
  if((*editingPage)[pageName].is<JsonObject>()){
    debugLog.printlnLog(debugLog.error, "Unexpected error (page cannot be deleted)");
    return false;
  }
  return true;
}

bool VisualData::deleteObject (String objectName){
  if(!isExistsObject(objectName)){
    debugLog.printlnLog(debugLog.error, "["+ objectName +"] does not exist.");
    return false;
  }
  /*
  if(!isExistsKey(objectName, "child")){
    debugLog.printlnLog(debugLog.error, "["+ objectName +"] does not exist.");
    JsonArray childArray = (*editingPage)["child"].as<JsonArray>();
    for (JsonVariant value : childArray) {
      if(isExistsKey(value, "parent")){
        
      }
    }
  }
  */
  editingPage->remove("objectName");
  if((*editingPage)[objectName].is<JsonObject>()){
    debugLog.printlnLog(debugLog.error, "Unexpected error (page cannot be deleted)");
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
        obj = (*editingPage)[objectName];
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
bool VisualData::setDrawWideLineObject (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t r, int color){
  return createTemplateObject(objectName, static_cast<int>(DrawType::DrawWideLine), {x0, y0, x1, y1, r, color});
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

#define SWITCH_CASE(type, action) case type: action break;

bool VisualData::drawObject(LGFX_Sprite &sprite, DrawType type, JsonArray args){
  debugLog.printLog(debugLog.none, "\t\ttype: ");
  switch (type) {
    SWITCH_CASE(DrawType::DrawPixel, {
      debugLog.printlnLog(debugLog.none, "DrawPixel");
      sprite.drawPixel(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int>());
    });
    SWITCH_CASE(DrawType::DrawLine, {
      debugLog.printlnLog(debugLog.none, "DrawLine");
      sprite.drawLine(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int>());
    });
    SWITCH_CASE(DrawType::DrawBezier, {
      debugLog.printlnLog(debugLog.none, "DrawBezier");
      sprite.drawBezier(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), args[6].as<int>());
    });
    SWITCH_CASE(DrawType::DrawWideLine, {
      debugLog.printlnLog(debugLog.none, "DrawWideLine");
      sprite.drawWideLine(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int>());
    });

    SWITCH_CASE(DrawType::DrawRect, {
      debugLog.printlnLog(debugLog.none, "DrawRect");
      sprite.drawRect(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int>());
    });
    SWITCH_CASE(DrawType::DrawRoundRect, {
      debugLog.printlnLog(debugLog.none, "DrawRoundRect");
      sprite.drawRoundRect(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int>());
    });
    SWITCH_CASE(DrawType::DrawTriangle, {
      debugLog.printlnLog(debugLog.none, "DrawTriangle");
      sprite.drawTriangle(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), args[6].as<int>());
    });
    SWITCH_CASE(DrawType::DrawCircle, {
      debugLog.printlnLog(debugLog.none, "DrawCircle");
      sprite.drawCircle(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int>());
    });
    SWITCH_CASE(DrawType::DrawEllipse, {
      debugLog.printlnLog(debugLog.none, "DrawEllipse");
      sprite.drawEllipse(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int>());
    });
    SWITCH_CASE(DrawType::DrawArc, {
      debugLog.printlnLog(debugLog.none, "DrawArc");
      sprite.drawArc(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), args[6].as<int>());
    });
    SWITCH_CASE(DrawType::DrawEllipseArc, {
      debugLog.printlnLog(debugLog.none, "DrawEllipseArc");
      sprite.drawEllipseArc(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), args[6].as<int32_t>(), args[7].as<int32_t>(), args[8].as<int>());
    });

    SWITCH_CASE(DrawType::FillRect, {
      debugLog.printlnLog(debugLog.none, "FillRect");
      sprite.fillRect(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int>());
    });
    SWITCH_CASE(DrawType::FillRoundRect, {
      debugLog.printlnLog(debugLog.none, "FillRoundRect");
      sprite.fillRoundRect(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int>());
    });
    SWITCH_CASE(DrawType::FillTriangle, {
      debugLog.printlnLog(debugLog.none, "FillTriangle");
      sprite.fillTriangle(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), args[6].as<int>());
    });
    SWITCH_CASE(DrawType::FillCircle, {
      debugLog.printlnLog(debugLog.none, "FillCircle");
      sprite.fillCircle(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int>());
    });
    SWITCH_CASE(DrawType::FillEllipse, {
      debugLog.printlnLog(debugLog.none, "FillEllipse");
      sprite.fillEllipse(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int>());
    });
    SWITCH_CASE(DrawType::FillArc, {
      debugLog.printlnLog(debugLog.none, "FillArc");
      sprite.fillArc(args[0].as<int32_t>(), args[1].as<int32_t>(), args[2].as<int32_t>(), args[3].as<int32_t>(), args[4].as<int32_t>(), args[5].as<int32_t>(), args[6].as<int>());
    });

    SWITCH_CASE(DrawType::DrawJpgFile, {
      debugLog.printlnLog(debugLog.none, "DrawJpgFile");
    });
    SWITCH_CASE(DrawType::DrawPngFile, {
      debugLog.printlnLog(debugLog.none, "DrawPngFile");
    });

    SWITCH_CASE(DrawType::DrawString, {
      debugLog.printlnLog(debugLog.none, "DrawString");
      sprite.setTextColor(args[3].as<int32_t>(), args[4].as<int32_t>());
      sprite.setTextDatum(args[5].as<uint8_t>());
      sprite.drawString(args[0].as<String>(), args[1].as<int32_t>(), args[2].as<int32_t>());
    });

    SWITCH_CASE(DrawType::ClipArc, {
      debugLog.printlnLog(debugLog.none, "ClipArc");
    });
    SWITCH_CASE(DrawType::ClipEllipseArc, {
      debugLog.printlnLog(debugLog.none, "ClipEllipseArc");
    });
    SWITCH_CASE(DrawType::ClipRect, {
      debugLog.printlnLog(debugLog.none, "ClipRect");
    });
    SWITCH_CASE(DrawType::ClipRoundRect, {
      debugLog.printlnLog(debugLog.none, "ClipRoundRect");
    });
    SWITCH_CASE(DrawType::ClipCircle, {
      debugLog.printlnLog(debugLog.none, "ClipCircle");
    });
    SWITCH_CASE(DrawType::ClipEllipse, {
      debugLog.printlnLog(debugLog.none, "ClipEllipse");
    });
    SWITCH_CASE(DrawType::ClipTriangle, {
      debugLog.printlnLog(debugLog.none, "ClipTriangle");
    });

    SWITCH_CASE(DrawType::FlexBox, {
      debugLog.printlnLog(debugLog.none, "FlexBox");
    });
    SWITCH_CASE(DrawType::TableBox, {
      debugLog.printlnLog(debugLog.none, "TableBox");
    });
    default:
      Serial.println(F("Unknown draw type"));
      break;
  }
  Serial.print("\t\targs:");
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
  debugLog.printlnLog(debugLog.info, "drawing: "+drawingPageName);

  sprite.clear(BLACK);

  for(JsonPair object : pageObjects){
    JsonObject objData = object.value().as<JsonObject>();
    
    DrawType type = static_cast<DrawType>(objData["type"].as<int>());
    JsonArray args = objData["args"].as<JsonArray>();
    debugLog.printLog(debugLog.none, "\tdrawObject: ");
    debugLog.printlnLog(debugLog.none, object.key().c_str());
    drawObject(sprite, type, args);
  }

    // String jsonString;
    // serializeJson(objData, jsonString);
    // debugLog.printLog(debugLog.info, jsonString);
  debugLog.printlnLog(debugLog.none, "----------");
  return true;
}

/*
bool VisualData::getPageData (LGFX_Sprite &sprite, String fileName) {
  String path = "/pageData/" + fileName + ".png";

  if (!SD.exists(path)) return false;

  File file = SD.open(path);
  if (!file) return false;

  bool result = sprite.drawPng(&file, 0, 0, 0.25, 0.2, lgfx::top_left);
  file.close();

  return result;
}
*/

bool VisualData::screenShot (LGFX_Sprite &sprite, String fileName){
  bool result = false;
  size_t pngSize = 0;

  // 画面の中央部分を PNG 化する例
  int32_t x = 0;
  int32_t y = 0;
  int32_t w = sprite.width();
  int32_t h = sprite.height();

  void* pngData = sprite.createPng(&pngSize, x, y, w, h);

  if (pngData) {
    File myFile = SD.open("/pageData");
    if (myFile) {
      myFile.close();
      debugLog.printlnLog(debugLog.info, "sdフォルダ確認");
    } else {
      SD.mkdir("/pageData");
      debugLog.printlnLog(debugLog.success, "sdフォルダ作成完了");
    }

    // PNG データを SD カードに保存する
    File file = SD.open("/pageData/"+ fileName +".png", FILE_WRITE);
    if (file) {
      file.write((uint8_t*)pngData, pngSize);
      file.close();
      debugLog.printlnLog(debugLog.success, "PNG保存成功");
      result = true;
    } else {
      debugLog.printlnLog(debugLog.error, "PNG保存失敗");
    }
    free(pngData); // メモリを解放
  } else {
    debugLog.printlnLog(debugLog.error, "PNGエンコード失敗");
  }

  return result;
}