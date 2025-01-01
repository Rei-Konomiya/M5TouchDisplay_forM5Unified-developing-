#pragma once

#include <Arduino.h>
#include <M5Unified.h>
#include <vector>

enum class DrawType{
  DrawPixel,
  DrawFastVLine,
  DrawFastHLine,
  DrawRect,
  DrawRoundRect,
  DrawCircle,
  DrawEllipse,
  DrawLine,
  DrawTriangle,
  drawBezier,
  drawEllipseArc,
  drawArc,
  
};

enum class FillType{
  No_fill,
  Fill
}

class VisualData : lgfx::LGFXBase{


  
  void drawPixel(int32_t x, int32_t y){
    lgfx::LGFXBase::drawPixel(x, y);
    lgfx::LGFXBase::drawString();
    
  }
};