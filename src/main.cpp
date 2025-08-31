#include <Arduino.h>
#include <M5Unified.h>
#include "VisualData.hpp"
#include "TouchData.hpp"

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

// VisualData / TouchData の初期化
VisualData vData(&lcd, true, false, false);
TouchData tData(&vData, true, false, false);

// 現在ページ番号
int currentPageNum = -1;

void setup() {
  // M5 初期化
  auto cfg = M5.config();
  M5.begin(cfg);
  lcd.begin();
  lcd.setFont(&fonts::lgfxJapanGothic_40);
  lcd.setBrightness(32);

  Serial.begin(115200);

  // スプライト作成
  initSprite(sprite1, cDepth_24);
  tData.initJudgeSprite(&lcd);

  // --- page1 の設定 ---
  vData.addPage("page1");
  int page1 = vData.getPageNumByName("page1");
  tData.changeEditPage(page1);

  vData.setFillRectObject("obj1", 0, 0, 200, 140, BLUE);
  vData.setFillRectObject("obj2", 120, 0, 200, 140, RED);
  vData.setFillRectObject("obj3", 0, 100, 200, 140, YELLOW);
  vData.setFillRectObject("obj4", 120, 100, 200, 140, GREEN);

  tData.setPressProcess("process1", "obj1");
  tData.setPressProcess("process2", "obj2");
  tData.setPressProcess("process3", "obj3");
  tData.setPressProcess("process4", "obj4");
  tData.setReleaseProcess("process11", "obj1");
  tData.setReleaseProcess("process12", "obj2");
  tData.setReleaseProcess("process13", "obj3");
  tData.setReleaseProcess("process14", "obj4");

  // --- page2 の設定 ---
  vData.addPage("page2");
  int page2 = vData.getPageNumByName("page2");
  tData.changeEditPage(page2);

  vData.setFillRectObject("obj1", 0, 0, 200, 140, BLUE);
  vData.setFillRectObject("obj2", 120, 0, 200, 140, RED, 1);
  vData.setFillRectObject("obj3", 0, 100, 200, 140, YELLOW);
  vData.setFillRectObject("obj4", 120, 100, 200, 140, GREEN);

  tData.setPressProcess("process1", "obj1");
  tData.setPressProcess("process2", "obj2");
  tData.setPressProcess("process3", "obj3");
  tData.setPressProcess("process4", "obj4");

  vData.finalizeSetup();
  tData.finalizeSetup();

  // --- 初期ページを page1 に設定 ---
  currentPageNum = page1;
  tData.setProcessPage();

  // page1 を描画
  vData.drawPage(sprite1, "page1");
  sprite1.pushSprite(&lcd, 0, 0);
}

void loop() {
  M5.update();

  String currentPageName = vData.getPageData().pageName;
  String pageName = currentPageName;
  // ページ切り替え
  if (M5.BtnA.isPressed()) {
    pageName = "page1";
  } else if (M5.BtnB.isPressed()) {
    pageName = "page2";
  }

  if (pageName != currentPageName) {
    currentPageName = pageName;

    vData.drawPage(sprite1, currentPageName);
    sprite1.pushSprite(&lcd, 0, 0);
    tData.setProcessPage();
  }

  // タッチ処理
  if (tData.update()) {
    String proc = tData.currentProcessName;
    if (proc != "") {
      sprite1.pushSprite(&lcd, 0, 0);
      lcd.setCursor(10, 10);
      lcd.setTextColor(WHITE, BLACK);
      lcd.printf("Process: %s", proc.c_str());
    }
  }
}
