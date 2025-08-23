#include <M5Unified.h>
#include "VisualData.hpp"

const int cDepth_1 = 1;
const int cDepth_8 = 8;
const int cDepth_16 = 16;
const int cDepth_24 = 24;

uint8_t spriteNum = 0;

M5GFX lcd;
LGFX_Sprite sprite1(&lcd);
VisualData vData(&lcd, false, false, false);

void initSprite(LGFX_Sprite &sprite, int colorDepth)
{
  sprite.setPsram(true);
  sprite.setColorDepth(colorDepth);
  sprite.setFont(&fonts::lgfxJapanGothic_40);
  sprite.createSprite(320, 240);
  sprite.setPivot(0, 0);
  Serial.print("スプライト");
  Serial.print(++spriteNum);
  Serial.println("作成後");
  Serial.println(ESP.getFreeHeap());
  Serial.println(ESP.getFreePsram());
}

void setup(){
  auto cfg = M5.config();
  M5.begin(cfg);
  lcd.begin();
  lcd.setFont(&fonts::lgfxJapanGothic_40);
  lcd.setBrightness(32);
  Serial.begin(115200); // シリアル通信の初期化

  Serial.print("スプライト作成前: ");
  Serial.println(ESP.getFreeHeap());
  Serial.println(ESP.getFreePsram());
  initSprite(sprite1, cDepth_24); // ページ追加
  
  vData.addPage("Page1");
  vData.setFillRectObject("Rect1", 50, 50, 100, 60, RED, 0, false);
  vData.setFillRectObject("Rect2", 80, 150, 120, 80, GREEN, 1, false);
  vData.addPage("Page2");
  vData.setFillRectObject("Rect3", 30, 40, 150, 90, BLUE, 0, false);
  vData.addPage("Page3");
  vData.setFillRectObject("Rect4", 60, 60, 100, 100, YELLOW, 0, false);
  
  vData.finalizeSetup();

  // 最初のページ表示
  vData.drawPage(sprite1, vData.getPageData(vData.getPageNumByName("Page1"))); // setup完了後の処理まとめ
  sprite1.pushSprite(&lcd, 0, 0);
}

void loop(){
  M5.update(); // BtnA～Cでページ切り替え
  if (M5.BtnA.wasPressed()){
    Serial.println("drawing page1");
    sprite1.fillSprite(WHITE);
    vData.drawPage(sprite1, vData.getPageData(vData.getPageNumByName("Page1")));
    sprite1.pushSprite(&lcd, 0, 0);
  }

  if (M5.BtnB.wasPressed()){
    Serial.println("drawing page2");
    sprite1.fillSprite(WHITE);
    vData.drawPage(sprite1, vData.getPageData(vData.getPageNumByName("Page2")));
    sprite1.pushSprite(&lcd, 0, 0);
  }

  if (M5.BtnC.wasPressed()){
    Serial.println("drawing page3");
    sprite1.fillSprite(WHITE);
    vData.drawPage(sprite1, vData.getPageData(vData.getPageNumByName("Page3")));
    sprite1.pushSprite(&lcd, 0, 0);
  }

  delay(10); // 軽く待機
}
