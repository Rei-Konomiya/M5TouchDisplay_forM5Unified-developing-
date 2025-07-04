#include <Arduino.h>
#include <M5Unified.h>
#include "VisualData.hpp"
#include "TouchData.hpp"

const int cDepth_1  = 1;
const int cDepth_8  = 8;
const int cDepth_16 = 16;
const int cDepth_24 = 24;

uint8_t spriteNum = 0;

void initSprite(LGFX_Sprite &sprite, int colorDepth) {
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


M5GFX lcd;
LGFX_Sprite sprite1(&lcd);
VisualData vData(&lcd, false, false, false);
// TouchDataコンストラクタの第三引数以降はデバッグログの有効/無効を制御します
// trueに設定すると、TouchData内部の処理ログが詳細に表示されます。
TouchData tData(&vData, &lcd, true, true, true); 

String currentPage = "page1"; // 現在のアクティブページ

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

  initSprite(sprite1, cDepth_24);

  // --- page1 の設定 ---
  vData.addPage("page1");
  vData.setFillRectObject("obj1", 0, 0, 200, 140, BLUE);
  vData.setFillRectObject("obj2", 120, 0, 200, 140, RED);
  vData.setFillRectObject("obj3", 0, 100, 200, 140, YELLOW);
  vData.setFillRectObject("obj4", 120, 100, 200, 140, GREEN);

  // --- page2 の設定 ---
  vData.addPage("page2");
  vData.setFillRectObject("obj1", 0, 0, 200, 140, BLUE);
  vData.setFillRectObject("obj2", 120, 0, 200, 140, RED, 1);
  vData.setFillRectObject("obj3", 0, 100, 200, 140, YELLOW);
  vData.setFillRectObject("obj4", 120, 100, 200, 140, GREEN);

  // --- page3 の設定 ---
  vData.addPage("page3");
  vData.setFillRectObject("obj1", 0, 0, 200, 140, BLUE);
  vData.setFillRectObject("obj2", 120, 0, 200, 140, RED, 1);
  vData.setFillRectObject("obj3", 0, 100, 200, 140, YELLOW, 1);
  vData.setFillRectObject("obj4", 120, 100, 200, 140, GREEN);


  tData.setPressProcess("page1", "process1", "obj1");
  tData.setPressProcess("page1", "process2", "obj2");
  tData.setPressProcess("page1", "process3", "obj3");
  tData.setPressProcess("page1", "process4", "obj4");
  tData.setReleaseProcess("page1", "process11", "obj1");
  tData.setReleaseProcess("page1", "process12", "obj2");
  tData.setReleaseProcess("page1", "process13", "obj3");
  tData.setReleaseProcess("page1", "process14", "obj4");  

  tData.setPressProcess("page2", "process1", "obj1");
  tData.setPressProcess("page2", "process2", "obj2");
  tData.setPressProcess("page2", "process3", "obj3");
  tData.setPressProcess("page2", "process4", "obj4");
  tData.setReleaseProcess("page2", "process11", "obj1");
  tData.setReleaseProcess("page2", "process12", "obj2");
  tData.setReleaseProcess("page2", "process13", "obj3");
  tData.setReleaseProcess("page2", "process14", "obj4");

  tData.setPressProcess("page3", "process1", "obj1");
  tData.setPressProcess("page3", "process2", "obj2");
  tData.setPressProcess("page3", "process3", "obj3");
  tData.setPressProcess("page3", "process4", "obj4");
  tData.setReleaseProcess("page3", "process11", "obj1");
  tData.setReleaseProcess("page3", "process12", "obj2");
  tData.setReleaseProcess("page3", "process13", "obj3");
  tData.setReleaseProcess("page3", "process14", "obj4");

  vData.getVisualData(); // デバッグ用

  // ★ ここで初期ページを設定
  // vData.getDrawingPage() はデフォルトで "defaultPage" を返す可能性があるため、
  // 明示的に "page1" を設定することが重要
  tData.setProcessPage(currentPage); 

  vData.drawPage(sprite1, currentPage); // 初期ページを描画
  sprite1.pushSprite(&lcd, 0, 0);
}

void loop() {
  M5.update();

  String newPage = currentPage;
  if(M5.BtnA.isPressed()){
    newPage = "page1";
  }else if(M5.BtnB.isPressed()){
    newPage = "page2";
  }else if(M5.BtnC.isPressed()){
    newPage = "page3";
  }

  if (newPage != currentPage) {
    currentPage = newPage;
    vData.drawPage(sprite1, currentPage); // スプライトに描画
    sprite1.pushSprite(&lcd, 0, 0);       // スプライトをLCDにプッシュ
    // ★ ページ切り替え時にTouchDataのプロセスも切り替える
    tData.setProcessPage(currentPage); 
  }

  if(tData.update()){ // TouchData の更新と判定を実行
    String proc = tData.getCurrentProcess();
    if(proc != "") {
      // 画面を一度クリアするか、描画するテキストの背景を塗りつぶすか検討
      // 例: M5.Lcd.fillRect(0, 220, M5.Lcd.width(), 20, BLACK); // 画面下部をクリア
      // または、メッセージをスプライトに描画し、pushSpriteする
      
      // M5.Lcd.print() の代わりに、スプライトの描画機能を使用
      // M5.Lcd.setCursor() や M5.Lcd.setTextColor() は M5.Lcd に直接影響するので注意
      
      // ★ 画面上の特定の場所にメッセージを表示する場合、スプライトを使うのが適切です ★
      // 例えば、メッセージ表示用の専用領域をスプライト内に確保し、そこに描画する
      // あるいは、メッセージ表示後にページ全体を再描画する

      // シンプルな例として、画面を一旦クリアしてメッセージを表示
      // ただし、これだと現在のページの描画が消えてしまうので、
      // 最終的にはスプライト上にテキストを描画して再プッシュするのが理想的です。
      
      // ページ全体の再描画を伴う場合の例 (パフォーマンスは落ちる可能性があります)
      vData.drawPage(sprite1, currentPage); // 現在のページ内容をスプライトに再描画
      sprite1.setCursor(10, 10); // スプライト上のカーソル位置を設定
      sprite1.setTextColor(WHITE, BLACK); // テキスト色と背景色を設定

      if (proc == "process1") {
          sprite1.print("Obj1 Pressed!"); 
      } else if (proc == "process2") {
          sprite1.print("Obj2 Pressed!"); 
      } else if (proc == "process3") {
          sprite1.print("Obj3 Pressed!"); 
      } else if (proc == "process4") {
          sprite1.print("Obj4 Pressed!"); 
      }
      else if (proc == "process11") {
          sprite1.print("Obj1 Released!"); 
      } else if (proc == "process12") {
          sprite1.print("Obj2 Released!"); 
      } else if (proc == "process13") {
          sprite1.print("Obj3 Released!"); 
      } else if (proc == "process14") {
          sprite1.print("Obj4 Released!"); 
      }
      sprite1.pushSprite(&lcd, 0, 0); // 更新されたスプライトをLCDにプッシュ
    }
  }
}