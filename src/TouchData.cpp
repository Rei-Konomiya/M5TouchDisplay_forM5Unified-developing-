#include "TouchData.hpp"
using VDS = VisualDataSet;
using TDS = TouchDataSet;


// =========================
// 1. 初期化
// =========================
TouchData::TouchData (VisualData* vData, bool enableErrorLog, bool enableInfoLog, bool enableSuccessLog){
  this->vData = vData;

  debugLog.setDebug(enableErrorLog, enableInfoLog, enableSuccessLog);
}

bool TouchData::initJudgeSprite(LovyanGFX* parent){
  judgeSprite.setColorDepth(16);
  judgeSprite.setPsram(true);
  return judgeSprite.createSprite(parent->width(), parent->height());
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
int TouchData::generateNewColor(TDS::ocPageData ocPageData) {
  int color = 1;  // 0は黒として避ける
  // 使用中の colorCode をセットに格納
  std::set<int> usedColors;
  for (const auto &oc : ocPageData.objectColors) usedColors.insert(oc.colorCode);
  // 1 から順に空き番号を探索
  while (usedColors.count(color)) color++;
  return color;
}

// createOrGetObjectColor関数
int TouchData::createOrGetObjectColor(int pageNum, int objectNum, bool getOnly) {
  for (auto& ocPage : touchDataSet.ocPages) {
    if (ocPage.pageNum == pageNum) {
      for (auto& oc : ocPage.objectColors) {
        if (oc.objectNum == objectNum) {
          return oc.colorCode; // 既存の色を返す
        }
      }
      // オブジェクトが存在しない場合
      debugLog.printlnLog(debugLog.info, "oc : objectData no exists. !" + vData->getObjectData(vData->getPageData(pageNum), objectNum).objectName);
      if (getOnly) return 0x000000; // BLACK
      TDS::objectColor oc;
      oc.objectNum = objectNum;
      oc.colorCode = generateNewColor(ocPage);
      ocPage.objectColors.push_back(oc);
      Serial.println("created objectColor.");
      return oc.colorCode;
    }
  }

  // ページが存在しない場合
  debugLog.printlnLog(debugLog.info, "oc : pageData no exists. !" + vData->getPageData(pageNum).pageName);
  if (getOnly) return 0x000000; // BLACK
  touchDataSet.ocPages.push_back({});
  auto& newPage = touchDataSet.ocPages.back();
  Serial.println("created ocPageData.");
  newPage.pageNum = pageNum;
  TDS::objectColor oc;
  oc.objectNum = objectNum;
  oc.colorCode = generateNewColor(newPage);
  newPage.objectColors.push_back(oc);
  Serial.println("created objectColor.");
  return oc.colorCode;
}


// =========================
// 4. セッター系 (createProcess 経由)
// =========================
bool TouchData::changeEditPage(int pageNum) {
  // まず現在の編集内容を確定
  commitProcessEdit();

  // 既存ページがあれば編集ページに取り込む
  TDS::PageData* p = getPageData(pageNum);
  if (p) {
    editingPage = *p;
    return true;
  }

  // なければ新規の編集ページを作り pageNum を設定しておく
  if(vData->isExistsPage(pageNum)){
    editingPage = TDS::PageData();
    editingPage.pageNum = pageNum;
    return true;
  }
  return false;
}

bool TouchData::deleteProcess(const String& processName, int pageNum, bool onDisplay) {
  TDS::PageData* targetPage = onDisplay ? &currentPageProcess : &editingPage;
  if (!targetPage) return false;

  bool deleted = false;
  for (auto it = targetPage->processes.begin(); it != targetPage->processes.end(); ) {
    if (it->processName == processName) {
      it = targetPage->processes.erase(it); // erase は eraseした次のイテレータを返す
      deleted = true;
    } else {
      ++it;
    }
  }

  for (auto &ocPage : touchDataSet.ocPages) {
    if (ocPage.pageNum == targetPage->pageNum) {
      ocPage.objectColors.erase(
        std::remove_if(ocPage.objectColors.begin(), ocPage.objectColors.end(),
            [this, targetPage](const TDS::objectColor &oc){
              return !vData->isExistsObject(oc.objectNum, targetPage->pageNum);
        }),
        ocPage.objectColors.end()
      );
    }
  }

  if (!isBatchUpdating && !onDisplay) {
    commitProcessEdit();
  }

  return deleted;
}

void TouchData::defragProcesses(int pageNum, bool onDisplay)
{
  TDS::PageData* targetPage = nullptr;

  if (pageNum >= 0) {
    targetPage = getPageData(pageNum);  // ページ指定がある場合
  } else {
    targetPage = onDisplay ? &currentPageProcess : &editingPage;
  }

  if (!targetPage) return;

  // erase-remove idiom で存在しないオブジェクトのプロセスを削除
  targetPage->processes.erase(
      std::remove_if(targetPage->processes.begin(), targetPage->processes.end(),
                      [this, targetPage](const TDS::ProcessData& p) {
                          return !vData->isExistsObject(p.objectNum, targetPage->pageNum);
                      }),
      targetPage->processes.end());

    // 存在しないオブジェクトの objectColor も削除
  for (auto &ocPage : touchDataSet.ocPages) {
    if (ocPage.pageNum == targetPage->pageNum) {
      ocPage.objectColors.erase(
        std::remove_if(ocPage.objectColors.begin(), ocPage.objectColors.end(),
            [this, targetPage](const TDS::objectColor &oc){
              return !vData->isExistsObject(oc.objectNum, targetPage->pageNum);
        }),
        ocPage.objectColors.end()
      );
    }
  }

  if (!isBatchUpdating && !onDisplay) {
    commitProcessEdit();
  }
}

bool TouchData::createProcess ( const String& processName, String objectName, TDS::TouchType type,
                              bool enableOverBorder, bool returnCurrentOver,
                              int multiClickCount, uint32_t colorCode,
                              bool onDisplay) {
  TDS::PageData* targetPage = onDisplay ? &currentPageProcess : &editingPage;
  if (!targetPage) return false;

  int objectNum = vData->getObjectNumByName(objectName);
  if (objectNum < 0) return false;

  // オブジェクト取得
  const auto& obj = vData->getObjectData(vData->getPageData(targetPage->pageNum), objectNum);

  // isUntouchable チェック
  if (obj.isUntouchable) {
    debugLog.printlnLog(debugLog.error, "The object is untouchable. Cannot create process. !" + obj.objectName);
    return false;  // 強制終了
  }

  int pageNum = onDisplay ? currentPageProcess.pageNum : editingPage.pageNum;
  if (isExistsProcessType(objectNum, type, pageNum)) {
    debugLog.printlnLog(debugLog.error, "this processData exists. !" + vData->getObjectData(vData->getPageData(pageNum), objectNum).objectName +", "+ int(type) +", "+ vData->getPageData(pageNum).pageName);
    return false;
  }
  if (isExistsProcessName(processName, onDisplay ? -1 : editingPage.pageNum)) {
    debugLog.printlnLog(debugLog.error, "this processName exists. !" + processName);
    return false;
  }

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
  proc.colorCode = createOrGetObjectColor(pageNum, objectNum);

  targetPage->processes.push_back(proc);

  if (!isBatchUpdating && !onDisplay) {
    commitProcessEdit();
  }

  return true;
}

// Release 系
bool TouchData::setReleaseProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(processName, objectName, TDS::TouchType::Release);
}

bool TouchData::setReleasingProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(processName, objectName, TDS::TouchType::Releasing);
}

// Press 系
bool TouchData::setPressProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(processName, objectName, TDS::TouchType::Press);
}

bool TouchData::setPressingProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(processName, objectName, TDS::TouchType::Pressing);
}

bool TouchData::setPressedProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(processName, objectName, TDS::TouchType::Pressed);
}

// Hold 系
bool TouchData::setHoldProcess(String processName, String objectName, bool onDisplay) {
    return createProcess(processName, objectName, TDS::TouchType::Hold);
}

bool TouchData::setHoldingProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(processName, objectName, TDS::TouchType::Holding);
}

bool TouchData::setHeldProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(processName, objectName, TDS::TouchType::Held);
}

// Flick 系
bool TouchData::setFlickProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(processName, objectName, TDS::TouchType::Flick);
}

bool TouchData::setFlickingProcess(String processName, String objectName,
                                  bool enableOverBorder, bool returnCurrentOver, bool onDisplay) {
  return createProcess(processName, objectName, TDS::TouchType::Flicking,
                        enableOverBorder, returnCurrentOver);
}

bool TouchData::setFlickedProcess(String processName, String objectName,
                                  bool enableOverBorder, bool returnCurrentOver, bool onDisplay) {
  return createProcess(processName, objectName, TDS::TouchType::Flicked,
                        enableOverBorder, returnCurrentOver);
}

// Drag 系
bool TouchData::setDragProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(processName, objectName, TDS::TouchType::Drag);
}

bool TouchData::setDraggingProcess(String processName, String objectName,
                                    bool enableOverBorder, bool returnCurrentOver, bool onDisplay) {
  return createProcess(processName, objectName, TDS::TouchType::Dragging,
                        enableOverBorder, returnCurrentOver);
}

bool TouchData::setDraggedProcess(String processName, String objectName,
                                  bool enableOverBorder, bool returnCurrentOver, bool onDisplay) {
  return createProcess(processName, objectName, TDS::TouchType::Dragged,
                        enableOverBorder, returnCurrentOver);
}

// Click 系
bool TouchData::setClickedProcess(String processName, String objectName, bool onDisplay) {
  return createProcess(processName, objectName, TDS::TouchType::Clicked);
}

bool TouchData::setMultiClickedProcess(String processName, String objectName,
                                        int count, bool onDisplay) {
  return createProcess(processName, objectName, TDS::TouchType::MultiClicked,
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

void TouchData::setProcessPage() {
  String drawingPageName = vData->getDrawingPage();
  int pageNum = vData->getPageNumByName(drawingPageName);
  // ページ番号を引数から取得
  if (!isExistsPage(pageNum)) {
    debugLog.printlnLog(debugLog.error, "Page not found. !" + String(pageNum));
    return;
  }

  // ページデータ取得（存在する場合もここで取得）
  currentPageProcess = *getPageData(pageNum);

  // 有効プロセスリストをクリア
  clearEnabledProcessList();

  debugLog.printlnLog(debugLog.success,
    "Process page set. PageNum=" + String(currentPageProcess.pageNum) +
    " / Process count=" + String(currentPageProcess.processes.size()));
}



bool TouchData::drawObjectProcess (const VDS::ObjectData &obj) {
  // isUntouchable が true の場合は描画スキップ
  if (obj.isUntouchable) return true;

  // ここで色情報を取得
  int pageNum = currentPageProcess.pageNum;   // 現在のページ番号
  int objectNum = obj.objectNum;              // objからオブジェクト番号を取得
  int objColor = createOrGetObjectColor(pageNum, objectNum, true); // getOnly = true


  switch (obj.type) {

    // -------------------- 基本描画 --------------------
    case VDS::DrawType::DrawPixel:
      judgeSprite.drawPixel(obj.objectArgs.pixel.x, obj.objectArgs.pixel.y, objColor);
      break;

    case VDS::DrawType::DrawLine:
      judgeSprite.drawLine(obj.objectArgs.line.x0, obj.objectArgs.line.y0,
                      obj.objectArgs.line.x1, obj.objectArgs.line.y1,
                      objColor);
      break;

    case VDS::DrawType::DrawBezier:
      judgeSprite.drawBezier(obj.objectArgs.bezier.x0, obj.objectArgs.bezier.y0,
                        obj.objectArgs.bezier.x1, obj.objectArgs.bezier.y1,
                        obj.objectArgs.bezier.x2, obj.objectArgs.bezier.y2,
                        objColor);
      break;

    case VDS::DrawType::DrawWideLine:
      judgeSprite.drawWideLine(obj.objectArgs.wideLine.x0, obj.objectArgs.wideLine.y0,
                          obj.objectArgs.wideLine.x1, obj.objectArgs.wideLine.y1,
                          obj.objectArgs.wideLine.r, objColor);
      break;

    case VDS::DrawType::DrawRect:
      judgeSprite.fillRect(obj.objectArgs.rect.x, obj.objectArgs.rect.y,
                      obj.objectArgs.rect.w, obj.objectArgs.rect.h,
                      objColor);
      break;

    case VDS::DrawType::DrawRoundRect:
      judgeSprite.fillRoundRect(obj.objectArgs.roundRect.x, obj.objectArgs.roundRect.y,
                            obj.objectArgs.roundRect.w, obj.objectArgs.roundRect.h,
                            obj.objectArgs.roundRect.r, objColor);
      break;

    case VDS::DrawType::DrawCircle:
      judgeSprite.fillCircle(obj.objectArgs.circle.x, obj.objectArgs.circle.y,
                        obj.objectArgs.circle.r, objColor);
      break;

    case VDS::DrawType::DrawEllipse:
      judgeSprite.fillEllipse(obj.objectArgs.ellipse.x, obj.objectArgs.ellipse.y,
                          obj.objectArgs.ellipse.rx, obj.objectArgs.ellipse.ry,
                          objColor);
      break;

    case VDS::DrawType::DrawTriangle:
      judgeSprite.fillTriangle(obj.objectArgs.triangle.x0, obj.objectArgs.triangle.y0,
                          obj.objectArgs.triangle.x1, obj.objectArgs.triangle.y1,
                          obj.objectArgs.triangle.x2, obj.objectArgs.triangle.y2,
                          objColor);
      break;

    // -------------------- 塗りつぶし --------------------
    case VDS::DrawType::FillRect:
      judgeSprite.fillRect(obj.objectArgs.rect.x, obj.objectArgs.rect.y,
                      obj.objectArgs.rect.w, obj.objectArgs.rect.h,
                      objColor);
      break;

    case VDS::DrawType::FillRoundRect:
      judgeSprite.fillRoundRect(obj.objectArgs.roundRect.x, obj.objectArgs.roundRect.y,
                            obj.objectArgs.roundRect.w, obj.objectArgs.roundRect.h,
                            obj.objectArgs.roundRect.r, objColor);
      break;

    case VDS::DrawType::FillCircle:
      judgeSprite.fillCircle(obj.objectArgs.circle.x, obj.objectArgs.circle.y,
                        obj.objectArgs.circle.r, objColor);
      break;

    case VDS::DrawType::FillTriangle:
      judgeSprite.fillTriangle(obj.objectArgs.triangle.x0, obj.objectArgs.triangle.y0,
                          obj.objectArgs.triangle.x1, obj.objectArgs.triangle.y1,
                          obj.objectArgs.triangle.x2, obj.objectArgs.triangle.y2,
                          objColor);
      break;

    case VDS::DrawType::FillEllipse:
      judgeSprite.fillEllipse(obj.objectArgs.ellipse.x, obj.objectArgs.ellipse.y,
                          obj.objectArgs.ellipse.rx, obj.objectArgs.ellipse.ry,
                          objColor);
      break;

    case VDS::DrawType::FillArc:
      judgeSprite.fillArc(obj.objectArgs.arc.x, obj.objectArgs.arc.y,
                      obj.objectArgs.arc.r0, obj.objectArgs.arc.r1,
                      obj.objectArgs.arc.angle0, obj.objectArgs.arc.angle1,
                      objColor);
      break;

    case VDS::DrawType::FillEllipseArc:
      judgeSprite.fillEllipseArc(obj.objectArgs.ellipseArc.x, obj.objectArgs.ellipseArc.y,
                            obj.objectArgs.ellipseArc.r0x, obj.objectArgs.ellipseArc.r1x,
                            obj.objectArgs.ellipseArc.r0y, obj.objectArgs.ellipseArc.r1y,
                            obj.objectArgs.ellipseArc.angle0, obj.objectArgs.ellipseArc.angle1,
                            objColor);
      break;

    // -------------------- 画像描画 --------------------
    case VDS::DrawType::DrawJpgFile:
      judgeSprite.fillRect(obj.objectArgs.jpg.x, obj.objectArgs.jpg.y,
                      obj.objectArgs.jpg.w, obj.objectArgs.jpg.h,
                      objColor);
      break;

    case VDS::DrawType::DrawPngFile:
      judgeSprite.fillRect(obj.objectArgs.png.x, obj.objectArgs.png.y,
                      obj.objectArgs.png.w, obj.objectArgs.png.h,
                      objColor);
      break;

    // -------------------- 文字描画 --------------------
    case VDS::DrawType::DrawString:
      if (obj.objectArgs.text.font)
        judgeSprite.setFont(obj.objectArgs.text.font);

      judgeSprite.setTextDatum(obj.objectArgs.text.datum);
      judgeSprite.setTextColor(objColor, objColor);
      judgeSprite.setTextSize(obj.objectArgs.text.textSize);

      judgeSprite.setTextWrap(obj.objectArgs.text.textWrap, false);

      judgeSprite.drawString(obj.objectArgs.text.text,
                        obj.objectArgs.text.x,
                        obj.objectArgs.text.y);

      break;

    // -------------------- Clip系（範囲制限） --------------------
    case VDS::DrawType::ClipArc:
    case VDS::DrawType::ClipEllipseArc:
    case VDS::DrawType::ClipRect:
    case VDS::DrawType::ClipRoundRect:
    case VDS::DrawType::ClipCircle:
    case VDS::DrawType::ClipEllipse:
    case VDS::DrawType::ClipTriangle:
      break;

    // -------------------- コンテナ系 --------------------
    case VDS::DrawType::FlexBox:
    case VDS::DrawType::TableBox:
      break;

    default:
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

  judgeSprite.fillSprite(BLACK);

  // 描画
  for (auto &obj : sortedObjects) {
    drawObjectProcess(obj);
  }

  return true;
}

bool TouchData::judgeProcess(int x, int y) {
  if (currentPageProcess.isEmpty()) {
      debugLog.printlnLog(Debug::error, "[ERROR] judgeProcess: currentPageProcess is empty. Cannot judge process.");
      currentPageProcess = TDS::PageData();
      return false;
  }

  if (enabledProcess.empty()) {
      debugLog.printlnLog(Debug::info, "judgeProcess: No enabled processes. currentProcessName cleared.");
      currentPageProcess = TDS::PageData();
      return false;
  }

  drawPageProcess();
  int color = judgeSprite.readPixel(x, y);
  auto t = M5.Touch.getDetail();

  std::vector<std::pair<String, TDS::TouchType>> candidateProcesses;

  for (auto procNum : enabledProcess) {
    for (auto& proc : currentPageProcess.processes) {
      if (proc.processNum != procNum) continue;

      uint32_t objColor = createOrGetObjectColor(currentPageProcess.pageNum, proc.objectNum, true);
      if (objColor != color) continue;

      bool valid = false;
      switch(proc.type) {
        case TDS::TouchType::Press:      valid = t.wasPressed(); break;
        case TDS::TouchType::Pressing:   valid = t.isPressed(); break;
        case TDS::TouchType::Pressed:    valid = t.wasReleased(); break;
        case TDS::TouchType::Release:    valid = t.wasReleased(); break;
        case TDS::TouchType::Releasing:  valid = t.isReleased(); break;
        case TDS::TouchType::Hold:       valid = t.wasHold(); break;
        case TDS::TouchType::Holding:    valid = t.isHolding() && !t.isDragging(); break;
        case TDS::TouchType::Held:       valid = wasHoldingOld && !t.wasDragged(); break;
        case TDS::TouchType::Drag:       valid = t.wasDragStart(); break;
        case TDS::TouchType::Dragging:   valid = t.isDragging(); break;
        case TDS::TouchType::Dragged:    valid = t.wasDragged(); break;
        case TDS::TouchType::Flick:      valid = t.wasFlickStart(); break;
        case TDS::TouchType::Flicking:   valid = t.isFlicking(); break;
        case TDS::TouchType::Flicked:    valid = t.wasFlicked(); break;
        case TDS::TouchType::Clicked:    valid = t.wasClicked(); break;
        case TDS::TouchType::MultiClicked: valid = t.getClickCount() >= proc.multiClickCount; break;
      }

      if (valid) {
        candidateProcesses.emplace_back(proc.processName, proc.type);
      }
    }
  }

  if (candidateProcesses.empty()) {
    currentProcessNameVector.clear();
    currentProcessName = "";
    return false;
  }

  // 優先度順
  std::vector<TDS::TouchType> priorityOrder = {
    TDS::TouchType::Flick, TDS::TouchType::Flicking, TDS::TouchType::Flicked,
    TDS::TouchType::Drag, TDS::TouchType::Dragging, TDS::TouchType::Dragged,
    TDS::TouchType::Hold, TDS::TouchType::Holding, TDS::TouchType::Held,
    TDS::TouchType::Clicked, TDS::TouchType::MultiClicked,
    TDS::TouchType::Press, TDS::TouchType::Pressing,
    TDS::TouchType::Pressed, TDS::TouchType::Release, TDS::TouchType::Releasing
  };

  auto getPriorityIndex = [&](TDS::TouchType type) {
    for (size_t i = 0; i < priorityOrder.size(); ++i) {
      if (priorityOrder[i] == type) return i;
    }
    return priorityOrder.size();
  };

  // 優先度でソート（indexが小さいほど優先度高）
  std::sort(candidateProcesses.begin(), candidateProcesses.end(),
  [&](const std::pair<String, TDS::TouchType> &a, const std::pair<String, TDS::TouchType> &b) {
    auto getPriorityIndex = [&](TDS::TouchType tType) -> int {
      for (size_t i = 0; i < priorityOrder.size(); ++i) {
        if (priorityOrder[i] == tType) return i;
      }
      return INT_MAX;
    };
    return getPriorityIndex(a.second) < getPriorityIndex(b.second);
  });

  currentProcessNameVector.clear();
  for (auto &p : candidateProcesses) currentProcessNameVector.push_back(p.first);
  currentProcessName = currentProcessNameVector.front();

  // HeldやactiveButtonの更新
  auto topType = candidateProcesses.front().second;
  if (topType == TDS::TouchType::Pressed || topType == TDS::TouchType::Release) {
    isObjectPressed = activeButton == currentProcessObject.objectNum;
  } else {
    activeButton = currentProcessObject.objectNum;
  }

  if (topType == TDS::TouchType::Hold || topType == TDS::TouchType::Holding) {
    wasHoldingOld = true;
  } else if (topType == TDS::TouchType::Held) {
    wasHoldingOld = false;
  }

  return true;
}



bool TouchData::update () {
  // --- タッチセンサーの有効確認 ---
  if (!M5.Touch.isEnabled()) {
    debugLog.printlnLog(Debug::error, "Touch sensor is disabled. Returning false.");
    return false;
  }

  // --- 描画ページの取得と設定 ---
  setProcessPage();

  if (currentPageProcess.isEmpty()) {
    debugLog.printlnLog(Debug::error, "TouchData::update - processPage is null after setProcessPage.");
    return false;
  }

  // --- タッチ詳細の取得 ---
  auto t = M5.Touch.getDetail();

  // --- タッチ状態に基づいた有効化/無効化処理 ---
  if (t.isPressed()) {
    enableProcess(true);  // Press関連プロセスを有効化
    debugLog.printlnLog(Debug::info, "TouchData::update - wasPressed -> enabling Press processes.");
  } else if (t.wasReleased()) {
    enableProcess(false); // Release関連プロセスを有効化
    debugLog.printlnLog(Debug::info, "TouchData::update - wasReleased -> enabling Release processes.");
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
    } else if (t.wasHold()) {
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



void TouchData::finalizeSetup(){
  commitProcessEdit();
  isBatchUpdating = false;
}