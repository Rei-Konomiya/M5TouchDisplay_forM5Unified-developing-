#pragma once

#include <Arduino.h>
#include <M5Unified.h>
#include <vector>
#include "TouchProcess_Class.hpp"

// 描画タイプの定義
enum class Type { 
  TextType,
  ImageType,
  FigureType
};

// 図形タイプの定義
enum class FigureType {
  Circle,
  Rectangle,
  Triangle
};

// 共通の描画データ
struct BaseData {
  String name;
  Type type;
  int x;
  int y;
  int w;
  int h;
  uint16_t color;  // 16ビットカラー（例: RGB565）
  String ParentName;
};

// テキストデータの定義
struct TextData {
  String text;
  int fontSize;
};

// 画像データの定義
struct ImageData {
  int fitContent;  // フィットコンテンツのフラグ（例: 0 = なし, 1 = あり）
  // 画像データへの参照（例: プロミネントなライブラリではなく、アドレス参照）
  const uint8_t* imagePtr;
  int imageSize;
};

// 円形データの定義
struct CircleData {
  int arcAngle;
};

// 四角形データの定義
struct RectangleData {
  int radius;
};

// 三角形データの定義
struct TriangleData {
  int direction;
  int top;
};

// 図形データ共同体
union FiguresOptionData{
  CircleData circleData;
  RectangleData rectangleData;
  TriangleData triangleData;
};

// 図形データ全容
struct FigureData {
  FigureType figureType;
  FiguresOptionData figuresOptionData;
  bool fillMode;
};

// 型別追加データを保持する共用体
union ExtraData {
  TextData textData;
  ImageData imageData;
  FigureData figureData;
};

// 単一オブジェクトのデータを保持
struct ObjectData {
  BaseData base;
  ExtraData extra;
};

// １ページ内の全体のデータを保持
struct VisualData {
  int pageNum;
  std::vector<ObjectData> pageData;
};


// データを管理するクラス
class VisualManager {
public:
  void createPage(int pageNum);

  void changeEditPage(int pageNum);
  void changeEditObject(String name);

  void createObject(String name, Type type);
  void createObject(String name, Type type, int pageNum);
  void createObject(String parent, String name, Type type);
  void createObject(String parent, String name, Type type, int pageNum);

  void setBaseData(String name, int x, int y, int w, int h);

  void setFont(String name, lgfx::IFont* font, float fontSize);

  void setTextData(String name, String text);

  void setTextColor(String name, String text, int color);

  // void setImageData(String name, char *path);
  // void setImageData(String name, char *path, int fitContent);

  void setFigureType(String name, FigureType figureType, bool fillMode);

  void setFiguresData(String name, FiguresOptionData& figuresOptionData);

  void addObjectToPage();
  void addObjectToPage(int pageNum);
  void addObjectToPage(String name);
  void addObjectToPage(int pageNum, String name);

  VisualData getPages(int pageNum);

private:
  int defaultFontSize;

  int settingPage = 0;
  ObjectData currentObject;  // 現在のObjectDataを保持（使い回し用）
  std::vector<VisualData> pages;   // ページ全体のデータを保持
};