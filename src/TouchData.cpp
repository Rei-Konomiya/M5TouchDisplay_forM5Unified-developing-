#include "TouchData.hpp"
using TDS = TouchDataSet;
using VDS = VisualDataSet;


// =========================
// 1. 初期化
// =========================
TouchData::TouchData (LovyanGFX* parent, bool enableErrorLog, bool enableInfoLog, bool enableSuccessLog){
  debugLog.setDebug(enableErrorLog, enableInfoLog, enableSuccessLog);

  judgeSprite.setPsram(true);
  judgeSprite.setColorDepth(8);
  bool created = judgeSprite.createSprite(320, 240);

  if (!created) {
    debugLog.printlnLog(Debug::error, "[ERROR] judgeSprite createSprite failed"); // DebugLogを使用
  }
}

// =========================
// 2. 存在確認系
// =========================
// ページが存在するか
bool TouchData::isExistsPage(int pageNum) const {
  if (pageNum < 0) {
    // currentPageProcess が有効なら true
    return (currentPageProcess.pageNum >= 0);
  }
  for (const auto& page : touchDataSet.pages) {
    if (page.pageNum == pageNum) return true;
  }
  return false;
}
// オブジェクトが存在するか
bool TouchData::isExistsObject (int objectNum, int pageNum) const {
  if (!vData) return false;  // vData が未設定の場合は false
    return vData->isExistsObject(objectNum, pageNum);
}
// プロセスが存在するか
bool TouchData::isExistsProcess(int processNum, int pageNum) const {
  if (pageNum < 0) {
    // currentPageProcess 内のチェック
    for (const auto& proc : currentPageProcess.processes) {
      if (proc.processNum == processNum) return true;
    }
    return false;
  }
  for (const auto& page : touchDataSet.pages) {
    if (page.pageNum == pageNum) {
      for (const auto& proc : page.processes) {
        if (proc.processNum == processNum) return true;
      }
    }
  }
  return false;
}
bool TouchData::isExistsProcessName(String processName, int pageNum) const {
  if (pageNum < 0) {
    // currentPageProcess 内のチェック
    for (const auto& proc : currentPageProcess.processes) {
      if (proc.processName == processName) return true;
    }
    return false;
  }
  for (const auto& page : touchDataSet.pages) {
    if (page.pageNum == pageNum) {
      for (const auto& proc : page.processes) {
        if (proc.processName == processName) return true;
      }
    }
  }
  return false;
}

// 指定オブジェクトに同じタッチタイプのプロセスが既に存在するか
bool TouchData::isExistsProcessType(int objectNum, TDS::TouchType type, int pageNum) const {
  if (pageNum < 0) {
    // currentPageProcess 内のチェック
    for (const auto& proc : currentPageProcess.processes) {
      if (proc.objectNum == objectNum && proc.type == type) {
        return true;
      }
    }
    return false;
  }
  for (const auto& page : touchDataSet.pages) {
    if (page.pageNum == pageNum) {
      for (const auto& proc : page.processes) {
        if (proc.objectNum == objectNum && proc.type == type) {
          return true;
        }
      }
    }
  }
  return false;
}


// pageNum から PageData への const 参照を取得（pageNum < 0 の場合 currentPageProcess）
const TDS::PageData* TouchData::getPageData(int pageNum) const {
  if (pageNum < 0) return &currentPageProcess;
  for (const auto& page : touchDataSet.pages) {
    if (page.pageNum == pageNum) return &page;
  }
  return nullptr;
}

// pageNum から PageData への参照を取得（pageNum < 0 の場合 currentPageProcess）
TDS::PageData* TouchData::getPageData(int pageNum) {
  if (pageNum < 0) return &currentPageProcess;
  for (auto& page : touchDataSet.pages) {
    if (page.pageNum == pageNum) return &page;
  }
  return nullptr;
}

// プロセス名から processNum を取得
int TouchData::getProcessNumByName(const String& processName, int pageNum) const {
  const TDS::PageData* page = getPageData(pageNum);
  if (!page) return -1;
  for (const auto& proc : page->processes) {
    if (proc.processName == processName) return proc.processNum;
  }
  return -1;
}

// processNum から対象オブジェクトの objectNum を取得
int TouchData::getObjectNumByProcess(int processNum, int pageNum) const {
  const TDS::PageData* page = getPageData(pageNum);
  if (!page) return -1;
  for (const auto& proc : page->processes) {
    if (proc.processNum == processNum) return proc.objectNum;
  }
  return -1;
}

// processNum からプロセス名を取得
String TouchData::getProcessName(int processNum, int pageNum) const {
  const TDS::PageData* page = getPageData(pageNum);
  if (!page) return "";
  for (const auto& proc : page->processes) {
    if (proc.processNum == processNum) return proc.processName;
  }
  return "";
}

// processNum からタッチタイプを取得
TDS::TouchType TouchData::getProcessType(int processNum, int pageNum) const {
  const TDS::PageData* page = getPageData(pageNum);
  if (!page) return TDS::TouchType::Press; // デフォルト
  for (const auto& proc : page->processes) {
    if (proc.processNum == processNum) return proc.type;
  }
  return TDS::TouchType::Press; // デフォルト
}

// processNum から判定用カラーコードを取得
uint32_t TouchData::getProcessColor(int processNum, int pageNum) const {
  const TDS::PageData* page = getPageData(pageNum);
  if (!page) return 0;
  for (const auto& proc : page->processes) {
    if (proc.processNum == processNum) return proc.colorCode;
  }
  return 0;
}


// 新しい colorCode を生成（0は黒として避ける）
uint32_t TouchData::generateNewColor() {
  uint32_t color = lastAssignedColor++;
  if (lastAssignedColor == 0) lastAssignedColor = 1;
  return color;
}

// createOrGetObjectColor関数
uint32_t TouchData::createOrGetObjectColor(int pageNum, int objectNum, bool getOnly) {
  for (auto& ocPage : touchDataSet.ocPages) {
    if (ocPage.pageNum == pageNum) {
      for (auto& oc : ocPage.objectColors) {
        if (oc.objectNum == objectNum) {
          return oc.colorCode; // 既存の色を返す
        }
      }
      // オブジェクトが存在しない場合
      if (getOnly) return 0x000000; // BLACK
      TDS::objectColor oc;
      oc.objectNum = objectNum;
      oc.colorCode = generateNewColor();
      ocPage.objectColors.push_back(oc);
      return oc.colorCode;
    }
  }

  // ページが存在しない場合
  if (getOnly) return 0x000000; // BLACK
  touchDataSet.ocPages.push_back({});
  auto& newPage = touchDataSet.ocPages.back();
  newPage.pageNum = pageNum;
  TDS::objectColor oc;
  oc.objectNum = objectNum;
  oc.colorCode = generateNewColor();
  newPage.objectColors.push_back(oc);
  return oc.colorCode;
}


// =========================
// 4. セッター系 (createProcess 経由)
// =========================
bool TouchData::createProcess (bool onDisplay,
                              const String& processName, String objectName, TDS::TouchType type,
                              bool enableOverBorder, bool returnCurrentOver,
                              int multiClickCount, uint32_t colorCode ) {
  TDS::PageData* targetPage = onDisplay ? &currentPageProcess : &editingPage;
  if (!targetPage) return false;

  int objectNum = vData->getObjectNumByName(objectName);
  if (objectNum < 0) return false;

  int pageNum = onDisplay ? currentPageProcess.pageNum : editingPage.pageNum;
  if (isExistsProcessType(objectNum, type, pageNum)) return false;
  if (isExistsProcessName(processName, onDisplay ? -1 : editingPage.pageNum)) return false;

  lastAssignedProcessNum++;
  int processNum = lastAssignedProcessNum;

  TDS::ProcessData proc{};
  proc.processNum = processNum;
  proc.processName = processName;
  proc.objectNum = objectNum;
  proc.type = type;
  proc.enableOverBorder = enableOverBorder;
  proc.returnCurrentOver = returnCurrentOver;
  proc.multiClickCount = multiClickCount;
  proc.colorCode = createOrGetObjectColor (pageNum, objectNum) ;

  targetPage->processes.push_back(proc);

  if (!isBatchUpdating && !onDisplay) {
    commitProcessEdit();
  }

  return true;
}

// Release 系
bool TouchData::setReleaseProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(onDisplay, processName, objectName, TDS::TouchType::Release);
}

bool TouchData::setReleasingProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(onDisplay, processName, objectName, TDS::TouchType::Releasing);
}

// Press 系
bool TouchData::setPressProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(onDisplay, processName, objectName, TDS::TouchType::Press);
}

bool TouchData::setPressingProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(onDisplay, processName, objectName, TDS::TouchType::Pressing);
}

bool TouchData::setPressedProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(onDisplay, processName, objectName, TDS::TouchType::Pressed);
}

// Hold 系
bool TouchData::setHoldProcess(String processName, String objectName, bool onDisplay) {
    return createProcess(onDisplay, processName, objectName, TDS::TouchType::Hold);
}

bool TouchData::setHoldingProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(onDisplay, processName, objectName, TDS::TouchType::Holding);
}

bool TouchData::setHeldProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(onDisplay, processName, objectName, TDS::TouchType::Held);
}

// Flick 系
bool TouchData::setFlickProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(onDisplay, processName, objectName, TDS::TouchType::Flick);
}

bool TouchData::setFlickingProcess(String processName, String objectName,
                                  bool enableOverBorder, bool returnCurrentOver, bool onDisplay) {
  return createProcess(onDisplay, processName, objectName, TDS::TouchType::Flicking,
                        enableOverBorder, returnCurrentOver);
}

bool TouchData::setFlickedProcess(String processName, String objectName,
                                  bool enableOverBorder, bool returnCurrentOver, bool onDisplay) {
  return createProcess(onDisplay, processName, objectName, TDS::TouchType::Flicked,
                        enableOverBorder, returnCurrentOver);
}

// Drag 系
bool TouchData::setDragProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(onDisplay, processName, objectName, TDS::TouchType::Drag);
}

bool TouchData::setDraggingProcess(String processName, String objectName,
                                    bool enableOverBorder, bool returnCurrentOver, bool onDisplay) {
  return createProcess(onDisplay, processName, objectName, TDS::TouchType::Dragging,
                        enableOverBorder, returnCurrentOver);
}

bool TouchData::setDraggedProcess(String processName, String objectName,
                                  bool enableOverBorder, bool returnCurrentOver, bool onDisplay) {
  return createProcess(onDisplay, processName, objectName, TDS::TouchType::Dragged,
                        enableOverBorder, returnCurrentOver);
}

// Click 系
bool TouchData::setClickedProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(onDisplay, processName, objectName, TDS::TouchType::Clicked);
}

bool TouchData::setMultiClickedProcess(String processName, String objectName,
                                        int count, bool onDisplay) {
  return createProcess(onDisplay, processName, objectName, TDS::TouchType::MultiClicked,
                        false, false, count);
}

bool TouchData::commitProcessEdit() {
  if (editingPage.isEmpty()) return false;

  TDS::PageData* existingPage = getPageData(editingPage.pageNum);
  if (existingPage) {
    // 既存ページがあれば更新
    *existingPage = editingPage;
  } else {
    // 存在しなければ追加
    touchDataSet.pages.push_back(editingPage);
  }

  // 編集ページリセット
  editingPage = TDS::PageData();

  return true;
}

void TouchData::beginProcessEdit() {
  isBatchUpdating = true;
}
void TouchData::endProcessEdit() {
  commitProcessEdit();
  isBatchUpdating = false;
}

bool TouchData::enableProcess(bool isPress) {
  enabledProcess.clear();
  const auto& page = currentPageProcess; // 表示中ページ
  for (const auto& proc : page.processes) {
    if (isPress) {
      switch(proc.type) {
        case TDS::TouchType::Press:
        case TDS::TouchType::Pressing:
        case TDS::TouchType::Pressed:
        case TDS::TouchType::Hold:
        case TDS::TouchType::Holding:
        case TDS::TouchType::Held:
        case TDS::TouchType::Drag:
        case TDS::TouchType::Dragging:
        case TDS::TouchType::Dragged:
        case TDS::TouchType::Flick:
        case TDS::TouchType::Flicking:
        case TDS::TouchType::Flicked:
        case TDS::TouchType::Clicked:
        case TDS::TouchType::MultiClicked:
          enabledProcess.push_back(proc.processNum);
          break;
        default: break;
      }
    } else { // Release系
      switch(proc.type) {
        case TDS::TouchType::Release:
        case TDS::TouchType::Releasing:
        case TDS::TouchType::Clicked:
        case TDS::TouchType::MultiClicked:
        case TDS::TouchType::Flicked:
        case TDS::TouchType::Dragged:
          enabledProcess.push_back(proc.processNum);
          break;
        default: break;
      }
    }
  }
  return true;
}

bool TouchData::disableProcessType(TDS::TouchType tType) {
  enabledProcess.erase(
    std::remove_if(enabledProcess.begin(), enabledProcess.end(),
      [this, tType](int pNum){
        return getProcessType(pNum) == tType;
      }),
    enabledProcess.end()
  );
  return true;
}

void TouchData::clearEnabledProcessList() {
  enabledProcess.clear();
}

bool TouchData::judgeProcess(int32_t x, int32_t y) {
  if (currentPageProcess.isEmpty()) return false;
  if (!judgeSprite.getBuffer()) return false;  // スプライト未初期化なら無効

  uint32_t touchedColor = judgeSprite.readPixel(x, y);
  if (touchedColor == 0) return false;  // 透明なら無効

  int hitProcessNum = -1;
  int maxZIndex = -1;

  // ページ内のプロセスを走査
  for (const auto& proc : currentPageProcess.processes) {
    if (proc.colorCode == touchedColor) {
      // zIndexがあるならここで比較（必要に応じて拡張）
      hitProcessNum = proc.processNum;
      break;
    }
  }

  // ヒットしたプロセスのみ enabledProcess に残す
  enabledProcess.clear();
  if (hitProcessNum != -1) {
    enabledProcess.push_back(hitProcessNum);
    return true;
  }
  return false;
}

void TouchData::setProcessPage() {
  if (touchDataSet.pages.empty()) {
    debugLog.printlnLog(debugLog.error, "No page data exists.");
    return;
  }

  // ページ番号の妥当性チェック
  if (!isExistsPage(lastAssignedPageNum)) {
    debugLog.printlnLog(debugLog.error, "Page not found: " + String(lastAssignedPageNum));
    return;
  }

  // 対象ページをセット
  currentPageProcess = *getPageData(lastAssignedPageNum);

  // 有効プロセスリストを初期化
  clearEnabledProcessList();

  // デバッグログ
  debugLog.printlnLog(debugLog.success,
    "Process page set. PageNum=" + String(currentPageProcess.pageNum) +
    " / Process count=" + String(currentPageProcess.processes.size()));
}


bool TouchData::drawObjectProcess (const VDS::ObjectData &obj) {
  // ここで色情報を取得
  int pageNum = currentPageProcess.pageNum;   // 現在のページ番号
  int objectNum = obj.objectNum;              // objからオブジェクト番号を取得
  uint32_t objColor = createOrGetObjectColor(pageNum, objectNum, true); // getOnly = true

  switch (obj.type) {

    // -------------------- 基本描画 --------------------
    case VDS::DrawType::DrawPixel:
      judgeSprite.drawPixel(obj.objectArgs.pixel.x, obj.objectArgs.pixel.y, obj.objectArgs.pixel.color);
      break;

    case VDS::DrawType::DrawLine:
      Serial.println(F("drawing DrawLine"));
      judgeSprite.drawLine(obj.objectArgs.line.x0, obj.objectArgs.line.y0,
                      obj.objectArgs.line.x1, obj.objectArgs.line.y1,
                      objColor);
      break;

    case VDS::DrawType::DrawBezier:
      Serial.println(F("drawing DrawBezier"));
      judgeSprite.drawBezier(obj.objectArgs.bezier.x0, obj.objectArgs.bezier.y0,
                        obj.objectArgs.bezier.x1, obj.objectArgs.bezier.y1,
                        obj.objectArgs.bezier.x2, obj.objectArgs.bezier.y2,
                        objColor);
      break;

    case VDS::DrawType::DrawWideLine:
      Serial.println(F("drawing DrawWideLine"));
      judgeSprite.drawWideLine(obj.objectArgs.wideLine.x0, obj.objectArgs.wideLine.y0,
                          obj.objectArgs.wideLine.x1, obj.objectArgs.wideLine.y1,
                          obj.objectArgs.wideLine.r, objColor);
      break;

    case VDS::DrawType::DrawRect:
      Serial.println(F("drawing DrawRect"));
      judgeSprite.drawRect(obj.objectArgs.rect.x, obj.objectArgs.rect.y,
                      obj.objectArgs.rect.w, obj.objectArgs.rect.h,
                      objColor);
      break;

    case VDS::DrawType::DrawRoundRect:
      Serial.println(F("drawing DrawRoundRect"));
      judgeSprite.drawRoundRect(obj.objectArgs.roundRect.x, obj.objectArgs.roundRect.y,
                            obj.objectArgs.roundRect.w, obj.objectArgs.roundRect.h,
                            obj.objectArgs.roundRect.r, objColor);
      break;

    case VDS::DrawType::DrawCircle:
      Serial.println(F("drawing DrawCircle"));
      judgeSprite.drawCircle(obj.objectArgs.circle.x, obj.objectArgs.circle.y,
                        obj.objectArgs.circle.r, objColor);
      break;

    case VDS::DrawType::DrawEllipse:
      Serial.println(F("drawing DrawEllipse"));
      judgeSprite.drawEllipse(obj.objectArgs.ellipse.x, obj.objectArgs.ellipse.y,
                          obj.objectArgs.ellipse.rx, obj.objectArgs.ellipse.ry,
                          objColor);
      break;

    case VDS::DrawType::DrawTriangle:
      Serial.println(F("drawing DrawTriangle"));
      judgeSprite.drawTriangle(obj.objectArgs.triangle.x0, obj.objectArgs.triangle.y0,
                          obj.objectArgs.triangle.x1, obj.objectArgs.triangle.y1,
                          obj.objectArgs.triangle.x2, obj.objectArgs.triangle.y2,
                          objColor);
      break;

    // -------------------- 塗りつぶし --------------------
    case VDS::DrawType::FillRect:
      Serial.println(F("drawing FillRect"));
      judgeSprite.fillRect(obj.objectArgs.rect.x, obj.objectArgs.rect.y,
                      obj.objectArgs.rect.w, obj.objectArgs.rect.h,
                      objColor);
      break;

    case VDS::DrawType::FillRoundRect:
      Serial.println(F("drawing FillRoundRect"));
      judgeSprite.fillRoundRect(obj.objectArgs.roundRect.x, obj.objectArgs.roundRect.y,
                            obj.objectArgs.roundRect.w, obj.objectArgs.roundRect.h,
                            obj.objectArgs.roundRect.r, objColor);
      break;

    case VDS::DrawType::FillCircle:
      Serial.println(F("drawing FillCircle"));
      judgeSprite.fillCircle(obj.objectArgs.circle.x, obj.objectArgs.circle.y,
                        obj.objectArgs.circle.r, objColor);
      break;

    case VDS::DrawType::FillTriangle:
      Serial.println(F("drawing FillTriangle"));
      judgeSprite.fillTriangle(obj.objectArgs.triangle.x0, obj.objectArgs.triangle.y0,
                          obj.objectArgs.triangle.x1, obj.objectArgs.triangle.y1,
                          obj.objectArgs.triangle.x2, obj.objectArgs.triangle.y2,
                          objColor);
      break;

    case VDS::DrawType::FillEllipse:
      Serial.println(F("drawing FillEllipse"));
      judgeSprite.fillEllipse(obj.objectArgs.ellipse.x, obj.objectArgs.ellipse.y,
                          obj.objectArgs.ellipse.rx, obj.objectArgs.ellipse.ry,
                          objColor);
      break;

    case VDS::DrawType::FillArc:
      Serial.println(F("drawing FillArc"));
      judgeSprite.fillArc(obj.objectArgs.arc.x, obj.objectArgs.arc.y,
                      obj.objectArgs.arc.r0, obj.objectArgs.arc.r1,
                      obj.objectArgs.arc.angle0, obj.objectArgs.arc.angle1,
                      objColor);
      break;

    case VDS::DrawType::FillEllipseArc:
      Serial.println(F("drawing FillEllipseArc"));
      judgeSprite.fillEllipseArc(obj.objectArgs.ellipseArc.x, obj.objectArgs.ellipseArc.y,
                            obj.objectArgs.ellipseArc.r0x, obj.objectArgs.ellipseArc.r1x,
                            obj.objectArgs.ellipseArc.r0y, obj.objectArgs.ellipseArc.r1y,
                            obj.objectArgs.ellipseArc.angle0, obj.objectArgs.ellipseArc.angle1,
                            objColor);
      break;

    // -------------------- 画像描画 --------------------
    case VDS::DrawType::DrawJpgFile:
      Serial.println(F("drawing DrawJpgFile"));
      judgeSprite.fillRect(obj.objectArgs.jpg.x, obj.objectArgs.jpg.y,
                      obj.objectArgs.jpg.w, obj.objectArgs.jpg.h,
                      objColor);
      break;

    case VDS::DrawType::DrawPngFile:
      Serial.println(F("drawing DrawPngFile"));
      judgeSprite.fillRect(obj.objectArgs.png.x, obj.objectArgs.png.y,
                      obj.objectArgs.png.w, obj.objectArgs.png.h,
                      objColor);
      break;

    // -------------------- 文字描画 --------------------
    case VDS::DrawType::DrawString:
      Serial.println(F("drawing DrawString"));
      judgeSprite.setTextColor(objColor, objColor); 
      judgeSprite.drawString(obj.objectArgs.text.text,
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


bool TouchData::drawPageProcess() {
  // Z-indexで安定ソート
  std::vector<VDS::ObjectData> sortedObjects = vData->currentPageCopy.objects;
  std::stable_sort(sortedObjects.begin(), sortedObjects.end(),
                    [](const VDS::ObjectData &a, const VDS::ObjectData &b) {
                      return a.zIndex < b.zIndex;
                    });

  Serial.println("clear Display");
  judgeSprite.fillSprite(BLACK);

  // 描画
  for (auto &obj : sortedObjects) {
    drawObjectProcess(obj);
  }

  return true;
}

bool TouchData::judgeProcess(int x, int y) {
  // 1:現在のページのプロセス情報が存在するかチェック
  if (currentPageProcess.isEmpty()) {
    debugLog.printlnLog(Debug::error, "[ERROR] judgeProcess: currentPageProcess is empty. Cannot judge process.");
    currentPageProcess = TDS::PageData();
    return false;
  }

  // 2:有効プロセスリストが存在するかチェック
  if (enabledProcess.empty()) {
    debugLog.printlnLog(Debug::info, "judgeProcess: No enabled processes. currentProcessName cleared.");
    currentPageProcess = TDS::PageData();
    return false;
  }

  // 3:タッチ座標の判定処理開始（ここからスプライト描画やカラーピック処理に移行）
  drawPageProcess();

  // 4:タッチ座標の色取得
  int color = judgeSprite.readPixel(x, y);

  // 5: enabledProcess に対応するプロセスを探索
  for (auto procNum : enabledProcess) {
    for (auto& proc : currentPageProcess.processes) {
      if (proc.processNum == procNum) {
        uint32_t objColor = createOrGetObjectColor(currentPageProcess.pageNum, proc.objectNum, true);
        if (objColor == color) {
          currentProcessName = proc.processName;  // 名前だけ保持
          return true;
        }
      }
    }
  }

  // 6: 該当なし
  currentProcessName = "";
  return false;
}


bool TouchData::update() {
  // --- タッチセンサーの有効確認 ---
  if (!M5.Touch.isEnabled()) {
    debugLog.printlnLog(Debug::error, "Touch sensor is disabled. Returning false.");
    return false;
  }

  // --- 描画ページの取得と設定 ---
  String drawingPageName = vData->getDrawingPage();
  setProcessPage();

  if (currentPageProcess.isEmpty()) {
    debugLog.printlnLog(Debug::error, "TouchData::update - processPage is null after setProcessPage.");
    return false;
  }

  // --- タッチ詳細の取得 ---
  auto t = M5.Touch.getDetail();

  // --- タッチ状態に基づいた有効化/無効化処理 ---
  if (t.wasPressed()) {
    enableProcess(true);  // Press関連プロセスを有効化
    debugLog.printlnLog(Debug::info, "TouchData::update - wasPressed -> enabling Press processes.");
  } 
  else if (t.wasReleased()) {
    enableProcess(false); // Release関連プロセスを有効化
    debugLog.printlnLog(Debug::info, "TouchData::update - wasReleased -> enabling Release processes.");
  } 
  else {
    // 新規イベントなし（ホールド継続 or タッチなし）
    debugLog.printlnLog(Debug::info,
      "TouchData::update - No new touch event. Holding=" + String(t.isHolding()) +
      ", Pressed=" + String(t.isPressed()));
    currentProcessName = ""; 
    return false;
  }

  // --- 特殊イベントによるプロセス無効化 ---
  if (t.isPressed()) {  
    if (t.wasFlickStart()) {
      debugLog.printlnLog(Debug::info, "TouchData::update - Flick start detected.");
      disableProcessType(TDS::TouchType::Hold);
      disableProcessType(TDS::TouchType::Holding);
      disableProcessType(TDS::TouchType::Held);
      disableProcessType(TDS::TouchType::Drag);
      disableProcessType(TDS::TouchType::Dragging);
      disableProcessType(TDS::TouchType::Dragged);
      disableProcessType(TDS::TouchType::Clicked);
      disableProcessType(TDS::TouchType::MultiClicked);
    } 
    else if (t.wasHold()) {
      debugLog.printlnLog(Debug::info, "TouchData::update - Hold detected.");
      disableProcessType(TDS::TouchType::Flick);
      disableProcessType(TDS::TouchType::Flicking);
      disableProcessType(TDS::TouchType::Flicked);
      disableProcessType(TDS::TouchType::Clicked);
      disableProcessType(TDS::TouchType::MultiClicked);
      
      if (t.wasDragStart()) {
        debugLog.printlnLog(Debug::info, "TouchData::update - Drag start detected.");
        disableProcessType(TDS::TouchType::Hold);
        disableProcessType(TDS::TouchType::Holding);
        disableProcessType(TDS::TouchType::Held);
      }
    }
  }

  // --- タッチ位置による判定 ---
  bool judged = judgeProcess(t.x, t.y);

  debugLog.printlnLog(Debug::info,
    "TouchData::update - Finished cycle. Judged=" + String(judged));

  return judged; // 判定結果を返す
}
