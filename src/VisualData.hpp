#pragma once

#include <Arduino.h>
#include <M5Unified.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include <SD.h>
#include <LittleFS.h>


class VisualData{
public:
  enum class DrawType{
    DrawPixel,      // ピクセル 描画
    DrawLine,       // 直線 描画
    DrawBezier,     // ベジェ曲線 描画
    DrawWideLine,   // 太い線を描ける

    DrawRect,       // 矩形 描画
    DrawRoundRect,  // 角丸矩形 描画
    DrawTriangle,   // 三角形 描画
    DrawCircle,     // 円形 描画
    DrawEllipse,    // 楕円 描画
    DrawArc,        // 正円弧 描画
    DrawEllipseArc, // 楕円弧 描画

    FillRect,       // 矩形 描画
    FillRoundRect,  // 角丸矩形 描画
    FillTriangle,   // 三角形 描画
    FillCircle,     // 円形 描画
    FillEllipse,    // 楕円 描画
    FillArc,        // 正円弧 描画
    FillEllipseArc, // 楕円弧 描画
    
    DrawJpg,      // jpg ファイル描画
    DrawPng,      // png ファイル描画

    DrawString,     // 文字ポジション付き

    ClipArc,        // 正円弧 範囲制限
    ClipEllipseArc, // 楕円弧 範囲制限
    ClipRect,       // 矩形 範囲制限
    ClipRoundRect,  // 角丸矩形 範囲制限
    ClipCircle,     // 円形 範囲制限
    ClipEllipse,    // 楕円 範囲制限
    ClipTriangle,   // 三角形 範囲制限

    FlexBox,        // 親オブジェクト専用 一列限定 横 or  縦並び補助 範囲制限
    TableBox        // 親オブジェクト専用 行列指定 横 and 縦並び補助 範囲制限
  };

  /*
    データの作り方
    1. JSONを使用する
    2. 以下のようなキーと値の組み合わせにする
      例） {
        オブジェクト名：{（オブジェクト名が "" のときは objに番号を付けた名前にする）
          描画タイプ：DrawTypeのうちのどれか
          引数：[0, 0, 100, 50, BLACK]
        },
        オブジェクト２：{
          以下同様
        }
      }
    3. ページ設定も次のように設定する
      例）{
        ページ名：{
          オブジェクト名：{
            以下同様
          }
        }
      }
  */

  DynamicJsonDocument* visualData;

  JsonObject editingPage;
  int objectNum = 0;

  VisualData(int docSize);

  DynamicJsonDocument getVisualData();

  void checkVisualData();

  void addPage(String pageName);

  bool changeSettingPage(String pageName);
  void setParentObject(String objectName, String parentName);

  void setDrawPixelObject       (String objectName, int32_t x, int32_t y                                                    , int color);
  void setDrawLineObject        (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1                          , int color);
  void setDrawBezierObject      (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2  , int color);

  void setDrawRectObject        (String objectName, int32_t x, int32_t y, int32_t w, int32_t h                              , int color);
  void setFillRectObject        (String objectName, int32_t x, int32_t y, int32_t w, int32_t h                              , int color);
  void setDrawRoundRectObject   (String objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r                   , int color);
  void setFillRoundRectObject   (String objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r                   , int color);

  void setDrawCircleObject      (String objectName, int32_t x, int32_t y                      , int32_t r                   , int color);
  void setFillCircleObject      (String objectName, int32_t x, int32_t y                      , int32_t r                   , int color);
  void setDrawEllipseObject     (String objectName, int32_t x, int32_t y, int32_t rx, int32_t ry                            , int color);
  void setFillEllipseObject     (String objectName, int32_t x, int32_t y, int32_t rx, int32_t ry                            , int color);
  void setDrawTriangleObject    (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2  , int color);
  void setFillTriangleObject    (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2  , int color);

  void setDrawArcObject         (String objectName, int32_t x, int32_t y, int32_t r0, int32_t r1, float angle0, float angle1, int color);
  void setFillArcObject         (String objectName, int32_t x, int32_t y, int32_t r0, int32_t r1, float angle0, float angle1, int color);
  void setDrawEllipseArcObject  (String objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, float angle0, float angle1, int color);
  void setFillEllipseArcObject  (String objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, float angle0, float angle1, int color);

  void setDrawJpgObject         (String objectName, fs::File *dataSource, const char *path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, lgfx::v1::jpeg_div::jpeg_div_t scale);
  void setDrawPngObject         (String objectName, fs::File *dataSource, const char *path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, float scale_x, float scale_y);

  void setDrawStringObject      (String objectName, String text, int32_t x, int32_t y, int color, int bgcolor, uint8_t datum);

  void setFlexBoxObject(String objectName, int32_t x, int32_t y, int32_t w, int32_t h);
  void setTableBoxObject(String objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t grid_x, int32_t grid_y);

  // 並べるときの余白
  /*
    0 autoSize        : 最小値 0 で自動調整
    10 autoSize       : 最小値 10 で自動調整
    10 10             : 最小値最大値を 10 で固定
    autoSize 10       : 最大値 10 で自動調整
    autoSize autoSize : 残りの余白を均等分割
  */
  const int32_t autoSize = -1;
  void setFlexMargin(int32_t min_x, int32_t max_x, int32_t min_y, int32_t max_y);
  // 並べるときの基準点
  void setContentPosition(uint8_t datum);

  void drawPage(LGFX_Sprite &sprite, String pageName);
};