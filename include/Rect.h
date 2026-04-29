#ifndef RECT_H
#define RECT_H

#include "color.h"
#include <SDL2/SDL.h>

class Rect {
public:
    float x, y, w, h;
    color c{};

    Rect(const float x, const float y, const float w, const float h, const color c);
    Rect(const float x, const float y, const float w, const float h);
    void display() const;
};

#endif
