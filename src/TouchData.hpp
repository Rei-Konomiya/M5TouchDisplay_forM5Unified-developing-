#pragma once

#include "VisualData.hpp"

class TouchData{
  TouchData(VisualData* vData, LovyanGFX* parent, bool enableErrorLog, bool enableInfoLog, bool enableSuccessLog);

  Debug debugLog;
  LGFX_Sprite judgeSprite;
  JsonDocument processList;
  JsonDocument enabledProcessList;

  VisualData* vData;

  enum class touchType{
    Release,          // Release
    Releasing,        // Release  ing

    Press,            // Press    start
    Pressing,         // Press    ing
    Pressed,          // Press    end

    Hold,             // Hold     start (pressed for more than 0.5 sec.)
    Holding,          // Hold     ing
    Held,             // Hold     end

    Flick,            // Flick    start (swiped in less than 0.5 sec.)
    Flicking,         // Flick    ing
    Flicked,          // Flick    end

    Drag,             // Drag     start (swipe 0.5 sec. after pressing)
    Dragging,         // Drag     ing
    Dragged,          // Drag     end

    Clicked,          // Click    end
    MultiClicked      // Click    end (multiple times)
  };

  enum class processType{
    Release,
    Press,
    Hold,
    Flick,
    Drag,
    Click
  };

  bool   isExistsProcessPage  (String pageName);
  bool   isExistsProcess      (String pageName, String processName);
  String isExistsProcessType  (String pageName, String objectName, touchType type);

  JsonObject createOrGetProcess (String pageName, String processName);

  bool setTemplateProcess     (String pageName, String processName, String objectName, touchType type, bool enableOverBorder=false, bool returnCurrentOver=false);

  bool setReleaseProcess      (String pageName, String processName, String objectName);
  bool setReleasingProcess    (String pageName, String processName, String objectName);

  bool setPressProcess        (String pageName, String processName, String objectName);
  bool setPressingProcess     (String pageName, String processName, String objectName);
  bool setPressedProcess      (String pageName, String processName, String objectName);

  bool setHoldProcess         (String pageName, String processName, String objectName);
  bool setHoldingProcess      (String pageName, String processName, String objectName);
  bool setHeldProcess         (String pageName, String processName, String objectName);

  bool setFlickProcess        (String pageName, String processName, String objectName);
  bool setFlickingProcess     (String pageName, String processName, String objectName, bool enableOverBorder, bool returnCurrentOver);
  bool setFlickedProcess      (String pageName, String processName, String objectName, bool enableOverBorder, bool returnCurrentOver);

  bool setDragProcess         (String pageName, String processName, String objectName);
  bool setDraggingProcess     (String pageName, String processName, String objectName, bool enableOverBorder, bool returnCurrentOver);
  bool setDraggedProcess      (String pageName, String processName, String objectName, bool enableOverBorder, bool returnCurrentOver);

  bool setClickedProcess      (String pageName, String processName, String objectName);
  bool setMultiClickedProcess (String pageName, String processName, String objectName, int count);

  bool startProcess(processType pType);
  bool update();

  String getCurrentProcess();
  bool isOverBorder();
  bool wasOverBorder();
};

/*
１．離れている時
離れているとき専用のプロセスセッターで登録されたもののみ、ここでプロセス名を返す
２．どこかがタッチされた
触れたときに判定するオブジェクトを判定し、範囲内にあるものだけを触れているオブジェクトとしてJSONに一時追加
３．移動せず離れた
「クリック判定をするもの」かつ範囲内のオブジェクトのプロセス名を返す
４．長押しせず移動した
「クリック判定」「短押し移動」のオブジェクトをJSONから削除して判定続行、離れれば現在範囲内のものを返す
５．長押しされた
「クリック」「短押し移動」をJSONから削除
６．長押し後、移動せず離れた
長押しから移動しないオブジェクトで、範囲内のものを返す
７．長押しして移動した
「長押し移動」以外のすべてのオブジェクトを削除する。離れたときに範囲内のオブジェクトを返す
*/