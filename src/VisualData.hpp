#pragma once

#include <Arduino.h>
#include <M5Unified.h>
#include <vector>

enum class DrawType{
  DrawPixel,      // ピクセル 描画
  DrawLine,       // 直線 描画
  DrawBezier,     // ベジェ曲線 描画
  DrawArc,        // 正円弧 描画
  DrawEllipseArc, // 楕円弧 描画
  DrawRect,       // 矩形 描画
  DrawRoundRect,  // 角丸矩形 描画
  DrawCircle,     // 円形 描画
  DrawEllipse,    // 楕円 描画
  DrawTriangle,   // 三角形 描画
  FillRect,       // 矩形 描画
  FillRoundRect,  // 角丸矩形 描画
  FillCircle,     // 円形 描画
  FillEllipse,    // 楕円 描画
  FillTriangle,   // 三角形 描画
  FloodFill,      // バケツ塗りつぶし
  
  drawJpg,
  drawPng,

  drawString
};

enum 

class VisualData{

};