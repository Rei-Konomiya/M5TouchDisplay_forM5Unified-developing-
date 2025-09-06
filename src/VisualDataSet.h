#include <Arduino.h>
#include <M5GFX.h>
#include <vector>
#include <SD.h>

class VisualDataSet{
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
    DrawBitmap,     // bitmap ファイル描画

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

  enum class DataType{
    SD,
    SPIFFS
  };



  struct PixelArgs      { int32_t x = 0;  int32_t y = 0;                                                                                                                 int color = 0; };
  struct LineArgs       { int32_t x0 = 0; int32_t y0 = 0; int32_t x1 = 0;  int32_t y1 = 0;                                                                               int color = 0; };
  struct BezierArgs     { int32_t x0 = 0; int32_t y0 = 0; int32_t x1 = 0;  int32_t y1 = 0;  int32_t x2 = 0;     int32_t y2 = 0;                                          int color = 0; };
  struct WideLineArgs   { int32_t x0 = 0; int32_t y0 = 0; int32_t x1 = 0;  int32_t y1 = 0;  int32_t r = 0;                                                               int color = 0; };
  
  struct RectArgs       { int32_t x = 0;  int32_t y = 0;  int32_t w = 0;   int32_t h = 0;                                                                                int color = 0; };
  struct RoundRectArgs  { int32_t x = 0;  int32_t y = 0;  int32_t w = 0;   int32_t h = 0;   int32_t r = 0;                                                               int color = 0; };

  struct CircleArgs     { int32_t x = 0;  int32_t y = 0;                                    int32_t r = 0;                                                               int color = 0; };
  struct EllipseArgs    { int32_t x = 0;  int32_t y = 0;                                    int32_t rx = 0;     int32_t ry = 0;                                          int color = 0; };
  struct TriangleArgs   { int32_t x0 = 0; int32_t y0 = 0; int32_t x1 = 0;  int32_t y1 = 0;  int32_t x2 = 0;     int32_t y2 = 0;                                          int color = 0; };
  
  struct ArcArgs        { int32_t x = 0;  int32_t y = 0;  int32_t r0 = 0;  int32_t r1 = 0;  int32_t angle0 = 0; int32_t angle1 = 0;                                      int color = 0; };
  struct EllipseArcArgs { int32_t x = 0;  int32_t y = 0;  int32_t r0x = 0; int32_t r1x = 0; int32_t r0y = 0;    int32_t r1y = 0; int32_t angle0 = 0; int32_t angle1 = 0; int color = 0; };
  
  struct JpgFileArgs    {
    DataType dataSource = DataType::SD;
    const char *path = nullptr;
    int32_t x = 0;
    int32_t y = 0;
    int32_t w = 0;
    int32_t h = 0;
    int32_t maxWidth = 0;
    int32_t maxHeight = 0;
    int32_t offX = 0;
    int32_t offY = 0;
    float scaleX = 0;
    float scaleY = 0;
  };

  struct PngFileArgs    {
    DataType dataSource = DataType::SD;
    const char *path = nullptr;
    int32_t x = 0;
    int32_t y = 0;
    int32_t w = 0;
    int32_t h = 0;
    int32_t maxWidth = 0;
    int32_t maxHeight = 0;
    int32_t offX = 0;
    int32_t offY = 0;
    float scaleX = 0;
    float scaleY = 0;
  };

  struct BitmapArgs {
    const uint16_t* data = nullptr;  // PROGMEMなどに置いた画像データ
    int32_t x = 0;
    int32_t y = 0;
    int32_t w = 0;
    int32_t h = 0;
  };

  struct StringArgs { 
    int32_t x = 0;
    int32_t y = 0;
    const char* text = nullptr;
    int color = 0;
    int bgcolor = 0;
    const lgfx::IFont* font = nullptr;         // <- 修正
    textdatum_t datum = textdatum_t::top_left; // <- 修正
    int textSize = 1;
    bool textWrap = true;
  };

  union ObjectArgs{
    PixelArgs      pixel;
    LineArgs       line;
    BezierArgs     bezier;
    WideLineArgs   wideLine;

    RectArgs       rect;
    RoundRectArgs  roundRect;

    CircleArgs     circle;
    EllipseArgs    ellipse;
    TriangleArgs   triangle;

    ArcArgs arc;
    EllipseArcArgs ellipseArc;

    JpgFileArgs    jpg;
    PngFileArgs    png;
    BitmapArgs     bitmap;

    StringArgs     text;
    
    ObjectArgs() {}
  };

  struct ObjectData{
    int objectNum = -1;
    String objectName = "";
    DrawType type = DrawType::DrawPixel;
    ObjectArgs objectArgs;
    uint8_t zIndex = 0;
    bool isUntouchable = false;

    bool isEmpty() const {
      return objectNum == -1; // ダミーデータは pageNum = -1 として判定
    }
  };

  struct PageData{
    int pageNum = -1;
    String pageName = "";
    std::vector<ObjectData> objects;

    bool isEmpty() const {
      return pageNum == -1; // ダミーデータは pageNum = -1 として判定
    }
  };

  std::vector<PageData> pages;
};