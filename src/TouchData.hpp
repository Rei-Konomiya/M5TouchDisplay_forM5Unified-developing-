#pragma once
#include <M5Unified.h>
#include <set>
#include "VisualData.hpp"
#include "TouchDataSet.h"

class TouchData {
public:
  Debug debugLog;
  using TDS = TouchDataSet;
  using VDS = VisualDataSet;

  VisualData* vData;               // VisualData への参照
  LGFX_Sprite judgeSprite;         // 判定用スプライト

  TDS touchDataSet;
  TDS::PageData editingPage;
  TDS::PageData currentPageProcess;
  std::vector<int> enabledProcess;
  
  std::vector<String> currentProcessNameVector;
  String currentProcessName = "";
  VDS::ObjectData currentProcessObject;
  int activeButton = 0;
  bool isObjectPressed = false;

  bool isBatchUpdating = false;
  int lastAssignedProcessNum = 0;

  bool wasHoldingOld = false;
  // =========================
  // 1. 初期化
  // =========================
  TouchData(VisualData* vData, bool enableErrorLog, bool enableInfoLog, bool enableSuccessLog);
  bool initJudgeSprite(LovyanGFX* parent);
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
  TDS::PageData* getPageData(int pageNum);

  int getProcessNumByName(const String& processName, int pageNum = -1) const;
  int getObjectNumByProcess(int processNum, int pageNum = -1) const;

  String getProcessName(int processNum, int pageNum = -1) const;
  TDS::TouchType getProcessType(int processNum, int pageNum = -1) const;
  uint32_t getProcessColor(int processNum, int pageNum = -1) const;

  int generateNewColor(TDS::ocPageData ocPageData);
  int createOrGetObjectColor(int pageNum, int objectNum, bool getOnly = false);
  // =========================
  // 4. データ登録系
  // =========================
  bool changeEditPage(int pageNum);

  bool deleteProcess(const String& processName, int pageNum = -1, bool onDisplay = false);
  void defragProcesses(int pageNum = -1, bool onDisplay = false);

  bool createProcess( const String& processName, String objectName, TDS::TouchType type,
                      bool enableOverBorder = false, bool returnCurrentOver = false,
                      int multiClickCount = 0, uint32_t colorCode = 0,
                      bool onDisplay = false );

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
  void beginProcessEdit();
  void endProcessEdit();

  // =========================
  // 5. プロセス有効/無効
  // =========================
  bool enableProcess(bool isPress);
  bool disableProcessType(TDS::TouchType tType);
  void clearEnabledProcessList();

  // =========================
  // 6. タッチ判定
  // =========================
  void setProcessPage();
  bool drawObjectProcess (const VDS::ObjectData &obj);
  bool drawPageProcess();
  bool judgeProcess(int x, int y);

  // =========================
  // 7. 更新（タッチ状態・判定）
  // =========================
  bool update();

  
  void finalizeSetup();
};
