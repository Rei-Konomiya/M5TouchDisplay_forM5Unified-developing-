#include <Arduino.h>
#include <M5GFX.h>
#include <vector>

class TouchDataSet
{
public:
  enum class TouchType : uint8_t {
    Press,
    Pressing,
    Pressed,
    Release,
    Releasing,
    Hold,
    Holding,
    Held,
    Drag,
    Dragging,
    Dragged,
    Flick,
    Flicking,
    Flicked,
    Clicked,
    MultiClicked
  };

  // 個々のプロセス情報
  struct Process{
    String name;                    // プロセス名
    String objectName;              // 対象オブジェクト名
    TouchType type;                 // タッチタイプ
    bool enableOverBorder = false;  // スワイプ専用
    bool returnCurrentOver = false; // スワイプ専用
    uint32_t colorCode;             // タッチ判定用の固有色
    int zIndex = 0;                 // ZIndex
    int multiClickCount = 0;        // MultiClicked時のクリック数
  };

  // ページごとのプロセスリスト
  struct Page{
    String name;                    // ページ名
    std::vector<Process> processes; // プロセスリスト
  };

  // TouchDataSet 本体
  std::vector<Page> pages;

  // 現在判定中のプロセス名
  String currentProcessName;

  // メソッド
  Page *getOrCreatePage(const String &pageName);
  Process *createOrGetProcess(const String &pageName, const String &processName);
  Process *findProcessByColor(uint32_t colorCode, const String &pageName);
};
