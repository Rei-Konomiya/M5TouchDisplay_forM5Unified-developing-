
#include "VisualData.hpp"

VisualData::VisualData(int docSize){
  visualData = new DynamicJsonDocument(docSize);
}

DynamicJsonDocument VisualData::getVisualData(){
  return *visualData;
};

void VisualData::checkVisualData(){
  serializeJsonPretty(getVisualData(), Serial);
};

void VisualData::addPage(String pageName){
  JsonObject Page = (*visualData)[pageName].to<JsonObject>();
  editingPage = Page;
  objectNum = 0;
};

void VisualData::setDrawPixelObject(String objectName, int32_t x, int32_t y, int color){
  objectNum++;
  JsonObject obj;
  if(objectName == ""){
    objectName = "obj-" + String(objectNum);
  }
  obj = editingPage[objectName].to<JsonObject>();
  obj["type"] = static_cast<int>(DrawType::DrawPixel);
  JsonArray args = obj["args"].to<JsonArray>();
  args.add(x);
  args.add(y);
  args.add(color);
};