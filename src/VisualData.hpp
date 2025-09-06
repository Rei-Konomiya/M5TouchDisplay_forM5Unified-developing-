#ifndef VISUAL_DATA_HPP
#define VISUAL_DATA_HPP

#include <iostream>
#include <initializer_list>
#include <Arduino.h>
#include <M5Unified.h>
#include <FS.h>
#include <SPIFFS.h>
#include <SD.h>
#include <LittleFS.h>
#include <JPEGDecoder.h>
#include <pngle.h>

#include "SerialDebug.h"
#include "VisualDataSet.h"

class VisualData{
public:
  Debug debugLog;
  using VDS = VisualDataSet;

  LGFX_Sprite clipSprite;

  VDS visualDataSet;
  VDS::PageData editingPage;

  VDS::PageData currentPageCopy;

  bool isBatchUpdating = false;
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

  const std::vector<VDS::PageData>& getVisualData() const;
  const VDS::PageData& getPageData(int pageNum = -1) const;
  const VDS::ObjectData& getObjectData(const VDS::PageData& page, int objNum) const;
  
  std::vector<VDS::PageData>& getVisualDataRef();
  VDS::PageData* getPageDataRef(int pageNum);
  VDS::ObjectData* getObjectDataRef(VDS::PageData* page, int objNum);

  void beginVisualUpdate();
  void endVisualUpdate();

  bool addPage(const char* name = nullptr, int pageNum = -1);
  bool commitVisualEdit();

  bool changeEditPage(int pageNum);
  bool deletePage(int pageNum);
  String getDrawingPage() const;

  VDS::ObjectData checkCreatable(const String& objectName, bool onDisplay = false);
  bool deleteObject(const String& objectName, bool onDisplay = false);
  bool moveObject(const String& objectName, size_t newIndex, bool onDisplay = false);

  VDS::ObjectData& createOrUpdateObject(VDS::DrawType type, const String& objectName, const VDS::ObjectArgs& args, uint8_t zIndex, bool isUntouchable, bool onDisplay);
  // ピクセル
  VDS::ObjectData setDrawPixelObject    (const String& objectName, int32_t x, int32_t y,                                                   int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  // 線系
  VDS::ObjectData setDrawLineObject     (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1,                         int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  VDS::ObjectData setDrawWideLineObject (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int r,                  int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  VDS::ObjectData setDrawBezierObject   (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  // 矩形系
  VDS::ObjectData setDrawRectObject       (const String& objectName, int32_t x, int32_t y, int32_t w, int32_t h,                           int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  VDS::ObjectData setFillRectObject       (const String& objectName, int32_t x, int32_t y, int32_t w, int32_t h,                           int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  VDS::ObjectData setDrawRoundRectObject  (const String& objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r,                int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  VDS::ObjectData setFillRoundRectObject  (const String& objectName, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r,                int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  // 円・楕円・三角形
  VDS::ObjectData setDrawCircleObject   (const String& objectName, int32_t x,  int32_t y,  int32_t r,                                      int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  VDS::ObjectData setFillCircleObject   (const String& objectName, int32_t x,  int32_t y,  int32_t r,                                      int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  VDS::ObjectData setDrawEllipseObject  (const String& objectName, int32_t x,  int32_t y,  int32_t rx, int32_t ry,                         int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  VDS::ObjectData setFillEllipseObject  (const String& objectName, int32_t x,  int32_t y,  int32_t rx, int32_t ry,                         int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  VDS::ObjectData setDrawTriangleObject (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  VDS::ObjectData setFillTriangleObject (const String& objectName, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  // アーク
  VDS::ObjectData setDrawArcObject        (const String& objectName, int32_t x, int32_t y, int32_t r0,  int32_t r1,  int32_t angle0, int32_t angle1,                                 int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  VDS::ObjectData setFillArcObject        (const String& objectName, int32_t x, int32_t y, int32_t r0,  int32_t r1,  int32_t angle0, int32_t angle1,                                 int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  VDS::ObjectData setDrawEllipseArcObject (const String& objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y,    int32_t r1y,    int32_t angle0, int32_t angle1, int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  VDS::ObjectData setFillEllipseArcObject (const String& objectName, int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y,    int32_t r1y,    int32_t angle0, int32_t angle1, int color, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  // 画像
  VDS::ObjectData setDrawJpgFileObject(const String& objectName, VDS::DataType dataSource, const char* path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, float scaleX, float scaleY, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  VDS::ObjectData setDrawPngFileObject(const String& objectName, VDS::DataType dataSource, const char* path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, float scaleX, float scaleY, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  VDS::ObjectData setDrawBitmapObject ( const String& objectName, const uint16_t* bitmap, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);
  // 文字
  VDS::ObjectData setDrawStringObject(const String& objectName, int32_t x, int32_t y, const char* text, int color = WHITE, int bgcolor = -1, const lgfx::IFont* font = &fonts::lgfxJapanGothic_40, textdatum_t datum = textdatum_t::top_left, int textSize = 1, bool textWrap = true, uint8_t zIndex = 0, bool isUntouchable = false, bool onDisplay = false);

  bool getJpgSize(fs::FS &fs, const char* filename, int &w, int &h);

  static int g_pngWidth;
  static int g_pngHeight;

  static void pngle_on_draw(pngle_t *png, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const uint8_t *rgba);
  static void pngle_on_header(pngle_t *png, uint32_t w, uint32_t h);
  bool getPngSize(File &file, int &width, int &height);

  bool drawObject(LGFX_Sprite &sprite, const VDS::ObjectData &obj);
  bool drawPage(LGFX_Sprite &sprite, const String pageName);

  /*
    drawObject  // オブジェクトごとに描画
    drawPage    // ページ単位で描画
  */



  void finalizeSetup();
};

#endif // VISUAL_DATA_HPP