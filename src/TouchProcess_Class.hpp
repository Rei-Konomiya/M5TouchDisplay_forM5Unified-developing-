#pragma once

#include <vector>
#include <algorithm>

#define RELEASING = 0   // 離しているとき
#define PRESS_ST  = 1   // 押し始めた瞬間
#define RELEASED  = 2   // 離した瞬間

#define PRESSING  = 11  // 押しているとき
#define CLICKED   = 12  // 押してすぐ離したとき
#define FLICKING  = 13  // 長押しせずに移動しているとき

#define HOLDING   = 21  // 長押ししているとき
#define PRESSED   = 22  // 長押ししてから離したとき
#define DRAGGING  = 23  // 長押ししてから移動しているとき

struct Data{
  int mode;
  int x;
  int y;
  int w;
  int h;
};

struct touchData{
  int 
}

class TouchProcess{
public:
  /** @fn
   * @brief タッチ判定をするように設定する
   * 
   * @param object 描画オブジェクトのインスタンスを入れる。判定があったときに呼び出す用
   */
  void addTouchProcess(int mode, GraphicsObject object){
    if (M5.Touch.isEnabled()) {
      data.x, data.y, data.w, data.h = object.returnPosition();
      data.mode = mode;
      data.object = object;

      TouchList.push_back(data);
    }
  }

  /** @fn
   * @brief タッチされたときのコールバック関数実行
   * 
   * @param callback 実行するコールバック関数
   */
  typedef std::function <void(const String&)> CallbackFunction;
  void doSomething(CallbackFunction callback, const String& data_json) {
    // コールバック関数を呼び出す
    callback(data_json);
  }

  /** @fn
   * @brief 現在のタッチ条件に合うオブジェクトを検索する
   */
  void searchObjects(int mode, int x, int y){
    auto it = std::find_if(TouchList.begin(), TouchList.end(), [mode](const Data& data){
      return data.mode == mode;
    });
  }

  /** @fn
   * @brief タッチの状態を確認する
   */
  void checkTouchLocate(){
    M5.update();    // タッチやボタンの操作状態の更新。これがないと値は更新されない
    if (M5.Touch.isEnabled()) {
      auto t = M5.Touch.getDetail();  // タッチ操作に関する値の取得

      auto r = t.isReleased();  // 離れているとき1
      auto wp = t.wasPressed();   // 触れた瞬間のみ1
      auto wr = t.wasReleased();  // 離れた瞬間のみ1

      auto p = t.isPressed();   // 押されているとき1
      auto wc = t.wasClicked();   // 0.5秒以内に指が移動せず離れたとき1
      auto f = t.isFlicking();  // 触れてから0.5秒以内に移動したとき1

      auto h = t.isHolding();   // 0.5秒以上長押しされているとき1
      auto wh = t.isHolding() & t.isDragging();
      auto d = t.isDragging();  // 長押し状態で移動したとき1
    }
  }

private:
  Data data;
  std::vector<Data> TouchList;
};
