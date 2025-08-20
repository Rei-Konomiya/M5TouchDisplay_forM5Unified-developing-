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
  int lastAssignedPageNum = -1;

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

  /*
    changeEditPage  // 編集ページ変更
    deletePage      // ページ削除
    getDrawingPage  // 描画中ページ名

    checkCreatable  // 編集中ページの有無とオブジェクト重複確認（新規・更新・不可）
    deleteObject    // 編集中ページから指定オブジェクトの削除

    createTemplateObject  // オブジェクトの設定

    setDrawPixelObject
    setDrawLineObject
    setDrawBezierObject
    setDrawWideLineObject

    setDrawRectObject
    setFillRectObject
    setDrawRoundRectObject
    setFillRoundRectObject

    setDrawCircleObject
    setFillCircleObject
    setDrawEllipseObject
    setFillEllipseObject
    setDrawTriangleObject
    setFillTriangleObject

    setDrawArcObject
    setFillArcObject
    setDrawEllipseArcObject
    setFillEllipseArcObject

    setDrawJpgFileObject
    setDrawPngFileObject

    drawObject  // オブジェクトごとに描画
    drawPage    // ページ単位で描画
  */



  void finalizeSetup();
};

#endif // VISUAL_DATA_HPP