
/*
  検討：
  ・エラーログだけでなく作成ログ、削除ログなどの出力
  ・オブジェクト削除時、接続が完全に消えたかの確認
*/
#include "VisualData.hpp"

// コンストラクタ
VisualData::VisualData(int docSize, bool enableErrorLog, bool enableInfoLog, bool enableSuccessLog){
  visualData = new DynamicJsonDocument(docSize);
  debugLog.setDebug(enableErrorLog, enableInfoLog, enableSuccessLog);
}

/** @fn
 * @brief 描画データの全容を返す
 * @return 全体描画データ
 */
DynamicJsonDocument VisualData::getVisualData(){
  return *visualData;
};

/** @fn
 * @brief 指定のページがあるかどうかを返す
 * @param pageName 検索するページ名
 * @return ある=true : ない=false
 */
bool VisualData::isExistsPage(String pageName){
  return (*visualData).containsKey(pageName);
}

/** @fn
 * @brief 指定のオブジェクトが現在編集中のページ内にあるかどうかを返す
 * @param pageName 検索するオブジェクト名
 * @return ある=true : ない=false
 */
bool VisualData::isExistsObject(String objectName){
  return visualData[*editingPage].containsKey(objectName);
}

/** @fn
 * @brief 編集中のページがあるかどうかを返す
 * @return ある=true : ない=false
 */
bool VisualData::isSetEditingPage(){
  return (*editingPage);
}


/** @fn
 * @brief 新たにページを作成する
 * @param pageName ページ名
 * @return 成功=true : 失敗=false
 */
bool VisualData::addPage(String pageName){
  JsonObject Page = (*visualData)[pageName].to<JsonObject>();
  *editingPage = Page;
  objectNum = 0;
};

/** @fn
 * @brief 編集するページを切り替える
 * @param pageName ページ名
 * @return 成功=true : 失敗=false
 */
bool VisualData::changeSettingPage(String pageName){
  if((*visualData).containsKey(pageName)){
    editingPage = (*visualData)[pageName];
  }
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
//     (*editingPage)[parentName]["child"].to<JsonArray>();
//   }
//   (*editingPage)[parentName]["child"].add(objectName);
//   (*editingPage)[objectName]["parent"] = parentName;
// }
/*
  親があるか確認、なければfalse
  子があるか確認、なければfalse
  あれば循環参照をチェック
  問題なければ親に子設定、子に親設定
  親の基準座標を「描画範囲を矩形に変換した右上」に置換
  子の各座標に親基準座標を足す
*/


uint8_t VisualData::checkCreatable(String objectName){
  if(isSetEditingPage){
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
  deletePage(pageName)
    ページがあるか確認し、なければfalse
    あればページを消す
    全体からページが消えたか確認、あればfalse
    なければtrue
  deleteObject(objectName)
    オブジェクトがあるか確認し、なければfalse
    あればオブジェクトを参照
    オブジェクトの子要素をチェックし、子要素から親設定を解除
    親要素をチェックし、親要素から子設定を解除
    オブジェクトを削除
    ページ内にオブジェクト名が無いか確認、あればfalse
    なければtrue
*/
bool VisualData::deletePage(String pageName){
  if(!isExistsPage(pageName)){
    debugLog.printLog(debugLog.error, "["+ pageName +"] does not exist.");
    return false;
  }
  editingPage = nullptr;
  (*visualData).remove(pageName);
  if((*visualData).containsKey(pageName)){
    debugLog.printLog(debugLog.error, "Unexpected error (page cannot be deleted)");
  }
}
bool VisualData::deleteObject(String objectName){
  (*editingPage).remove(objectName);
}

bool VisualData::setDrawPixelObject(String objectName, int32_t x, int32_t y, int color){
  uint8_t mode = checkCreatable(objectName);
  if(mode){
    JsonObject obj;
    switch(mode){
      case 1:
        if(objectName == ""){
          objectName = "obj-" + String(++objectNum);
        }
        obj = (*editingPage)[objectName].to<JsonObject>();
        break;
      case 2:
        obj = (*editingPage)[objectName];
        break;
    }
    obj["type"] = static_cast<int>(DrawType::DrawPixel);
    JsonArray args = obj["args"].to<JsonArray>();
    args.add(x);
    args.add(y);
    args.add(color);
  }
};

bool setDrawLineObject        (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1                          , int color);
bool setDrawBezierObject      (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2  , int color);

bool setDrawRectObject        (String objectName, int32_t x, int32_t y, int32_t w, int32_t h                              , int color);
bool setFillRectObject        (String objectName, int32_t x, int32_t y, int32_t w, int32_t h                              , int color);
bool setDrawRoundRectObject   (String objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r                   , int color);
bool setFillRoundRectObject   (String objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r                   , int color);

bool setDrawCircleObject      (String objectName, int32_t x, int32_t y                      , int32_t r                   , int color);
bool setFillCircleObject      (String objectName, int32_t x, int32_t y                      , int32_t r                   , int color);
bool setDrawEllipseObject     (String objectName, int32_t x, int32_t y, int32_t rx, int32_t ry                            , int color);
bool setFillEllipseObject     (String objectName, int32_t x, int32_t y, int32_t rx, int32_t ry                            , int color);
bool setDrawTriangleObject    (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2  , int color);
bool setFillTriangleObject    (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2  , int color);

bool setDrawArcObject         (String objectName, int32_t x, int32_t y, int32_t r0, int32_t r1, float angle0, float angle1, int color);
bool setFillArcObject         (String objectName, int32_t x, int32_t y, int32_t r0, int32_t r1, float angle0, float angle1, int color);
bool setDrawEllipseArcObject  (String objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, float angle0, float angle1, int color);
bool setFillEllipseArcObject  (String objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, float angle0, float angle1, int color);

bool setDrawJpgObject         (String objectName, fs::File *dataSource, const char *path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, lgfx::v1::jpeg_div::jpeg_div_t scale);
bool setDrawPngObject         (String objectName, fs::File *dataSource, const char *path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, float scale_x, float scale_y);

bool setDrawStringObject      (String objectName, String text, int32_t x, int32_t y, int color, int bgcolor, uint8_t datum);

bool setFlexBoxObject(String objectName, int32_t x, int32_t y, int32_t w, int32_t h);
bool setTableBoxObject(String objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t grid_x, int32_t grid_y);

// 並べるときの余白
/*
  0 autoSize        : 最小値 0 で自動調整
  10 autoSize       : 最小値 10 で自動調整
  10 10             : 最小値最大値を 10 で固定
  autoSize 10       : 最大値 10 で自動調整
  autoSize autoSize : 残りの余白を均等分割
*/
const int32_t autoSize = -1;
bool setFlexMargin(int32_t min_x, int32_t max_x, int32_t min_y, int32_t max_y);
// 並べるときの基準点
bool setContentPosition(uint8_t datum);


bool drawPage(LGFX_Sprite &sprite, String pageName);