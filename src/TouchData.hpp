#ifndef TOUCHDATA_HPP
#define TOUCHDATA_HPP

#include <M5Unified.h>
#include <ArduinoJson.h>
#include "VisualData.hpp"
#include "SerialDebug.h"

class VisualData; 

class TouchData{
public:
    // ★ 修正: StaticJsonDocument ではなく JsonDocument を使用
    JsonDocument processListDoc;    // processListのメモリ管理用
    JsonObject processList;         // 全ページのプロセスを保持

    JsonDocument enabledDoc;        // enabledProcessListのメモリ管理用
    JsonObject enabledProcessList;  // 現在有効なプロセスを保持

    JsonObject processPage;         // 現在選択されているページのプロセス
    
private:
    VisualData* vData;
    LovyanGFX* parent;
    LGFX_Sprite* judgeSprite;
    String currentProcessName = "";
    Debug debugLog;

public:
    enum class touchType : int {
        Release = 0, Releasing = 1, Press = 2, Pressing = 3, Pressed = 4,
        Hold = 5, Holding = 6, Held = 7, Flick = 8, Flicking = 9, Flicked = 10,
        Drag = 11, Dragging = 12, Dragged = 13, Clicked = 14, MultiClicked = 15
    };

    TouchData (VisualData* vData, LovyanGFX* parent, bool enableErrorLog, bool enableInfoLog, bool enableSuccessLog);

    void setProcessPage(String pageName);

    bool isExistsProcessPage (String pageName);
    bool isExistsProcess (String pageName, String processName);
    String isExistsProcessType(String pageName, String objectName, touchType type);
    JsonObject createOrGetProcess(String pageName, String processName);
    bool setTemplateProcess (String pageName, String processName, String objectName, touchType type, bool enableOverBorder = false, bool returnCurrentOver = false);
    
    // 各種setProcess関数
    bool setReleaseProcess(String pageName, String processName, String objectName);
    bool setReleasingProcess (String pageName, String processName, String objectName);
    bool setPressProcess(String pageName, String processName, String objectName);
    bool setPressingProcess(String pageName, String processName, String objectName);
    bool setPressedProcess(String pageName, String processName, String objectName);
    bool setHoldProcess (String pageName, String processName, String objectName);
    bool setHoldingProcess (String pageName, String processName, String objectName);
    bool setHeldProcess (String pageName, String processName, String objectName);
    bool setFlickProcess (String pageName, String processName, String objectName);
    bool setFlickingProcess (String pageName, String processName, String objectName, bool enableOverBorder = false, bool returnCurrentOver = false);
    bool setFlickedProcess (String pageName, String processName, String objectName, bool enableOverBorder = false, bool returnCurrentOver = false);
    bool setDragProcess (String pageName, String processName, String objectName);
    bool setDraggingProcess (String pageName, String processName, String objectName, bool enableOverBorder = false, bool returnCurrentOver = false);
    bool setDraggedProcess (String pageName, String processName, String objectName, bool enableOverBorder = false, bool returnCurrentOver = false);
    bool setClickedProcess (String pageName, String processName, String objectName);
    bool setMultiClickedProcess (String pageName, String processName, String objectName, int count);

    bool enableProcess(bool isPress);
    bool disableProcessType(touchType tType);
    bool disableProcess(String processName);
    bool judgeProcess(uint32_t x, uint32_t y);
    bool update();
    String getCurrentProcess();
    bool isOverBorder();
    bool wasOverBorder();
};

#endif // TOUCHDATA_HPP