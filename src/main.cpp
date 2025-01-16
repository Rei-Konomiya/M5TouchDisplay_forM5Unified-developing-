
#include <Arduino.h>
#include <Arduino.h>
#include <M5Unified.h>
#include "VisualData.hpp"

const int cDepth_1  = 1;
const int cDepth_8  = 8;
const int cDepth_16 = 16;
const int cDepth_24 = 24;

uint8_t spriteNum = 0;

// これでスプライトを全て初期化できる
void initSprite(LGFX_Sprite &sprite, int colorDepth) {
  sprite.setPsram(true);
  sprite.setColorDepth(colorDepth);
  sprite.setFont(&fonts::lgfxJapanGothic_40);
  sprite.createSprite(320, 240);  // 例として320x240で作成
  sprite.setPivot(0, 0);
  
  Serial.print("スプライト");
  Serial.print(++spriteNum);
  Serial.println("作成後");
  Serial.println(ESP.getFreeHeap());
  Serial.println(ESP.getFreePsram());
}


M5GFX lcd;
LGFX_Sprite sprite1(&lcd);
VisualData vData(true, true, true);

void setup(){
  auto cfg = M5.config();
  M5.begin(cfg);
  lcd.begin();
  lcd.setFont(&fonts::lgfxJapanGothic_40);
  lcd.setBrightness(32);
  Serial.begin(115200);

  Serial.print("スプライト作成前: ");
  Serial.println(ESP.getFreeHeap());
  Serial.println(ESP.getFreePsram());

  initSprite(sprite1, cDepth_24);


  vData.addPage("page1");
  vData.setDrawPixelObject("", 10, 20, BLACK);
  vData.setFillRectObject("", 30, 40, 60, 70, RED);
  vData.addPage("page2");
  vData.setDrawCircleObject("", 80, 90, 100, BLUE);
  vData.getVisualData();

  vData.drawPage(sprite1, "page1");
  vData.drawPage(sprite1, "page2");
}

void loop() {
}