#pragma once

#include <iostream>
#include <initializer_list>
#include <Arduino.h>
#include <M5Unified.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include <SD.h>
#include <LittleFS.h>

#include "SerialDebug.h"


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
    
    DrawJpgFile,    // jpg ファイル描画
    DrawPngFile,    // png ファイル描画

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

  enum class dataType{
    SD,
    SPIFFS
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

  Debug debugLog;

  JsonDocument visualData;

  JsonObject *editingPage = new JsonObject();
  String drawingPageName;
  int objectNum = 0;

  VisualData(LovyanGFX* parent, bool enableErrorLog, bool enableInfoLog, bool enableSuccessLog);

  JsonDocument getVisualData();

  bool isExistsPage(String pageName);
  bool isExistsObject(String objectName);
  bool isExistsObjectToPage(String pageName, String objectName);
  bool isExistsKey(String objectName, String keyName);
  bool isSetEditingPage();

  bool addPage(String pageName);

  bool changeSettingPage(String pageName);
  //bool setParentObject(String objectName, String parentName);

  uint8_t checkCreatable(String objectName);

  bool deleteData(String pageName, String keyName);
  bool deletePage(String pageName);
  bool deleteObject(String objectName);

  bool createTemplateObject(String objectName, int drawType, std::initializer_list<int> argsList);

  bool setDrawPixelObject       (String objectName, int32_t x, int32_t y                                                    , int color);
  bool setDrawLineObject        (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1                          , int color);
  bool setDrawBezierObject      (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2  , int color);
  bool setDrawWideLineObject    (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t r               , int color);

  bool setDrawRectObject        (String objectName, int32_t x, int32_t y, int32_t w, int32_t h                              , int color);
  bool setFillRectObject        (String objectName, int32_t x, int32_t y, int32_t w, int32_t h                              , int color);
  bool setDrawRoundRectObject   (String objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r                   , int color);
  bool setFillRoundRectObject   (String objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r                   , int color);

  bool setDrawCircleObject      (String objectName, int32_t x, int32_t y                      , int32_t r                   , int color);
  bool setFillCircleObject      (String objectName, int32_t x, int32_t y                      , int32_t r                   , int color);
  bool setDrawEllipseObject     (String objectName, int32_t x, int32_t y, int32_t rx, int32_t ry                            , int color);
  bool setFillEllipseObject     (String objectName, int32_t x, int32_t y, int32_t rx, int32_t ry                            , int color);
  bool setDrawTriangleObject    (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2  , int color);
  bool setFillTriangleObject    (String objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2  , int color);

  bool setDrawArcObject         (String objectName, int32_t x, int32_t y, int32_t r0, int32_t r1, int32_t angle0, int32_t angle1, int color);
  bool setFillArcObject         (String objectName, int32_t x, int32_t y, int32_t r0, int32_t r1, int32_t angle0, int32_t angle1, int color);
  bool setDrawEllipseArcObject  (String objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, int32_t angle0, int32_t angle1, int color);
  bool setFillEllipseArcObject  (String objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, int32_t angle0, int32_t angle1, int color);

  bool setDrawJpgFileObject     (String objectName, dataType dataSource, const char *path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, lgfx::v1::jpeg_div::jpeg_div_t scale);
  bool setDrawPngFileObject     (String objectName, dataType dataSource, const char *path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, float scale_x, float scale_y);

  bool setDrawStringObject      (String objectName, String text, int32_t x, int32_t y, int color, int bgcolor, uint8_t datum);

  //bool setFlexBoxObject(String objectName, int32_t x, int32_t y, int32_t w, int32_t h);
  //bool setTableBoxObject(String objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t grid_x, int32_t grid_y);

  // 並べるときの余白
  /*
    0 autoSize        : 最小値 0 で自動調整
    10 autoSize       : 最小値 10 で自動調整
    10 10             : 最小値最大値を 10 で固定
    autoSize 10       : 最大値 10 で自動調整
    autoSize autoSize : 残りの余白を均等分割
  */
  //const int32_t autoSize = -1;
  //bool setFlexMargin(int32_t min_x, int32_t max_x, int32_t min_y, int32_t max_y);
  // 並べるときの基準点
  //bool setContentPosition(uint8_t datum);

  String getDrawingPage();

  LGFX_Sprite clipSprite;

  bool drawObject(LGFX_Sprite &sprite, DrawType type, JsonArray args);
  bool drawPage(LGFX_Sprite &sprite, String pageName);
};