#include "VisualData.hpp"

// コンストラクタ
VisualData::VisualData (LovyanGFX* parent, bool enableErrorLog, bool enableInfoLog, bool enableSuccessLog){
  debugLog.setDebug(enableErrorLog, enableInfoLog, enableSuccessLog);
  clipSprite = new LGFX_Sprite(parent);
}


// ページ番号が dataset に存在するか
bool VisualData::isExistsPage (int pageNum) const {
  for (const auto& page : dataset.pages) {
    if (!page.isEmpty() && page.pageNum == pageNum) return true;
  }
  return false;
}
// ページ名の重複チェック
bool VisualData::isExistsPageName (const String& name) const {
  for (const auto& page : dataset.pages) {
    if (page.pageName == name) return true;
  }
  return false;
}
// オブジェクト番号が編集ページ内に存在するか
bool VisualData::isExistsObject(int objNum, int pageNum) const {
    const VisualDataSet::PageData* page = nullptr;

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
    const VisualDataSet::PageData* page = nullptr;

    if (pageNum < 0) {
        // 編集中ページを対象
        page = &editingPage;
    } else {
        // dataset の指定ページを取得
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
    for (const auto& page : dataset.pages) {
        if (page.pageName == name) return page.pageNum;
    }
    return -1;
}

// オブジェクト名から objectNum を取得（存在しなければ -1）
// pageNum が -1 の場合は編集中ページを対象
int VisualData::getObjectNumByName(const String& objectName, int pageNum) const {
    const VisualDataSet::PageData* page = nullptr;

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


// dataset.pages の const 参照を返す
const std::vector<VisualDataSet::PageData>& VisualData::getVisualData() const {
  return dataset.pages;
}
// pageNum が存在すれば参照を返し、なければダミーを返す
const VisualDataSet::PageData& VisualData::getPageData(int pageNum) const {
  for (const auto& page : dataset.pages) {
    if (!page.isEmpty() && page.pageNum == pageNum) return page;
  }

  static VisualDataSet::PageData dummyPage; // 念のため
  return dummyPage;
}
// objNum が存在すれば参照を返し、なければダミーを返す
const VisualDataSet::ObjectData& VisualData::getObjectData(const VisualDataSet::PageData& page, int objNum) const {
  if (page.isEmpty()) {
    static VisualDataSet::ObjectData dummyObj;
    return dummyObj;
  }

  for (const auto& obj : page.objects) {
    if (!obj.isEmpty() && obj.objectNum == objNum) return obj;
  }

  static VisualDataSet::ObjectData dummyObj; // 存在しない場合はダミー
  return dummyObj;
}

std::vector<VisualDataSet::PageData>& VisualData::getVisualDataRef() {
  return dataset.pages;
}
VisualDataSet::PageData* VisualData::getPageDataRef(int pageNum) {
  if (!isExistsPage(pageNum)) return nullptr;  // 先に存在チェック
  auto& pages = getVisualDataRef();
  for (auto& page : pages) {
    if (!page.isEmpty() && page.pageNum == pageNum) return &page;
  }
  return nullptr;
}
VisualDataSet::ObjectData* VisualData::getObjectDataRef(VisualDataSet::PageData* page, int objNum) {
  if (!page || page->isEmpty()) return nullptr;  // 空ページや nullptr は存在しない
  for (auto& obj : page->objects) {
    if (!obj.isEmpty() && obj.objectNum == objNum) return &obj;
  }
  return nullptr;
}

// 一括変更モードの開始・終了
void VisualData::beginPageUpdate () {
  isBatchUpdating = true;
}
void VisualData::endPageUpdate () {
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
  VisualDataSet::PageData newPage;
  newPage.pageNum = pageNum;
  newPage.pageName = pageNameStr;

  dataset.pages.push_back(newPage);

  return changeEditPage(pageNum);
}

// 現在の編集ページを dataset に送る
bool VisualData::commitEditingPage() {
  if (editingPage.isEmpty()) return false;

  auto* existingPage = getPageDataRef (editingPage.pageNum);
  if (existingPage) {
    *existingPage = editingPage;  // データ更新
  } else {
    dataset.pages.push_back(editingPage);
  }

  editingPage = VisualDataSet::PageData(); // 編集ページリセット
  return true;
}

// 編集ページを切り替える
bool VisualData::changeEditPage (int pageNum) {
  // 編集中のデータを dataset に同期
  commitEditingPage();

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
  for (auto it = dataset.pages.begin(); it != dataset.pages.end(); ++it) {
    if (it->pageNum == pageNum) {
      dataset.pages.erase(it);

      // 削除したページが編集中だった場合は編集中をリセット
      if (editingPage.pageNum == pageNum) {
        editingPage = VisualDataSet::PageData();
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



// 新規オブジェクト作成/取得（暫定版）
VisualDataSet::ObjectData VisualData::createObject (VisualDataSet::DrawType type, const String& objectName, const VisualDataSet::ObjectArgs& args, uint8_t zIndex) {
  if (!isExistsEditingPage()) {
    debugLog.printlnLog(debugLog.error, "No editing page set.");
    return VisualDataSet::ObjectData(); // 空データ返却
  }

  // オブジェクト番号自動割り当て
  int objNum = 0;
  for (const auto& obj : editingPage.objects) {
    if (obj.objectNum >= objNum) objNum = obj.objectNum + 1;
  }

  // オブジェクト名自動生成
  String name = objectName;
  if (name == "") {
    name = "obj-" + String(objNum);
  }

  // 新規 ObjectData 作成
  VisualDataSet::ObjectData newObj;
  newObj.objectNum = objNum;
  newObj.objectName = name;
  newObj.type = type;
  newObj.objectArgs = args;
  newObj.zIndex = zIndex;

  // 編集ページに追加
  editingPage.objects.push_back(newObj);

  return newObj;
}

VisualDataSet::ObjectData VisualData::setDrawPixelObject (const String& objectName, int32_t x, int32_t y, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;  // ← 1行
  args.pixel.x = x;
  args.pixel.y = y;
  args.pixel.color = color;
  return createObject(VisualDataSet::DrawType::DrawPixel, objectName, args, zIndex);
}

// 直線
VisualDataSet::ObjectData VisualData::setDrawLineObject (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.line.x0 = x0;
  args.line.y0 = y0;
  args.line.x1 = x1;
  args.line.y1 = y1;
  args.line.color = color;
  return createObject(VisualDataSet::DrawType::DrawLine, objectName, args, zIndex);
}

// ベジェ曲線
VisualDataSet::ObjectData VisualData::setDrawBezierObject (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.bezier.x0 = x0; args.bezier.y0 = y0;
  args.bezier.x1 = x1; args.bezier.y1 = y1;
  args.bezier.x2 = x2; args.bezier.y2 = y2;
  args.bezier.color = color;
  return createObject(VisualDataSet::DrawType::DrawBezier, objectName, args, zIndex);
}

// 太線
VisualDataSet::ObjectData VisualData::setDrawWideLineObject (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t r, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.wideLine.x0 = x0; args.wideLine.y0 = y0;
  args.wideLine.x1 = x1; args.wideLine.y1 = y1;
  args.wideLine.r = r;
  args.wideLine.color = color;
  return createObject(VisualDataSet::DrawType::DrawWideLine, objectName, args, zIndex);
}

// 矩形
VisualDataSet::ObjectData VisualData::setDrawRectObject (const String& objectName, int32_t x, int32_t y, int32_t w, int32_t h, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.rect.x = x; args.rect.y = y;
  args.rect.w = w; args.rect.h = h;
  args.rect.color = color;
  return createObject(VisualDataSet::DrawType::DrawRect, objectName, args, zIndex);
}
VisualDataSet::ObjectData VisualData::setFillRectObject (const String& objectName, int32_t x, int32_t y, int32_t w, int32_t h, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.rect.x = x; args.rect.y = y;
  args.rect.w = w; args.rect.h = h;
  args.rect.color = color;
  return createObject(VisualDataSet::DrawType::FillRect, objectName, args, zIndex);
}

// 角丸矩形
VisualDataSet::ObjectData VisualData::setDrawRoundRectObject (const String& objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.roundRect.x = x; args.roundRect.y = y;
  args.roundRect.w = w; args.roundRect.h = h;
  args.roundRect.r = r;
  args.roundRect.color = color;
  return createObject(VisualDataSet::DrawType::DrawRoundRect, objectName, args, zIndex);
}
VisualDataSet::ObjectData VisualData::setFillRoundRectObject (const String& objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.roundRect.x = x; args.roundRect.y = y;
  args.roundRect.w = w; args.roundRect.h = h;
  args.roundRect.r = r;
  args.roundRect.color = color;
  return createObject(VisualDataSet::DrawType::FillRoundRect, objectName, args, zIndex);
}

// 円
VisualDataSet::ObjectData VisualData::setDrawCircleObject (const String& objectName, int32_t x, int32_t y, int32_t r, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.circle.x = x; args.circle.y = y;
  args.circle.r = r;
  args.circle.color = color;
  return createObject(VisualDataSet::DrawType::DrawCircle, objectName, args, zIndex);
}
VisualDataSet::ObjectData VisualData::setFillCircleObject (const String& objectName, int32_t x, int32_t y, int32_t r, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.circle.x = x; args.circle.y = y;
  args.circle.r = r;
  args.circle.color = color;
  return createObject(VisualDataSet::DrawType::FillCircle, objectName, args, zIndex);
}

// 楕円
VisualDataSet::ObjectData VisualData::setDrawEllipseObject (const String& objectName, int32_t x, int32_t y, int32_t rx, int32_t ry, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.ellipse.x = x; args.ellipse.y = y;
  args.ellipse.rx = rx; args.ellipse.ry = ry;
  args.ellipse.color = color;
  return createObject(VisualDataSet::DrawType::DrawEllipse, objectName, args, zIndex);
}
VisualDataSet::ObjectData VisualData::setFillEllipseObject (const String& objectName, int32_t x, int32_t y, int32_t rx, int32_t ry, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.ellipse.x = x; args.ellipse.y = y;
  args.ellipse.rx = rx; args.ellipse.ry = ry;
  args.ellipse.color = color;
  return createObject(VisualDataSet::DrawType::FillEllipse, objectName, args, zIndex);
}

// 三角形
VisualDataSet::ObjectData VisualData::setDrawTriangleObject (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.triangle.x0 = x0; args.triangle.y0 = y0;
  args.triangle.x1 = x1; args.triangle.y1 = y1;
  args.triangle.x2 = x2; args.triangle.y2 = y2;
  args.triangle.color = color;
  return createObject(VisualDataSet::DrawType::DrawTriangle, objectName, args, zIndex);
}
VisualDataSet::ObjectData VisualData::setFillTriangleObject (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.triangle.x0 = x0; args.triangle.y0 = y0;
  args.triangle.x1 = x1; args.triangle.y1 = y1;
  args.triangle.x2 = x2; args.triangle.y2 = y2;
  args.triangle.color = color;
  return createObject(VisualDataSet::DrawType::FillTriangle, objectName, args, zIndex);
}

// アーク
VisualDataSet::ObjectData VisualData::setDrawArcObject(const String& objectName, int32_t x, int32_t y, int32_t r0, int32_t r1, int32_t angle0, int32_t angle1, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.arc.x = x; args.arc.y = y;
  args.arc.r0 = r0; args.arc.r1 = r1;
  args.arc.angle0 = angle0; args.arc.angle1 = angle1;
  args.arc.color = color;
  return createObject(VisualDataSet::DrawType::DrawArc, objectName, args, zIndex);
}

VisualDataSet::ObjectData VisualData::setFillArcObject(const String& objectName, int32_t x, int32_t y, int32_t r0, int32_t r1, int32_t angle0, int32_t angle1, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.arc.x = x; args.arc.y = y;
  args.arc.r0 = r0; args.arc.r1 = r1;
  args.arc.angle0 = angle0; args.arc.angle1 = angle1;
  args.arc.color = color;
  return createObject(VisualDataSet::DrawType::FillArc, objectName, args, zIndex);
}

VisualDataSet::ObjectData VisualData::setDrawEllipseArcObject(const String& objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, int32_t angle0, int32_t angle1, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.ellipseArc.x = x; args.ellipseArc.y = y;
  args.ellipseArc.r0x = r0x; args.ellipseArc.r1x = r1x;
  args.ellipseArc.r0y = r0y; args.ellipseArc.r1y = r1y;
  args.ellipseArc.angle0 = angle0; args.ellipseArc.angle1 = angle1;
  args.ellipseArc.color = color;
  return createObject(VisualDataSet::DrawType::DrawEllipseArc, objectName, args, zIndex);
}

VisualDataSet::ObjectData VisualData::setFillEllipseArcObject(const String& objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, int32_t angle0, int32_t angle1, int color, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.ellipseArc.x = x; args.ellipseArc.y = y;
  args.ellipseArc.r0x = r0x; args.ellipseArc.r1x = r1x;
  args.ellipseArc.r0y = r0y; args.ellipseArc.r1y = r1y;
  args.ellipseArc.angle0 = angle0; args.ellipseArc.angle1 = angle1;
  args.ellipseArc.color = color;
  return createObject(VisualDataSet::DrawType::FillEllipseArc, objectName, args, zIndex);
}


// JPEG画像
VisualDataSet::ObjectData VisualData::setDrawJpgFileObject (const String& objectName, VisualDataSet::DataType dataSource, const char* path,
                                                            int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight,
                                                            int32_t offX, int32_t offY, lgfx::v1::jpeg_div::jpeg_div_t scale, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.jpg.dataSource = dataSource;
  args.jpg.path = path;
  args.jpg.x = x; args.jpg.y = y;
  args.jpg.maxWidth = maxWidth; args.jpg.maxHeight = maxHeight;
  args.jpg.offX = offX; args.jpg.offY = offY;
  args.jpg.scale = scale;
  return createObject(VisualDataSet::DrawType::DrawJpgFile, objectName, args, zIndex);
}

// PNG画像
VisualDataSet::ObjectData VisualData::setDrawPngFileObject (const String& objectName, VisualDataSet::DataType dataSource, const char* path,
                                                            int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight,
                                                            int32_t offX, int32_t offY, float scaleX, float scaleY, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.png.dataSource = dataSource;
  args.png.path = path;
  args.png.x = x; args.png.y = y;
  args.png.maxWidth = maxWidth; args.png.maxHeight = maxHeight;
  args.png.offX = offX; args.png.offY = offY;
  args.png.scaleX = scaleX; args.png.scaleY = scaleY;
  return createObject(VisualDataSet::DrawType::DrawPngFile, objectName, args, zIndex);
}

// 文字
VisualDataSet::ObjectData VisualData::setDrawStringObject (const String& objectName, int32_t x, int32_t y, const char* text, int color, int bgcolor, uint8_t zIndex) {
  VisualDataSet::ObjectArgs args;
  args.text.x = x; args.text.y = y;
  args.text.text = text;
  args.text.color = color; args.text.bgcolor = bgcolor;
  return createObject(VisualDataSet::DrawType::DrawString, objectName, args, zIndex);
}



/*
  checkCreatable  // 編集中ページの有無とオブジェクト重複確認（新規・更新・不可）
  deleteObject    // 編集中ページから指定オブジェクトの削除

  drawObject  // オブジェクトごとに描画
  drawPage    // ページ単位で描画
*/

void VisualData::finalizeSetup () {
  endPageUpdate();
}