/*
  検討：
  ・エラーログだけでなく作成ログ、削除ログなどの出力
  ・オブジェクト削除時、接続が完全に消えたかの確認
*/
#include "VisualData.hpp"

VisualData::VisualData(int docSize){
  visualData = new DynamicJsonDocument(docSize);
}

DynamicJsonDocument VisualData::getVisualData(){
  return *visualData;
};

bool VisualData::isExistsPage(String pageName){
  return visualData.containsKey(pageName);
}

bool VisualData::isExistsObject(String objectName){
  return visualData[editingPage].containsKey(objectName);
  
bool VisualData::isSet EditingPage(){
  return (editingPage != "");

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

void VisualData::addPage(String pageName){
  JsonObject Page = (*visualData)[pageName].to<JsonObject>();
  editingPage = Page;
  objectNum = 0;
};

bool VisualData::changeSettingPage(String pageName){
  if(obj.containsKey(pageName)){
    editingPage = pageName
};

void setParentObject(String objectName, String parentName);
/*
  親があるか確認、なければfalse
  子があるか確認、なければfalse
  あれば循環参照をチェック
  問題なければ親に子設定、子に親設定
  親の基準座標を「描画範囲を矩形に変換した右上」に置換
  子の各座標に親基準座標を足す
*/

void VisualData::setDrawPixelObject(String objectName, int32_t x, int32_t y, int color){
  objectNum++;
  JsonObject obj;
  if(objectName == ""){
    objectName = "obj-" + String(objectNum);
  }
  obj = editingPage[objectName].to<JsonObject>();
  obj["type"] = static_cast<int>(DrawType::DrawPixel);
  JsonArray args = obj["args"].to<JsonArray>();
  args.add(x);
  args.add(y);
  args.add(color);
};

void setDrawLineObject        (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1                          , int color);
void setDrawBezierObject      (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2  , int color);

void setDrawRectObject        (String objectName, int32_t x, int32_t y, int32_t w, int32_t h                              , int color);
void setFillRectObject        (String objectName, int32_t x, int32_t y, int32_t w, int32_t h                              , int color);
void setDrawRoundRectObject   (String objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r                   , int color);
void setFillRoundRectObject   (String objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r                   , int color);

void setDrawCircleObject      (String objectName, int32_t x, int32_t y                      , int32_t r                   , int color);
void setFillCircleObject      (String objectName, int32_t x, int32_t y                      , int32_t r                   , int color);
void setDrawEllipseObject     (String objectName, int32_t x, int32_t y, int32_t rx, int32_t ry                            , int color);
void setFillEllipseObject     (String objectName, int32_t x, int32_t y, int32_t rx, int32_t ry                            , int color);
void setDrawTriangleObject    (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2  , int color);
void setFillTriangleObject    (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2  , int color);

void setDrawArcObject         (String objectName, int32_t x, int32_t y, int32_t r0, int32_t r1, float angle0, float angle1, int color);
void setFillArcObject         (String objectName, int32_t x, int32_t y, int32_t r0, int32_t r1, float angle0, float angle1, int color);
void setDrawEllipseArcObject  (String objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, float angle0, float angle1, int color);
void setFillEllipseArcObject  (String objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, float angle0, float angle1, int color);

void setDrawJpgObject         (String objectName, fs::File *dataSource, const char *path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, lgfx::v1::jpeg_div::jpeg_div_t scale);
void setDrawPngObject         (String objectName, fs::File *dataSource, const char *path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, float scale_x, float scale_y);

void setDrawStringObject      (String objectName, String text, int32_t x, int32_t y, int color, int bgcolor, uint8_t datum);

void setFlexBoxObject(String objectName, int32_t x, int32_t y, int32_t w, int32_t h);
void setTableBoxObject(String objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t grid_x, int32_t grid_y);

// 並べるときの余白
/*
  0 autoSize        : 最小値 0 で自動調整
  10 autoSize       : 最小値 10 で自動調整
  10 10             : 最小値最大値を 10 で固定
  autoSize 10       : 最大値 10 で自動調整
  autoSize autoSize : 残りの余白を均等分割
*/
const int32_t autoSize = -1;
void setFlexMargin(int32_t min_x, int32_t max_x, int32_t min_y, int32_t max_y);
// 並べるときの基準点
void setContentPosition(uint8_t datum);


void drawPage(LGFX_Sprite &sprite, String pageName);