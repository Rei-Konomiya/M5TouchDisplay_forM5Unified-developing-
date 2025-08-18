#include <Arduino.h>
#include <M5GFX.h>

class VisualDataSet{
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

  enum class DataType{
    SD,
    SPIFFS
  };



  struct PixelArgs      { int32_t x;  int32_t y;                                                  int color; };
  struct LineArgs       { int32_t x0; int32_t y0; int32_t x1; int32_t y1;                         int color; };
  struct BezierArgs     { int32_t x0; int32_t y0; int32_t x1; int32_t y1; int32_t x2; int32_t y2; int color; };
  struct WideLineArgs   { int32_t x0; int32_t y0; int32_t x1; int32_t y1; int32_t r;              int color; };
  
  struct RectArgs       { int32_t x;  int32_t y;  int32_t w;  int32_t h;                          int color; };
  struct RoundRectArgs  { int32_t x;  int32_t y;  int32_t w;  int32_t h;  int32_t r;              int color; };

  struct CircleArgs     { int32_t x;  int32_t y;                          int32_t r;              int color; };
  struct EllipseArgs    { int32_t x;  int32_t y;                          int32_t rx; int32_t ry; int color; };
  struct TriangleArgs   { int32_t x0; int32_t y0; int32_t x1; int32_t y1; int32_t x2; int32_t y2; int color; };
  
  struct JpgFileArgs    { DataType dataSource; const char *path; int32_t x; int32_t y; int32_t maxWidth; int32_t maxHeight; int32_t offX; int32_t offY; lgfx::v1::jpeg_div::jpeg_div_t scale; uint8_t zindex; };
  struct PngFileArgs    { DataType dataSource; const char *path; int32_t x; int32_t y; int32_t maxWidth; int32_t maxHeight; int32_t offX; int32_t offY; float scaleX; float scaleY;           uint8_t zindex; };

  struct StringArgs     { int32_t x; int32_t y; const char* text; int color; int bgcolor; };

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

    JpgFileArgs    jpg;
    PngFileArgs    png;

    StringArgs     text;
    
    ObjectArgs() {}
    ~ObjectArgs() {}
  };

  struct ObjectData{
    int objectNumber;
    String objectName;
    DrawType type;
    ObjectArgs objectArgs;
    uint8_t zindex;
  };
};