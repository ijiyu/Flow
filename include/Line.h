#ifndef LINE_H
#define LINE_H

#include "color.h"
#include <SDL2/SDL.h>

class Line {
public:
    float x1, y1, x2, y2;
    color c{};

    Line(const float x1, const float y1, const float x2, const float y2, const color c);
    Line(const float x1, const float y1, const float x2, const float y2);
    void display() const;
};

#endif
