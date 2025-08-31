#include "VisualData.hpp"
using VDS = VisualDataSet;

// コンストラクタ
VisualData::VisualData (LovyanGFX* parent, bool enableErrorLog, bool enableInfoLog, bool enableSuccessLog){
  debugLog.setDebug(enableErrorLog, enableInfoLog, enableSuccessLog);
  clipSprite = new LGFX_Sprite(parent);
}


// ページ番号が visualDataSet に存在するか
bool VisualData::isExistsPage (int pageNum) const {
  for (const auto& page : visualDataSet.pages) {
    if (!page.isEmpty() && page.pageNum == pageNum) return true;
  }
  return false;
}
// ページ名の重複チェック
bool VisualData::isExistsPageName (const String& name) const {
  for (const auto& page : visualDataSet.pages) {
    if (page.pageName == name) return true;
  }
  return false;
}
// オブジェクト番号が編集ページ内に存在するか
bool VisualData::isExistsObject(int objNum, int pageNum) const {
  const VDS::PageData* page = nullptr;

  if (pageNum < 0) {
    page = &editingPage; // 編集中ページ
  } else {
    page = &getPageData(pageNum); // 指定ページ
    if (page->isEmpty()) return false;
  }

  for (const auto& obj : page->objects) {
    if (obj.objectNum == objNum) return true;
  }

  return false;
}
// ページ内に指定オブジェクト名が存在するかチェック
bool VisualData::isExistsObjectName(const String& objectName, int pageNum) const {
  const VDS::PageData* page = nullptr;

  if (pageNum < 0) {
    // 編集中ページを対象
    page = &editingPage;
  } else {
    // visualDataSet の指定ページを取得
    page = &getPageData(pageNum);
    if (page->isEmpty()) return false;
  }

  for (const auto& obj : page->objects) {
    if (obj.objectName == objectName) return true;
  }

  return false;
}

//bool VisualData::isExistsParent (int pageNum, int objNum) const {}
//bool VisualData::isExistsChild (int pageNum, int objNum) const {}

// 編集ページが存在するか（空ページでないか）
bool VisualData::isExistsEditingPage () const {
  return !editingPage.isEmpty();
}


// ページ名から pageNum を取得（存在しなければ -1）
int VisualData::getPageNumByName(const String& name) const {
  for (const auto& page : visualDataSet.pages) {
    if (page.pageName == name) return page.pageNum;
  }
  return -1;
}

// オブジェクト名から objectNum を取得（存在しなければ -1）
// pageNum が -1 の場合は編集中ページを対象
int VisualData::getObjectNumByName(const String& objectName, int pageNum) const {
  const VDS::PageData* page = nullptr;

  if (pageNum < 0) {
    page = &editingPage;
  } else {
    page = &getPageData(pageNum);
    if (!page || page->isEmpty()) return -1;
  }

  for (const auto& obj : page->objects) {
    if (obj.objectName == objectName) return obj.objectNum;
  }

  return -1;
}


// visualDataSet.pages の const 参照を返す
const std::vector<VDS::PageData>& VisualData::getVisualData() const {
  return visualDataSet.pages;
}
// pageNum が存在すれば参照を返し、なければダミーを返す
const VDS::PageData& VisualData::getPageData(int pageNum) const {
  if(pageNum < 0) return currentPageCopy;
  for (const auto& page : visualDataSet.pages) {
    if (!page.isEmpty() && page.pageNum == pageNum) return page;
  }

  static VDS::PageData dummyPage; // 念のため
  return dummyPage;
}
// objNum が存在すれば参照を返し、なければダミーを返す
const VDS::ObjectData& VisualData::getObjectData(const VDS::PageData& page, int objNum) const {
  if (page.isEmpty()) {
    static VDS::ObjectData dummyObj;
    return dummyObj;
  }

  for (const auto& obj : page.objects) {
    if (!obj.isEmpty() && obj.objectNum == objNum) return obj;
  }

  static VDS::ObjectData dummyObj; // 存在しない場合はダミー
  return dummyObj;
}

std::vector<VDS::PageData>& VisualData::getVisualDataRef() {
  return visualDataSet.pages;
}
VDS::PageData* VisualData::getPageDataRef(int pageNum) {
  if (!isExistsPage(pageNum)) return nullptr;  // 先に存在チェック
  auto& pages = getVisualDataRef();
  for (auto& page : pages) {
    if (!page.isEmpty() && page.pageNum == pageNum) return &page;
  }
  return nullptr;
}
VDS::ObjectData* VisualData::getObjectDataRef(VDS::PageData* page, int objNum) {
  if (!page || page->isEmpty()) return nullptr;  // 空ページや nullptr は存在しない
  for (auto& obj : page->objects) {
    if (!obj.isEmpty() && obj.objectNum == objNum) return &obj;
  }
  return nullptr;
}

// 一括変更モードの開始・終了
void VisualData::beginVisualUpdate () {
  isBatchUpdating = true;
}
void VisualData::endVisualUpdate () {
  commitVisualEdit();
  isBatchUpdating = false;
}

// 新規ページ追加
bool VisualData::addPage (const char* name, int pageNum) {

  // ページ番号自動割り当て
  if (pageNum < 0) {
    pageNum = lastAssignedPageNum + 1;
    while (isExistsPage(pageNum)) pageNum++;
    lastAssignedPageNum = pageNum;
  } else if (isExistsPage(pageNum)) {
    return false;  // 番号重複
  } else {
    lastAssignedPageNum = pageNum;
  }

  // ページ名自動生成
  String pageNameStr = name ? String(name) : "page" + String(pageNum);

  // 名前重複チェック
  if (isExistsPageName(pageNameStr)) return false;

  // 新規ページ作成
  VDS::PageData newPage;
  newPage.pageNum = pageNum;
  newPage.pageName = pageNameStr;

  visualDataSet.pages.push_back(newPage);

  return changeEditPage(pageNum);
}

// 現在の編集ページを visualDataSet に送る
bool VisualData::commitVisualEdit() {
  if (editingPage.isEmpty()) return false;

  auto* existingPage = getPageDataRef (editingPage.pageNum);
  if (existingPage) {
    *existingPage = editingPage;  // データ更新
  } else {
    visualDataSet.pages.push_back(editingPage);
  }

  editingPage = VDS::PageData(); // 編集ページリセット
  return true;
}

// 編集ページを切り替える
bool VisualData::changeEditPage (int pageNum) {
  // 編集中のデータを visualDataSet に同期
  commitVisualEdit();

  // 指定ページが存在するかチェック
  if(isExistsPage(pageNum)){
    auto target = getPageDataRef(pageNum);
    editingPage = *target; // 編集ページを切り替え
    return true;
  }

  return false;
}

// ページ削除
bool VisualData::deletePage (int pageNum) {
  for (auto it = visualDataSet.pages.begin(); it != visualDataSet.pages.end(); ++it) {
    if (it->pageNum == pageNum) {
      visualDataSet.pages.erase(it);

      // 削除したページが編集中だった場合は編集中をリセット
      if (editingPage.pageNum == pageNum) {
        editingPage = VDS::PageData();
      }
      return true;
    }
  }
  return false; // ページが見つからない
}

// 現在描画中のページ名を返す
String VisualData::getDrawingPage () const {
  if (currentPageCopy.isEmpty()) {
    return ""; // 何も描画していない場合
  }
  return currentPageCopy.pageName;
}


VDS::ObjectData VisualData::checkCreatable(const String& objectName, bool onDisplay) {
  VDS::ObjectData result;

  // 編集中ページが無い場合
  if (!isExistsEditingPage()) {
    debugLog.printlnLog(debugLog.error, "No editing page available.");
    return result;  // 空データ
  }

  auto& target = onDisplay ? currentPageCopy.objects : editingPage.objects;

  // 既存オブジェクトがある場合 → 上書き用に取得して返す
  for (auto& obj : target) {
    if (obj.objectName == objectName) {
      debugLog.printlnLog(debugLog.info, "[" + objectName + "] already exists. Will overwrite.");
      return obj;
    }
  }

  // 新規作成可能 → 空の ObjectData に名前をセットして返す
  result.objectName = objectName;
  debugLog.printlnLog(debugLog.info, "[" + objectName + "] is creatable (new object).");
  return result;
}

bool VisualData::deleteObject(const String& objectName, bool onDisplay) {
  if (!isExistsEditingPage()) {
    debugLog.printlnLog(debugLog.error, "No editing page available.");
    return false;
  }

  auto& objs = onDisplay ? currentPageCopy.objects : editingPage.objects;

  // オブジェクトが存在するか確認
  int objIndex = -1;
  for (size_t i = 0; i < objs.size(); i++) {
    if (objs[i].objectName == objectName) {
      objIndex = i;
      break;
    }
  }

  if (objIndex < 0) {
    debugLog.printlnLog(debugLog.error, "[" + objectName + "] does not exist.");
    return false;
  }

  // 削除処理（順番は保たれる）
  objs.erase(objs.begin() + objIndex);

  debugLog.printlnLog(debugLog.success, "[" + objectName + "] has been deleted.");
  return true;
}

bool VisualData::moveObject(const String& objectName, size_t newIndex, bool onDisplay) {
  if (!isExistsEditingPage()) {
    debugLog.printlnLog(debugLog.error, "No editing page available.");
    return false;
  }

  auto& objs = onDisplay ? currentPageCopy.objects : editingPage.objects;

  // 現在の位置を検索
  int currentIndex = -1;
  for (size_t i = 0; i < objs.size(); i++) {
    if (objs[i].objectName == objectName) {
      currentIndex = i;
      break;
    }
  }

  if (currentIndex < 0) {
    debugLog.printlnLog(debugLog.error, "[" + objectName + "] does not exist.");
    return false;
  }

  // 範囲チェック
  if (newIndex >= objs.size()) newIndex = objs.size() - 1;
  if (newIndex == currentIndex) return true; // すでにその位置

  // オブジェクトを取り出す
  VDS::ObjectData obj = objs[currentIndex];

  // 元の位置から削除
  objs.erase(objs.begin() + currentIndex);

  // 新しい位置に挿入
  objs.insert(objs.begin() + newIndex, obj);

  debugLog.printlnLog(debugLog.info, "[" + objectName + "] moved from " +
    String(currentIndex) + " to " + String(newIndex) + ".");

  return true;
}





VDS::ObjectData& VisualData::createOrUpdateObject (VDS::DrawType type, const String& objectName, const VDS::ObjectArgs& args, uint8_t zIndex, bool onDisplay) {
  VDS::PageData* targetPage = onDisplay ? &currentPageCopy : &editingPage;

  if (!targetPage || targetPage->isEmpty()) {
    static VDS::ObjectData dummy;
    debugLog.printlnLog(debugLog.error, "No target page available.");
    return dummy;
  }

  // 既存オブジェクトがある場合 → 上書き
  for (auto& obj : targetPage->objects) {
    if (obj.objectName == objectName) {
      obj.type = type;
      obj.objectArgs = args;
      obj.zIndex = zIndex;
      debugLog.printlnLog(debugLog.info, "[" + objectName + "] updated in place.");
      return obj;
    }
  }

  // 新規追加
  VDS::ObjectData newObj;
  newObj.objectNum  = targetPage->objects.size();
  newObj.objectName = objectName;
  newObj.type       = type;
  newObj.objectArgs = args;
  newObj.zIndex     = zIndex;

  targetPage->objects.push_back(newObj);
  return targetPage->objects.back();
}

VDS::ObjectData VisualData::setDrawPixelObject (const String& objectName, int32_t x, int32_t y, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;  // ← 1行
  args.pixel.x = x;
  args.pixel.y = y;
  args.pixel.color = color;
  return createOrUpdateObject(VDS::DrawType::DrawPixel, objectName, args, zIndex, onDisplay);
}

// 直線
VDS::ObjectData VisualData::setDrawLineObject (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.line.x0 = x0;
  args.line.y0 = y0;
  args.line.x1 = x1;
  args.line.y1 = y1;
  args.line.color = color;
  return createOrUpdateObject(VDS::DrawType::DrawLine, objectName, args, zIndex, onDisplay);
}

// ベジェ曲線
VDS::ObjectData VisualData::setDrawBezierObject (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.bezier.x0 = x0; args.bezier.y0 = y0;
  args.bezier.x1 = x1; args.bezier.y1 = y1;
  args.bezier.x2 = x2; args.bezier.y2 = y2;
  args.bezier.color = color;
  return createOrUpdateObject(VDS::DrawType::DrawBezier, objectName, args, zIndex, onDisplay);
}

// 太線
VDS::ObjectData VisualData::setDrawWideLineObject (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t r, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.wideLine.x0 = x0; args.wideLine.y0 = y0;
  args.wideLine.x1 = x1; args.wideLine.y1 = y1;
  args.wideLine.r = r;
  args.wideLine.color = color;
  return createOrUpdateObject(VDS::DrawType::DrawWideLine, objectName, args, zIndex, onDisplay);
}

// 矩形
VDS::ObjectData VisualData::setDrawRectObject (const String& objectName, int32_t x, int32_t y, int32_t w, int32_t h, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.rect.x = x; args.rect.y = y;
  args.rect.w = w; args.rect.h = h;
  args.rect.color = color;
  return createOrUpdateObject(VDS::DrawType::DrawRect, objectName, args, zIndex, onDisplay);
}
VDS::ObjectData VisualData::setFillRectObject (const String& objectName, int32_t x, int32_t y, int32_t w, int32_t h, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.rect.x = x; args.rect.y = y;
  args.rect.w = w; args.rect.h = h;
  args.rect.color = color;

  return createOrUpdateObject(VDS::DrawType::FillRect, objectName, args, zIndex, onDisplay);
}

// 角丸矩形
VDS::ObjectData VisualData::setDrawRoundRectObject (const String& objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.roundRect.x = x; args.roundRect.y = y;
  args.roundRect.w = w; args.roundRect.h = h;
  args.roundRect.r = r;
  args.roundRect.color = color;
  return createOrUpdateObject(VDS::DrawType::DrawRoundRect, objectName, args, zIndex, onDisplay);
}
VDS::ObjectData VisualData::setFillRoundRectObject (const String& objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.roundRect.x = x; args.roundRect.y = y;
  args.roundRect.w = w; args.roundRect.h = h;
  args.roundRect.r = r;
  args.roundRect.color = color;
  return createOrUpdateObject(VDS::DrawType::FillRoundRect, objectName, args, zIndex, onDisplay);
}

// 円
VDS::ObjectData VisualData::setDrawCircleObject (const String& objectName, int32_t x, int32_t y, int32_t r, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.circle.x = x; args.circle.y = y;
  args.circle.r = r;
  args.circle.color = color;
  return createOrUpdateObject(VDS::DrawType::DrawCircle, objectName, args, zIndex, onDisplay);
}
VDS::ObjectData VisualData::setFillCircleObject (const String& objectName, int32_t x, int32_t y, int32_t r, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.circle.x = x; args.circle.y = y;
  args.circle.r = r;
  args.circle.color = color;
  return createOrUpdateObject(VDS::DrawType::FillCircle, objectName, args, zIndex, onDisplay);
}

// 楕円
VDS::ObjectData VisualData::setDrawEllipseObject (const String& objectName, int32_t x, int32_t y, int32_t rx, int32_t ry, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.ellipse.x = x; args.ellipse.y = y;
  args.ellipse.rx = rx; args.ellipse.ry = ry;
  args.ellipse.color = color;
  return createOrUpdateObject(VDS::DrawType::DrawEllipse, objectName, args, zIndex, onDisplay);
}
VDS::ObjectData VisualData::setFillEllipseObject (const String& objectName, int32_t x, int32_t y, int32_t rx, int32_t ry, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.ellipse.x = x; args.ellipse.y = y;
  args.ellipse.rx = rx; args.ellipse.ry = ry;
  args.ellipse.color = color;
  return createOrUpdateObject(VDS::DrawType::FillEllipse, objectName, args, zIndex, onDisplay);
}

// 三角形
VDS::ObjectData VisualData::setDrawTriangleObject (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.triangle.x0 = x0; args.triangle.y0 = y0;
  args.triangle.x1 = x1; args.triangle.y1 = y1;
  args.triangle.x2 = x2; args.triangle.y2 = y2;
  args.triangle.color = color;
  return createOrUpdateObject(VDS::DrawType::DrawTriangle, objectName, args, zIndex, onDisplay);
}
VDS::ObjectData VisualData::setFillTriangleObject (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.triangle.x0 = x0; args.triangle.y0 = y0;
  args.triangle.x1 = x1; args.triangle.y1 = y1;
  args.triangle.x2 = x2; args.triangle.y2 = y2;
  args.triangle.color = color;
  return createOrUpdateObject(VDS::DrawType::FillTriangle, objectName, args, zIndex, onDisplay);
}

// アーク
VDS::ObjectData VisualData::setDrawArcObject(const String& objectName, int32_t x, int32_t y, int32_t r0, int32_t r1, int32_t angle0, int32_t angle1, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.arc.x = x; args.arc.y = y;
  args.arc.r0 = r0; args.arc.r1 = r1;
  args.arc.angle0 = angle0; args.arc.angle1 = angle1;
  args.arc.color = color;
  return createOrUpdateObject(VDS::DrawType::DrawArc, objectName, args, zIndex, onDisplay);
}

VDS::ObjectData VisualData::setFillArcObject(const String& objectName, int32_t x, int32_t y, int32_t r0, int32_t r1, int32_t angle0, int32_t angle1, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.arc.x = x; args.arc.y = y;
  args.arc.r0 = r0; args.arc.r1 = r1;
  args.arc.angle0 = angle0; args.arc.angle1 = angle1;
  args.arc.color = color;
  return createOrUpdateObject(VDS::DrawType::FillArc, objectName, args, zIndex, onDisplay);
}

VDS::ObjectData VisualData::setDrawEllipseArcObject(const String& objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, int32_t angle0, int32_t angle1, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.ellipseArc.x = x; args.ellipseArc.y = y;
  args.ellipseArc.r0x = r0x; args.ellipseArc.r1x = r1x;
  args.ellipseArc.r0y = r0y; args.ellipseArc.r1y = r1y;
  args.ellipseArc.angle0 = angle0; args.ellipseArc.angle1 = angle1;
  args.ellipseArc.color = color;
  return createOrUpdateObject(VDS::DrawType::DrawEllipseArc, objectName, args, zIndex, onDisplay);
}

VDS::ObjectData VisualData::setFillEllipseArcObject(const String& objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, int32_t angle0, int32_t angle1, int color, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.ellipseArc.x = x; args.ellipseArc.y = y;
  args.ellipseArc.r0x = r0x; args.ellipseArc.r1x = r1x;
  args.ellipseArc.r0y = r0y; args.ellipseArc.r1y = r1y;
  args.ellipseArc.angle0 = angle0; args.ellipseArc.angle1 = angle1;
  args.ellipseArc.color = color;
  return createOrUpdateObject(VDS::DrawType::FillEllipseArc, objectName, args, zIndex, onDisplay);
}


// JPEG画像
VDS::ObjectData VisualData::setDrawJpgFileObject (const String& objectName, VDS::DataType dataSource, const char* path,
                                                            int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight,
                                                            int32_t offX, int32_t offY, float scaleX, float scaleY, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.jpg.dataSource = dataSource;
  args.jpg.path = path;
  args.jpg.x = x; args.jpg.y = y;
  args.jpg.maxWidth = maxWidth; args.jpg.maxHeight = maxHeight;
  args.jpg.offX = offX; args.jpg.offY = offY;
  args.jpg.scaleX = scaleX; args.jpg.scaleY = scaleY;

  // サイズ取得
  int originalWidth, originalHeight;
  if (getJpgSize(SD, path, originalWidth, originalHeight)) {
    int w  = min(int(originalWidth * scaleX), maxWidth);
    int h = min(int(originalHeight * scaleY), maxHeight);
    args.jpg.w = w; args.jpg.h = h;
  } else {
    args.jpg.w = 0; args.jpg.h = 0;
  }

  return createOrUpdateObject(VDS::DrawType::DrawJpgFile, objectName, args, zIndex, onDisplay);
}

// PNG画像
VDS::ObjectData VisualData::setDrawPngFileObject (const String& objectName, VDS::DataType dataSource, const char* path,
                                                            int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight,
                                                            int32_t offX, int32_t offY, float scaleX, float scaleY, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.png.dataSource = dataSource;
  args.png.path = path;
  args.png.x = x; args.png.y = y;
  args.png.maxWidth = maxWidth; args.png.maxHeight = maxHeight;
  args.png.offX = offX; args.png.offY = offY;
  args.png.scaleX = scaleX; args.png.scaleY = scaleY;

  int originalWidth, originalHeight;
  File pngFile = SD.open(args.png.path);
  if (pngFile) {
    if (getPngSize(pngFile, originalWidth, originalHeight)) {
      int w  = min(int(originalWidth  * scaleX), maxWidth);
      int h = min(int(originalHeight * scaleY), maxHeight);
      args.png.w  = w; args.png.h = h;
    }
    pngFile.close();
  }

  return createOrUpdateObject(VDS::DrawType::DrawPngFile, objectName, args, zIndex, onDisplay);
}

// 文字
VDS::ObjectData VisualData::setDrawStringObject (const String& objectName, int32_t x, int32_t y, const char* text, int color, int bgcolor, uint8_t zIndex, bool onDisplay) {;
  VDS::ObjectArgs args;
  args.text.x = x; args.text.y = y;
  args.text.text = text;
  args.text.color = color; args.text.bgcolor = bgcolor;
  return createOrUpdateObject(VDS::DrawType::DrawString, objectName, args, zIndex, onDisplay);
}

bool VisualData::getJpgSize (fs::FS &fs, const char* filename, int &w, int &h) {
  File jpgFile = fs.open(filename);
  if (!jpgFile) return 0;
  if (!JpegDec.decodeSdFile(jpgFile)) {
    jpgFile.close();
    return false;
  }
  w = JpegDec.width;
  h = JpegDec.height;

  jpgFile.close();
  return true;
}

// 描画しないダミーコールバック
void VisualData::pngle_on_draw(pngle_t *png, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const uint8_t *rgba) {
    // 何もしない
}

// PNG ヘッダ読み取り用コールバック
void VisualData::pngle_on_header(pngle_t *png, uint32_t w, uint32_t h) {
  g_pngWidth  = w;
  g_pngHeight = h;
}

bool VisualData::getPngSize(File &file, int &width, int &height) {
  g_pngWidth = 0;
  g_pngHeight = 0;

  pngle_t *pngle = pngle_new();
  if (!pngle) return false;

  pngle_set_draw_callback(pngle, pngle_on_draw);
  pngle_set_init_callback(pngle, pngle_on_header);

  uint8_t buf[1024];
  int remain = 0;

  while (file.available()) {
    int len = file.read(buf + remain, sizeof(buf) - remain);
    if (len <= 0) break;
    int fed = pngle_feed(pngle, buf, remain + len);
    if (fed < 0) {
      Serial.printf("pngle error: %s\n", pngle_error(pngle));
      pngle_destroy(pngle);
      return false;
    }
    remain = remain + len - fed;
    if (g_pngWidth > 0 && g_pngHeight > 0) break;
  }

  pngle_destroy(pngle);

  width  = g_pngWidth;
  height = g_pngHeight;

  return (width > 0 && height > 0);
}



bool VisualData::drawObject (LGFX_Sprite &sprite, const VDS::ObjectData &obj) {
  switch (obj.type) {

    // -------------------- 基本描画 --------------------
    case VDS::DrawType::DrawPixel:
      sprite.drawPixel(obj.objectArgs.pixel.x, obj.objectArgs.pixel.y, obj.objectArgs.pixel.color);
      break;

    case VDS::DrawType::DrawLine:
      Serial.println(F("drawing DrawLine"));
      sprite.drawLine(obj.objectArgs.line.x0, obj.objectArgs.line.y0,
                      obj.objectArgs.line.x1, obj.objectArgs.line.y1,
                      obj.objectArgs.line.color);
      break;

    case VDS::DrawType::DrawBezier:
      Serial.println(F("drawing DrawBezier"));
      sprite.drawBezier(obj.objectArgs.bezier.x0, obj.objectArgs.bezier.y0,
                        obj.objectArgs.bezier.x1, obj.objectArgs.bezier.y1,
                        obj.objectArgs.bezier.x2, obj.objectArgs.bezier.y2,
                        obj.objectArgs.bezier.color);
      break;

    case VDS::DrawType::DrawWideLine:
      Serial.println(F("drawing DrawWideLine"));
      sprite.drawWideLine(obj.objectArgs.wideLine.x0, obj.objectArgs.wideLine.y0,
                          obj.objectArgs.wideLine.x1, obj.objectArgs.wideLine.y1,
                          obj.objectArgs.wideLine.r, obj.objectArgs.wideLine.color);
      break;

    case VDS::DrawType::DrawRect:
      Serial.println(F("drawing DrawRect"));
      sprite.drawRect(obj.objectArgs.rect.x, obj.objectArgs.rect.y,
                      obj.objectArgs.rect.w, obj.objectArgs.rect.h,
                      obj.objectArgs.rect.color);
      break;

    case VDS::DrawType::DrawRoundRect:
      Serial.println(F("drawing DrawRoundRect"));
      sprite.drawRoundRect(obj.objectArgs.roundRect.x, obj.objectArgs.roundRect.y,
                            obj.objectArgs.roundRect.w, obj.objectArgs.roundRect.h,
                            obj.objectArgs.roundRect.r, obj.objectArgs.roundRect.color);
      break;

    case VDS::DrawType::DrawCircle:
      Serial.println(F("drawing DrawCircle"));
      sprite.drawCircle(obj.objectArgs.circle.x, obj.objectArgs.circle.y,
                        obj.objectArgs.circle.r, obj.objectArgs.circle.color);
      break;

    case VDS::DrawType::DrawEllipse:
      Serial.println(F("drawing DrawEllipse"));
      sprite.drawEllipse(obj.objectArgs.ellipse.x, obj.objectArgs.ellipse.y,
                          obj.objectArgs.ellipse.rx, obj.objectArgs.ellipse.ry,
                          obj.objectArgs.ellipse.color);
      break;

    case VDS::DrawType::DrawTriangle:
      Serial.println(F("drawing DrawTriangle"));
      sprite.drawTriangle(obj.objectArgs.triangle.x0, obj.objectArgs.triangle.y0,
                          obj.objectArgs.triangle.x1, obj.objectArgs.triangle.y1,
                          obj.objectArgs.triangle.x2, obj.objectArgs.triangle.y2,
                          obj.objectArgs.triangle.color);
      break;

    // -------------------- 塗りつぶし --------------------
    case VDS::DrawType::FillRect:
      Serial.println(F("drawing FillRect"));
      sprite.fillRect(obj.objectArgs.rect.x, obj.objectArgs.rect.y,
                      obj.objectArgs.rect.w, obj.objectArgs.rect.h,
                      obj.objectArgs.rect.color);
      break;

    case VDS::DrawType::FillRoundRect:
      Serial.println(F("drawing FillRoundRect"));
      sprite.fillRoundRect(obj.objectArgs.roundRect.x, obj.objectArgs.roundRect.y,
                            obj.objectArgs.roundRect.w, obj.objectArgs.roundRect.h,
                            obj.objectArgs.roundRect.r, obj.objectArgs.roundRect.color);
      break;

    case VDS::DrawType::FillCircle:
      Serial.println(F("drawing FillCircle"));
      sprite.fillCircle(obj.objectArgs.circle.x, obj.objectArgs.circle.y,
                        obj.objectArgs.circle.r, obj.objectArgs.circle.color);
      break;

    case VDS::DrawType::FillTriangle:
      Serial.println(F("drawing FillTriangle"));
      sprite.fillTriangle(obj.objectArgs.triangle.x0, obj.objectArgs.triangle.y0,
                          obj.objectArgs.triangle.x1, obj.objectArgs.triangle.y1,
                          obj.objectArgs.triangle.x2, obj.objectArgs.triangle.y2,
                          obj.objectArgs.triangle.color);
      break;

    case VDS::DrawType::FillEllipse:
      Serial.println(F("drawing FillEllipse"));
      sprite.fillEllipse(obj.objectArgs.ellipse.x, obj.objectArgs.ellipse.y,
                          obj.objectArgs.ellipse.rx, obj.objectArgs.ellipse.ry,
                          obj.objectArgs.ellipse.color);
      break;

    case VDS::DrawType::FillArc:
      Serial.println(F("drawing FillArc"));
      sprite.fillArc(obj.objectArgs.arc.x, obj.objectArgs.arc.y,
                      obj.objectArgs.arc.r0, obj.objectArgs.arc.r1,
                      obj.objectArgs.arc.angle0, obj.objectArgs.arc.angle1,
                      obj.objectArgs.arc.color);
      break;

    case VDS::DrawType::FillEllipseArc:
      Serial.println(F("drawing FillEllipseArc"));
      sprite.fillEllipseArc(obj.objectArgs.ellipseArc.x, obj.objectArgs.ellipseArc.y,
                            obj.objectArgs.ellipseArc.r0x, obj.objectArgs.ellipseArc.r1x,
                            obj.objectArgs.ellipseArc.r0y, obj.objectArgs.ellipseArc.r1y,
                            obj.objectArgs.ellipseArc.angle0, obj.objectArgs.ellipseArc.angle1,
                            obj.objectArgs.ellipseArc.color);
      break;

    // -------------------- 画像描画 --------------------
    case VDS::DrawType::DrawJpgFile:
      Serial.println(F("drawing DrawJpgFile"));
      if (obj.objectArgs.jpg.path != nullptr) {
        if (obj.objectArgs.jpg.dataSource == VDS::DataType::SD) {
          File f = SD.open(obj.objectArgs.jpg.path);
          if (f) {
            sprite.drawJpg(&f,
                          obj.objectArgs.jpg.x, obj.objectArgs.jpg.y,
                          obj.objectArgs.jpg.maxWidth, obj.objectArgs.jpg.maxHeight,
                          obj.objectArgs.jpg.offX, obj.objectArgs.jpg.offY,
                          obj.objectArgs.jpg.scaleX, obj.objectArgs.jpg.scaleY);
            f.close();
          } else {
            Serial.printf("Failed to open JPG: %s\n", obj.objectArgs.jpg.path);
          }
        }
      }
      break;

    case VDS::DrawType::DrawPngFile:
      Serial.println(F("drawing DrawPngFile"));
      if (obj.objectArgs.png.path != nullptr) {
        if (obj.objectArgs.png.dataSource == VDS::DataType::SD) {
          File f = SD.open(obj.objectArgs.png.path);
          if (f) {
            sprite.drawPng(&f,
                          obj.objectArgs.png.x, obj.objectArgs.png.y,
                          obj.objectArgs.png.maxWidth, obj.objectArgs.png.maxHeight,
                          obj.objectArgs.png.offX, obj.objectArgs.png.offY,
                          obj.objectArgs.png.scaleX, obj.objectArgs.png.scaleY);
            f.close();
          } else {
            Serial.printf("Failed to open PNG: %s\n", obj.objectArgs.png.path);
          }
        }
      }
      break;

    // -------------------- 文字描画 --------------------
    case VDS::DrawType::DrawString:
      Serial.println(F("drawing DrawString"));
      sprite.setTextColor(obj.objectArgs.text.color, obj.objectArgs.text.bgcolor);
      sprite.drawString(obj.objectArgs.text.text,
                        obj.objectArgs.text.x, obj.objectArgs.text.y);
      break;

    // -------------------- Clip系（範囲制限） --------------------
    case VDS::DrawType::ClipArc:
    case VDS::DrawType::ClipEllipseArc:
    case VDS::DrawType::ClipRect:
    case VDS::DrawType::ClipRoundRect:
    case VDS::DrawType::ClipCircle:
    case VDS::DrawType::ClipEllipse:
    case VDS::DrawType::ClipTriangle:
      Serial.println(F("Clip type not implemented"));
      break;

    // -------------------- コンテナ系 --------------------
    case VDS::DrawType::FlexBox:
    case VDS::DrawType::TableBox:
      Serial.println(F("Container type not implemented"));
      break;

    default:
      Serial.println(F("Unknown DrawType"));
      return false;
  }

  return true;
}


bool VisualData::drawPage(LGFX_Sprite &sprite, const String pageName) {

  VDS::PageData page;
  page = getPageData(getPageNumByName(pageName));
  if (page.isEmpty()) return false;

  currentPageCopy = page;
  Serial.printf("Drawing page: %s\n", pageName.c_str());
  Serial.printf("Number of objects: %d\n", currentPageCopy.objects.size());

  // Z-indexで安定ソート
  std::vector<VDS::ObjectData> sortedObjects = currentPageCopy.objects;
  std::stable_sort(sortedObjects.begin(), sortedObjects.end(),
                    [](const VDS::ObjectData &a, const VDS::ObjectData &b) {
                      return a.zIndex < b.zIndex;
                    });

  Serial.println("clear Display");
  sprite.fillSprite(BLACK);

  // 描画
  for (auto &obj : sortedObjects) {
    drawObject(sprite, obj);
  }

  return true;
}



void VisualData::finalizeSetup () {
  endVisualUpdate();
}