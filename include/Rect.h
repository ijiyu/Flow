#ifndef RECT_H
#define RECT_H

#include "color.h"
#include <SDL2/SDL.h>

class Rect {
public:
    int x, y, w, h;
    color c{};

    Rect(const int x, const int y, const int w, const int h, const color c);
    Rect(const int x, const int y, const int w, const int h);
    void display() const;
};

#endif
