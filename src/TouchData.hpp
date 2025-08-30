#pragma once
#include <M5Unified.h>
#include "VisualData.hpp"
#include "TouchDataSet.h"

class TouchData {
public:
  Debug debugLog;
  using TDS = TouchDataSet;

  VisualData* vData;               // VisualData への参照
  LGFX_Sprite judgeSprite;         // 判定用スプライト

  TDS touchDataSet;
  TDS::PageData editingPage;
  TDS::PageData currentPageProcess;
  std::vector<int> enabledProcess;
  String currentProcessName = "";

  bool isBatchUpdating = false;
  int lastAssignedPageNum = 0;
  int lastAssignedProcessNum = 0;
  uint32_t lastAssignedColor = 0;

  // =========================
  // 1. 初期化
  // =========================
  TouchData(LovyanGFX* parent, bool enableErrorLog, bool enableInfoLog, bool enableSuccessLog);

  // =========================
  // 2. 存在確認系
  // =========================
  bool isExistsPage(int pageNum) const;
  bool isExistsObject(int objectNum, int pageNum = -1) const;
  bool isExistsProcess(int processNum, int pageNum = -1) const;
  bool isExistsProcessName(String processName, int pageNum = -1) const;
  bool isExistsProcessType(int objectNum, TDS::TouchType type, int pageNum = -1) const;

  // =========================
  // 3. データ取得系
  // =========================
  const TDS::PageData* getPageData(int pageNum) const;
  TDS::PageData* getPageData(int pageNum);    // ?

  int getProcessNumByName(const String& processName, int pageNum = -1) const;
  int getObjectNumByProcess(int processNum, int pageNum = -1) const;    // ?

  String getProcessName(int processNum, int pageNum = -1) const;
  TDS::TouchType getProcessType(int processNum, int pageNum = -1) const;
  uint32_t getProcessColor(int processNum, int pageNum = -1) const;

  uint32_t generateNewColor();
  uint32_t createOrGetObjectColor(int pageNum, int objectNum, bool getOnly = false);
  // =========================
  // 4. データ登録系
  // =========================
  bool createProcess(bool onDisplay = false,
                      const String& processName, String objectName, TDS::TouchType type,
                      bool enableOverBorder = false, bool returnCurrentOver = false,
                      int multiClickCount = 0, uint32_t colorCode = 0
                      );

  bool setReleaseProcess(String processName, String objectName, bool onDisplay = false);
  bool setReleasingProcess(String processName, String objectName, bool onDisplay = false);
  bool setPressProcess(String processName, String objectName, bool onDisplay = false);
  bool setPressingProcess(String processName, String objectName, bool onDisplay = false);
  bool setPressedProcess(String processName, String objectName, bool onDisplay = false);

  bool setHoldProcess(String processName, String objectName, bool onDisplay = false);
  bool setHoldingProcess(String processName, String objectName, bool onDisplay = false);
  bool setHeldProcess(String processName, String objectName, bool onDisplay = false);

  bool setFlickProcess(String processName, String objectName, bool onDisplay = false);
  bool setFlickingProcess(String processName, String objectName, bool enableOverBorder = false, bool returnCurrentOver = false, bool onDisplay = false);
  bool setFlickedProcess(String processName, String objectName, bool enableOverBorder = false, bool returnCurrentOver = false, bool onDisplay = false);

  bool setDragProcess(String processName, String objectName, bool onDisplay = false);
  bool setDraggingProcess(String processName, String objectName, bool enableOverBorder = false, bool returnCurrentOver = false, bool onDisplay = false);
  bool setDraggedProcess(String processName, String objectName, bool enableOverBorder = false, bool returnCurrentOver = false, bool onDisplay = false);

  bool setClickedProcess(String processName, String objectName, bool onDisplay = false);
  bool setMultiClickedProcess(String processName, String objectName, int count, bool onDisplay = false);

  bool commitProcessEdit();
  void TouchData::beginProcessEdit();
  void TouchData::endProcessEdit();

  // =========================
  // 5. プロセス有効/無効
  // =========================
  bool enableProcess(bool isPress);
  bool disableProcessType(TDS::TouchType tType);
  void clearEnabledProcessList();

  // =========================
  // 6. タッチ判定
  // =========================w
  void setProcessPage();
  bool drawObjectProcess (const VDS::ObjectData &obj);
  bool drawPageProcess();
  bool judgeProcess(int32_t x, int32_t y);

  // =========================
  // 7. 更新（タッチ状態・判定）
  // =========================
  bool update();
};
