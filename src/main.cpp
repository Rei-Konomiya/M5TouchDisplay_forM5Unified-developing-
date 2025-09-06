#include <Arduino.h>
#include <M5Unified.h>
#include "VisualTouch.h"

// カラー深度
const int cDepth_24 = 16;
uint8_t spriteNum = 0;

// スプライト初期化関数
void initSprite(LGFX_Sprite &sprite, int colorDepth) {
  sprite.setPsram(true);
  sprite.setColorDepth(colorDepth);
  sprite.setFont(&fonts::lgfxJapanGothic_40);
  sprite.createSprite(320, 240);
  sprite.setPivot(0, 0);
}
// M5GFX オブジェクト
M5GFX lcd;

// スプライト
LGFX_Sprite sprite1(&lcd);
LGFX_Sprite sprite(&lcd);


void sSetup() {
  /*以下３つは表示用スプライトの設定*/
  sprite.setColorDepth(8);
  sprite.createSprite(lcd.width(), lcd.height());
  sprite.setTextSize(1.5);
}

// VisualData / TouchData の初期化
VisualTouch vt(&lcd, true, false, false);

// 現在ページ番号
int currentPageNum = -1;

bool isTester;

void setup() {
  // M5 初期化
  auto cfg = M5.config();
  M5.begin(cfg);
  lcd.begin();
  lcd.setFont(&fonts::lgfxJapanGothic_40);
  lcd.setBrightness(32);
  sSetup();

  Serial.begin(115200);

  // スプライト作成
  initSprite(sprite1, cDepth_24);
  vt.tData.initJudgeSprite(&lcd);

  // --- page1 の設定 ---
  vt.vData.addPage("page1");
  int page1 = vt.vData.getPageNumByName("page1");
  vt.tData.changeEditPage(page1);

  vt.vData.setFillRectObject("obj1", 0, 0, 200, 140, BLUE);
  vt.vData.setFillRectObject("obj2", 120, 0, 200, 140, RED);
  vt.vData.setFillRectObject("obj3", 0, 100, 200, 140, YELLOW);
  vt.vData.setFillRectObject("obj4", 120, 100, 200, 140, GREEN);

  vt.tData.setPressProcess("press1", "obj1");
  vt.tData.setPressProcess("press2", "obj2");
  vt.tData.setPressProcess("press3", "obj3");
  vt.tData.setPressProcess("press4", "obj4");
  vt.tData.setFlickProcess("flick1", "obj1");
  vt.tData.setFlickProcess("flick2", "obj2");
  vt.tData.setFlickProcess("flick3", "obj3");
  vt.tData.setFlickProcess("flick4", "obj4");
  vt.tData.setFlickingProcess("flicking1", "obj1");
  vt.tData.setFlickingProcess("flicking2", "obj2");
  vt.tData.setFlickingProcess("flicking3", "obj3");
  vt.tData.setFlickingProcess("flicking4", "obj4");
  vt.tData.setFlickedProcess("flicked1", "obj1");
  vt.tData.setFlickedProcess("flicked2", "obj2");
  vt.tData.setFlickedProcess("flicked3", "obj3");
  vt.tData.setFlickedProcess("flicked4", "obj4");

  // --- page2 の設定 ---
  vt.vData.addPage("page2");
  int page2 = vt.vData.getPageNumByName("page2");
  vt.tData.changeEditPage(page2);

  vt.vData.setFillRectObject("obj1", 0, 0, 200, 140, BLUE);
  vt.vData.setFillRectObject("obj2", 120, 0, 200, 140, RED, 1);
  vt.vData.setFillRectObject("obj3", 0, 100, 200, 140, YELLOW);
  vt.vData.setFillRectObject("obj4", 120, 100, 200, 140, GREEN);

  vt.tData.setHoldProcess("process1", "obj1");
  vt.tData.setHoldProcess("process2", "obj2");
  vt.tData.setHoldProcess("process3", "obj3");
  vt.tData.setHoldProcess("process4", "obj4");

  vt.vData.finalizeSetup();
  vt.tData.finalizeSetup();

  // --- 初期ページを page1 に設定 ---
  currentPageNum = page1;
  vt.tData.setProcessPage();

  // page1 を描画
  vt.vData.drawPage(sprite1, "page1");
  sprite1.pushSprite(&lcd, 0, 0);
}

void test(){
  lcd.startWrite();     // 表示処理を軽く早くするおまじない。endWrite()とセットで使う
  sprite.startWrite();  // 同上
  if (M5.Touch.isEnabled()) {
    auto t = M5.Touch.getDetail();  // タッチ操作に関する値の取得

    /*ここから非表示*/
    auto bMsec = t.base_msec; // 電源がついてからの時間(ms単位)、おそらく長押しなどの0.5秒の判定はこれを使っていると予想
    
    auto pr = t.prev; // 座標値のハード側の値を出したもの、値の中身は (prev_y * 65536) + prev_x で求められる
    auto prx = t.prev_x;  // x座標のハード値
    auto pry = t.prev_y;  // y座標のハード値
    
    auto st = t.state;    // 状態遷移をbit値で管理しているもの。is~やwas~関数で返すそれぞれの値を取得・保持している。使用することはおそらくない
    auto s = t.size;      // おそらく判定範囲。不明
    /*ここまで非表示*/

    auto p = t.isPressed();   // 押されているとき1
    auto r = t.isReleased();  // 離れているとき1
    auto h = t.isHolding();   // 0.5秒以上長押しされているとき1
    auto f = t.isFlicking();  // 触れてから0.5秒以内に移動したとき1
    auto d = t.isDragging();  // 長押し状態で移動したとき1

    auto wp = t.wasPressed();   // 触れた瞬間のみ1
    auto wr = t.wasReleased();  // 離れた瞬間のみ1
    auto wc = t.wasClicked();   // 0.5秒以内に指が移動せず離れたとき1
    auto wh = t.wasHold();      // 0.5秒以上長押しされて移動せず離れたとき1
    auto wfs = t.wasFlickStart(); // フリック開始時のみ1
    auto wf = t.wasFlicked();   // 0.5秒以内に移動して離れたとき1
    auto wds = t.wasDragStart();  //ドラッグ開始時のみ1
    auto wd = t.wasDragged();   // 長押し状態で移動して離れたとき1

    auto x = t.x;         // 現在触れているx座標
    auto y = t.y;         // 現在触れているy座標
    auto bx = t.base_x;   // 触れた瞬間のx座標
    auto by = t.base_y;   // 触れた瞬間のy座標

    auto dix = t.distanceX(); // 触れた瞬間と現在触れている座標のx距離
    auto diy = t.distanceY(); // 触れた瞬間と現在触れている座標のy距離
    auto dex = t.deltaX();    // 前回検知した座標と現在検知した座標のx距離 (=ドラッグ・フリックx速度)
    auto dey = t.deltaY();    // 前回検知した座標と現在検知した座標のy距離 (=ドラッグ・フリックy速度)

    //同一値のためメンバは非表示
    auto cCount = t.click_count;  // 同じ座標で連続クリックされた回数（ホールド・フリック・別座標でのクリック、0.5秒以上の操作なしで0に戻る）
    auto gcCount = t.getClickCount(); // click_countの値を返す関数。つまりclick_countと同じ

    auto id = t.id;   // 2点同時押し時に1、ただしy座標がほぼ端から端まで離れていないと1点としてカウントされる為実用化は難しい

    sprite.setCursor(0, 0);

    // 1=true, 0=false
    sprite.printf("isPressed = %d, isReleased = %d\n", p, r);
    sprite.printf("isHolding = %d\n", h);
    sprite.printf("isFlicking = %d, isDragging = %d\n", f, d);
    sprite.println();

    // 1=true, 0=false 一瞬しか 1 にならないので見逃し注意
    sprite.printf("wasPressed = %d, wasReleased = %d\n", wp, wr);
    sprite.printf("wasClicked = %d, wasHold = %d\n", wc, wh);
    sprite.printf("wasFlickStart = %d, wasFlicked = %d\n", wfs, wf);
    sprite.printf("wasDragStart = %d, wasDragged = %d\n", wds, wd);
    sprite.println();

    // 座標値表示
    sprite.printf("x = %3d, y = %3d\n", x, y);
    sprite.printf("base_x = %3d, base_y = %3d\n", bx, by);
    sprite.println();

    // 座標値表示  左または上に向かって動かすと負数になる
    sprite.printf("distanceX = %4d, distanceY = %4d\n", dix, diy);
    sprite.printf("deltaX = %3d, deltaY = %3d\n", dex, dey);
    sprite.println();

    // どちらも判定が難しいため、動作を確認するには何度も試す必要アリ
    sprite.printf("getClickCount = %2d\n", gcCount);
    sprite.printf("id = %d\n", id);
  }

  sprite.pushSprite(&lcd, 0, 0); // これを消すと画面描画がされなくなるため注意
  sprite.endWrite();  // おまじない
  lcd.endWrite();     // おまじない

  delay(1); // 更新間隔は1ミリ秒で設定
}

void loop() {
  M5.update();

  String currentPageName = vt.vData.getPageData().pageName;
  String pageName = currentPageName;
  // ページ切り替え
  if (M5.BtnA.isPressed()) {
    pageName = "page1";
  } else if (M5.BtnB.isPressed()) {
    pageName = "page2";
  } else if (M5.BtnC.isPressed()) {
    isTester = !isTester;
  }

  if(isTester){
    test();
  }else{
    sprite1.pushSprite(&lcd, 0, 0);
    if (pageName != currentPageName) {
      currentPageName = pageName;

      vt.vData.drawPage(sprite1, currentPageName);
      sprite1.pushSprite(&lcd, 0, 0);
      vt.tData.setProcessPage();
    }

    // タッチ処理
    if (vt.tData.update()) {
      String proc = vt.tData.currentProcessName;
      if (proc != "") {
        Serial.printf("Process: %s\n", proc.c_str());
      }
    }
  }
}

