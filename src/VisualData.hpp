#ifndef VISUAL_DATA_HPP
#define VISUAL_DATA_HPP

#include <iostream>
#include <initializer_list>
#include <Arduino.h>
#include <M5Unified.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include <SD.h>
#include <LittleFS.h>
#include <vector>

#include "SerialDebug.h"
#include "VisualDataSet.h"

class VisualData{
  Debug debugLog;

  LGFX_Sprite clipSprite;

  VisualDataSet dataset;
  VisualDataSet::PageData editingPage;
  VisualDataSet::ObjectData editingObject;

  VisualDataSet::PageData currentPageCopy;

  bool isBatchUpdating = false;
  int objectNum = 0;
  int lastAssignedPageNum = 0;

  int lastAssignedObjectNum = 0;

  VisualData(LovyanGFX* parent, bool enableErrorLog, bool enableInfoLog, bool enableSuccessLog);

  bool isExistsPage(int pageNum) const;
  bool isExistsPageName(const String& name) const;
  bool isExistsObject(int objNum, int pageNum = -1) const;
  bool isExistsObjectName(const String& objectName, int pageNum = -1) const;
  bool isExistsParent(int pageNum, int objNum) const;
  bool isExistsChild(int pageNum, int objNum) const;
  bool isExistsEditingPage() const;

  int getPageNumByName(const String& name) const;
  int getObjectNumByName(const String& objectName, int pageNum = -1) const;

  const std::vector<VisualDataSet::PageData>& getVisualData() const;
  const VisualDataSet::PageData& getPageData(int pageNum) const;
  const VisualDataSet::ObjectData& getObjectData(const VisualDataSet::PageData& page, int objNum) const;
  
  std::vector<VisualDataSet::PageData>& getVisualDataRef();
  VisualDataSet::PageData* getPageDataRef(int pageNum);
  VisualDataSet::ObjectData* getObjectDataRef(VisualDataSet::PageData* page, int objNum);

  void beginPageUpdate();
  void endPageUpdate();

  bool addPage(const char* name = nullptr, int pageNum = -1);
  bool commitEditingPage();

  bool changeEditPage(int pageNum);
  bool deletePage(int pageNum);
  String getDrawingPage() const;

  VisualDataSet::ObjectData createObject(VisualDataSet::DrawType type, const String& objectName, const VisualDataSet::ObjectArgs& args, uint8_t zIndex);
  // ピクセル
  VisualDataSet::ObjectData setDrawPixelObject    (const String& objectName, int32_t x, int32_t y,                                                   int color, uint8_t zIndex = 0);
  // 線系
  VisualDataSet::ObjectData setDrawLineObject     (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1,                         int color, uint8_t zIndex = 0);
  VisualDataSet::ObjectData setDrawWideLineObject (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int r,                  int color, uint8_t zIndex = 0);
  VisualDataSet::ObjectData setDrawBezierObject   (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int color, uint8_t zIndex = 0);
  // 矩形系
  VisualDataSet::ObjectData setDrawRectObject       (const String& objectName, int32_t x, int32_t y, int32_t w, int32_t h,                           int color, uint8_t zIndex = 0);
  VisualDataSet::ObjectData setFillRectObject       (const String& objectName, int32_t x, int32_t y, int32_t w, int32_t h,                           int color, uint8_t zIndex = 0);
  VisualDataSet::ObjectData setDrawRoundRectObject  (const String& objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r,                int color, uint8_t zIndex = 0);
  VisualDataSet::ObjectData setFillRoundRectObject  (const String& objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r,                int color, uint8_t zIndex = 0);
  // 円・楕円・三角形
  VisualDataSet::ObjectData setDrawCircleObject   (const String& objectName, int32_t x,  int32_t y,  int32_t r,                                      int color, uint8_t zIndex = 0);
  VisualDataSet::ObjectData setFillCircleObject   (const String& objectName, int32_t x,  int32_t y,  int32_t r,                                      int color, uint8_t zIndex = 0);
  VisualDataSet::ObjectData setDrawEllipseObject  (const String& objectName, int32_t x,  int32_t y,  int32_t rx, int32_t ry,                         int color, uint8_t zIndex = 0);
  VisualDataSet::ObjectData setFillEllipseObject  (const String& objectName, int32_t x,  int32_t y,  int32_t rx, int32_t ry,                         int color, uint8_t zIndex = 0);
  VisualDataSet::ObjectData setDrawTriangleObject (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int color, uint8_t zIndex = 0);
  VisualDataSet::ObjectData setFillTriangleObject (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int color, uint8_t zIndex = 0);
  // アーク
  VisualDataSet::ObjectData setDrawArcObject        (const String& objectName, int32_t x, int32_t y, int32_t r0,  int32_t r1,  int32_t angle0, int32_t angle1,                                 int color, uint8_t zIndex = 0);
  VisualDataSet::ObjectData setFillArcObject        (const String& objectName, int32_t x, int32_t y, int32_t r0,  int32_t r1,  int32_t angle0, int32_t angle1,                                 int color, uint8_t zIndex = 0);
  VisualDataSet::ObjectData setDrawEllipseArcObject (const String& objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y,    int32_t r1y,    int32_t angle0, int32_t angle1, int color, uint8_t zIndex = 0);
  VisualDataSet::ObjectData setFillEllipseArcObject (const String& objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y,    int32_t r1y,    int32_t angle0, int32_t angle1, int color, uint8_t zIndex = 0);
  // 画像
  VisualDataSet::ObjectData setDrawJpgFileObject(const String& objectName, VisualDataSet::DataType dataSource, const char* path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, lgfx::v1::jpeg_div::jpeg_div_t scale, uint8_t zIndex = 0);
  VisualDataSet::ObjectData setDrawPngFileObject(const String& objectName, VisualDataSet::DataType dataSource, const char* path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, float scaleX, float scaleY,           uint8_t zIndex = 0);
  // 文字
  VisualDataSet::ObjectData setDrawStringObject(const String& objectName, int32_t x, int32_t y, const char* text, int color, int bgcolor, uint8_t zIndex = 0);


  /*
    checkCreatable  // 編集中ページの有無とオブジェクト重複確認（新規・更新・不可）
    deleteObject    // 編集中ページから指定オブジェクトの削除

    drawObject  // オブジェクトごとに描画
    drawPage    // ページ単位で描画
  */



  void finalizeSetup();
};

#endif // VISUAL_DATA_HPP