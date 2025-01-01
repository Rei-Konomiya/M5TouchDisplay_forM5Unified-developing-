#include "VisualData_Class.hpp"

void VisualManager::createPage(int pageNum);

void changeEditPage(int pageNum);
void changeEditObject(String objName);

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