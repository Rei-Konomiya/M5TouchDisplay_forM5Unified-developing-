#ifndef VISUALTOUCH_H
#define VISUALTOUCH_H

#include "VisualData.hpp"
#include "TouchData.hpp"

class VisualTouch {
public:
    VisualData vData;
    TouchData tData;

    VisualTouch(LovyanGFX* lcd, bool enableErrorLog, bool enableInfoLog, bool enableSuccessLog)
        : vData(lcd, enableErrorLog, enableInfoLog, enableSuccessLog),
            tData(&vData, enableErrorLog, enableInfoLog, enableSuccessLog)
    {}
};

#endif
