
#include <Arduino.h>
#include <M5Unified.h>
#include "VisualData.hpp"
#include "TouchData.hpp"

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
VisualData vData(&lcd, false, false, false);
TouchData tData(&vData, &lcd, true, true, true);

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
  // vData.setDrawPixelObject("", 10, 20, GREEN);
  // vData.setFillRectObject("", 30, 40, 60, 70, RED);
  vData.setFillRectObject("obj1", 0, 0, 200, 140, BLUE);
  vData.setFillRectObject("obj2", 120, 0, 200, 140, RED);
  vData.setFillRectObject("obj3", 0, 100, 200, 140, YELLOW);
  vData.setFillRectObject("obj4", 120, 100, 200, 140, GREEN);
  tData.setPressProcess("page1", "process1", "obj1");
  tData.setPressProcess("page1", "process2", "obj2");
  tData.setPressProcess("page1", "process3", "obj3");
  tData.setPressProcess("page1", "process4", "obj4");
  tData.setReleaseProcess("page1", "process11", "obj1");
  tData.setReleaseProcess("page1", "process12", "obj2");
  tData.setReleaseProcess("page1", "process13", "obj3");
  tData.setReleaseProcess("page1", "process14", "obj4");

  vData.addPage("page2");
  // vData.setDrawCircleObject("", 80, 90, 100, BLUE);
  // vData.setFillTriangleObject("", lcd.width(), lcd.height(), lcd.width(), 0, 0, lcd.height(), YELLOW);
  vData.setFillRectObject("obj1", 0, 0, 200, 140, BLUE);
  vData.setFillRectObject("obj2", 120, 0, 200, 140, RED, 1);
  vData.setFillRectObject("obj3", 0, 100, 200, 140, YELLOW);
  vData.setFillRectObject("obj4", 120, 100, 200, 140, GREEN);
  // tData.setPressProcess("page2", "process1", "obj1");
  // tData.setPressProcess("page2", "process2", "obj2");
  // tData.setPressProcess("page2", "process3", "obj3");
  // tData.setPressProcess("page2", "process4", "obj4");
  // tData.setReleaseProcess("page2", "process11", "obj1");
  // tData.setReleaseProcess("page2", "process12", "obj2");
  // tData.setReleaseProcess("page2", "process13", "obj3");
  // tData.setReleaseProcess("page2", "process14", "obj4");

  vData.addPage("page3");
  vData.setFillRectObject("obj1", 0, 0, 200, 140, BLUE);
  vData.setFillRectObject("obj2", 120, 0, 200, 140, RED, 1);
  vData.setFillRectObject("obj3", 0, 100, 200, 140, YELLOW, 1);
  vData.setFillRectObject("obj4", 120, 100, 200, 140, GREEN);
  // tData.setPressProcess("page3", "process1", "obj1");
  // tData.setPressProcess("page3", "process2", "obj2");
  // tData.setPressProcess("page3", "process3", "obj3");
  // tData.setPressProcess("page3", "process4", "obj4");
  // tData.setReleaseProcess("page3", "process11", "obj1");
  // tData.setReleaseProcess("page3", "process12", "obj2");
  // tData.setReleaseProcess("page3", "process13", "obj3");
  // tData.setReleaseProcess("page3", "process14", "obj4");

  vData.getVisualData();
  

  vData.drawPage(sprite1, "page1");
  sprite1.pushSprite(&lcd, 0, 0);
}

void loop() {
  M5.update();
  if(M5.BtnA.isPressed()){
    vData.drawPage(sprite1, "page1");
    sprite1.pushSprite(&lcd, 0, 0);
  }else if(M5.BtnB.isPressed()){
    vData.drawPage(sprite1, "page2");
    sprite1.pushSprite(&lcd, 0, 0);
  }else if(M5.BtnC.isPressed()){
    vData.drawPage(sprite1, "page3");
    sprite1.pushSprite(&lcd, 0, 0);
  }

  if(tData.update()){ // タッチ更新＆判定
    String proc = tData.getCurrentProcess();
    String pageName = vData.getDrawingPage();
    JsonObject pageProc = tData.processList[pageName];
    JsonObject processData = pageProc[proc].as<JsonObject>();
    String objName = processData["objectName"].as<String>();

    JsonObject objData = vData.getObjectData(pageName, objName);
    JsonArray args = objData["args"].as<JsonArray>();
    uint16_t color = args[4].as<uint16_t>();

    // Serial.println(proc);

    if(proc == "process1") vData.setFillRectObject("obj1", 0, 0, 200, 140, WHITE, 2);
    else if(proc == "process2") vData.setFillRectObject("obj2", 120, 0, 200, 140, WHITE, 2);
    else if(proc == "process3") vData.setFillRectObject("obj3", 0, 100, 200, 140, WHITE, 2);
    else if(proc == "process4") vData.setFillRectObject("obj4", 120, 100, 200, 140, WHITE, 2);

    else if(proc == "process11") vData.setFillRectObject("obj1", 0, 0, 200, 140, BLUE);
    else if(proc == "process12") {
      if(pageName == "page2" || pageName == "page3") vData.setFillRectObject("obj2", 120, 0, 200, 140, RED, 1);
      else vData.setFillRectObject("obj2", 120, 0, 200, 140, RED);
    }
    else if(proc == "process13") {
      if(pageName == "page3") vData.setFillRectObject("obj3", 0, 100, 200, 140, YELLOW, 1);
      else vData.setFillRectObject("obj3", 0, 100, 200, 140, YELLOW);
    } 
    else if(proc == "process14") vData.setFillRectObject("obj4", 120, 100, 200, 140, GREEN);
    // vData.drawPage(sprite1, pageName);
    sprite1.pushSprite(&lcd, 0, 0);
  }
}