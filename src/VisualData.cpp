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
bool VisualData::isExistsObject(int objNum, int pageNum = -1) const {
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
bool VisualData::isExistsObjectName(const String& objectName, int pageNum = -1) const {
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
        if (obj.pageName == objectName) return true;
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
int VisualData::getObjectNumByName(const String& objectName, int pageNum = -1) const {
    const VisualDataSet::PageData* page = nullptr;

    if (pageNum < 0) {
        page = &editingPage;
    } else {
        page = &getPageData(pageNum);
        if (!page || page->isEmpty()) return -1;
    }

    for (const auto& obj : page->objects) {
        if (obj.pageName == objectName) return obj.objectNum;
    }

    return -1;
}


// dataset.pages の const 参照を返す
const std::vector<VisualDataSet::PageData>& VisualData::getVisualData() const {
  return dataset.pages;
}
// pageNum が存在すれば参照を返し、なければダミーを返す
const VisualDataSet::PageData& VisualData::getPageData(int pageNum) const {
  if (isExistsPage(pageNum)) {
    static VisualDataSet::PageData dummyPage; // 空データ
    return dummyPage;
  }
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
bool VisualData::addPage(const char* name = nullptr, int pageNum = -1) {

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

  // 編集中ページを確定
  commitEditingPage();
  editingPage = newPage;

  return true;
}

// 現在の編集ページを dataset に送る
bool VisualData::commitEditingPage() {
  if (editingPage.isEmpty()) return false;

  auto* existingPage = getPageDataRef(editingPage.pageNum);
  if (existingPage) {
    *existingPage = editingPage;  // データ更新
  } else {
    dataset.pages.push_back(editingPage);
  }

  editingPage = VisualDataSet::PageData(); // 編集ページリセット
  return true;
}


/*
  changeEditPage  // 編集ページ変更
  deletePage      // ページ削除
  getDrawingPage  // 描画中ページ名

  checkCreatable  // 編集中ページの有無とオブジェクト重複確認（新規・更新・不可）
  deleteObject    // 編集中ページから指定オブジェクトの削除

  createTemplateObject  // オブジェクトの設定

  setDrawPixelObject
  setDrawLineObject
  setDrawBezierObject
  setDrawWideLineObject

  setDrawRectObject
  setFillRectObject
  setDrawRoundRectObject
  setFillRoundRectObject

  setDrawCircleObject
  setFillCircleObject
  setDrawEllipseObject
  setFillEllipseObject
  setDrawTriangleObject
  setFillTriangleObject

  setDrawArcObject
  setFillArcObject
  setDrawEllipseArcObject
  setFillEllipseArcObject

  setDrawJpgFileObject
  setDrawPngFileObject

  drawObject  // オブジェクトごとに描画
  drawPage    // ページ単位で描画
*/

void VisualData::finalizeSetup () {
  endPageUpdate();
}