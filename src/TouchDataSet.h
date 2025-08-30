#include <Arduino.h>
#include <M5GFX.h>
#include <vector>

class TouchDataSet {
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
  struct ProcessData {
    int processNum      = -1;
    String processName  = "";
    int objectNum       = -1;                 // 対象オブジェクト番号
    TouchType type      = TouchType::Clicked; // デフォルトはClicked
    bool enableOverBorder   = false;          // スワイプ専用
    bool returnCurrentOver  = false;          // スワイプ専用
    int multiClickCount     = 0;              // MultiClicked時のクリック数
    uint32_t colorCode      = 0;               // タッチ判定用の固有色

    bool isEmpty() const {
      return processNum == -1;  // 無効プロセスの判定
    }
  };

  // ページごとのプロセスリスト
  struct PageData {
    int pageNum   = -1;
    std::vector<ProcessData> processes;

    bool isEmpty() const {
      return pageNum == -1;  // 無効ページの判定
    }
  };

  // オブジェクトごとのカラーリスト
  struct objectColor {
    int objectNum = -1;
    uint32_t colorCode = 0;

    bool isEmpty() const {
      return objectNum == -1;  // 無効ページの判定
    }
  };

  struct ocPageData {
    int pageNum = -1;
    std::vector<objectColor> objectColors;

    bool isEmpty() const {
      return pageNum == -1;  // 無効ページの判定
    }
  };

  // TouchDataSet 本体
  std::vector<PageData> pages;
  std::vector<ocPageData> ocPages;
};
