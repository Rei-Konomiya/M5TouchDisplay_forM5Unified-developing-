// OldTouchData.hpp

#ifndef OLD_TOUCH_DATA_HPP
#define OLD_TOUCH_DATA_HPP

#include <Arduino.h>
#include <M5Unified.h>
#include <vector> // std::vector を使用
#include <algorithm> // std::sort を使用
#include <ArduinoJson.h> // ArduinoJson ライブラリを使用
#include "OldVisualData.hpp"
#include "SerialDebug.h" // DebugLog クラスを使用

// OldTouchData.hpp で定義
enum class touchType {
    None,
    Press,       // 押された瞬間
    Pressing,    // 押し続けている間
    Pressed,     // 押された状態が終了
    Release,     // 離された瞬間
    Releasing,   // 離し続けている間
    Hold,        // 短いホールド開始
    Holding,     // ホールド中
    Held,        // ホールド終了
    Flick,       // フリック開始（移動距離が閾値を超えた瞬間）
    Flicking,    // フリック中
    Flicked,     // フリック終了（指が離れた後）
    Drag,        // ドラッグ開始
    Dragging,    // ドラッグ中
    Dragged,     // ドラッグ終了（指が離れた後）
    Clicked,     // 短いクリック
    MultiClicked // マルチクリック
};

// パフォーマンス計測結果を格納する構造体
struct PerformanceMeasurement {
    String name;
    unsigned long duration_us;
};

class OldTouchData {
private:
    OldVisualData* vData;
    LovyanGFX* parent;
    LGFX_Sprite* judgeSprite;

    Debug debugLog;

    JsonDocument processListDoc; // プロセスリスト全体を管理するDoc
    JsonObject processList;     // 全ページにわたるプロセスリスト
    JsonObject processPage;     // 現在選択されているページのプロセスリスト

    JsonDocument enabledDoc;    // 有効なプロセスリストを管理するDoc
    JsonObject enabledProcessList; // 現在有効になっているプロセスリスト

    String currentProcessName;

    std::vector<PerformanceMeasurement> perfMeasurements; // パフォーマンス計測結果を格納

    // 内部で使用するヘルパー関数
    bool isExistsProcessPage(String pageName);
    bool isExistsProcess(String pageName, String processName);
    String isExistsProcessType(String pageName, String objectName, touchType type);
    JsonObject createOrGetProcess(String pageName, String processName);
    bool setTemplateProcess(String pageName, String processName, String objectName, touchType type, bool enableOverBorder = false, bool returnCurrentOver = false);
    
    // パフォーマンス計測結果をクリアするメソッド
    void clearPerformanceMeasurements();

public:
    OldTouchData(OldVisualData* vData, LovyanGFX* parent, bool enableErrorLog = false, bool enableInfoLog = false, bool enableSuccessLog = false);
    ~OldTouchData(); // デストラクタ

    void setProcessPage(String pageName);

    bool setReleaseProcess(String pageName, String processName, String objectName);
    bool setReleasingProcess(String pageName, String processName, String objectName);
    bool setPressProcess(String pageName, String processName, String objectName);
    bool setPressingProcess(String pageName, String processName, String objectName);
    bool setPressedProcess(String pageName, String processName, String objectName);

    bool setHoldProcess(String pageName, String processName, String objectName);
    bool setHoldingProcess(String pageName, String processName, String objectName);
    bool setHeldProcess(String pageName, String processName, String objectName);

    bool setFlickProcess(String pageName, String processName, String objectName);
    bool setFlickingProcess(String pageName, String processName, String objectName, bool enableOverBorder = false, bool returnCurrentOver = false);
    bool setFlickedProcess(String pageName, String processName, String objectName, bool enableOverBorder = false, bool returnCurrentOver = false);

    bool setDragProcess(String pageName, String processName, String objectName);
    bool setDraggingProcess(String pageName, String processName, String objectName, bool enableOverBorder = false, bool returnCurrentOver = false);
    bool setDraggedProcess(String pageName, String processName, String objectName, bool enableOverBorder = false, bool returnCurrentOver = false);

    bool setClickedProcess(String pageName, String processName, String objectName);
    bool setMultiClickedProcess(String pageName, String processName, String objectName, int count);

    bool enableProcess(bool isPress);
    bool disableProcessType(touchType tType);
    bool disableProcess(String processName);

    bool judgeProcess(uint32_t x, uint32_t y);
    bool update();
    String getCurrentProcess();

    bool isOverBorder();
    bool wasOverBorder();
};

#endif // OLD_TOUCH_DATA_HPP