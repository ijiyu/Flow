#ifndef LINE_H
#define LINE_H

#include "color.h"
#include <SDL2/SDL.h>

class Line {
public:
    int x1, y1, x2, y2;
    color c{};

    Line(const int x1, const int y1, const int x2, const int y2, const color c);
    Line(const int x1, const int y1, const int x2, const int y2);
    void display() const;
};

#endif
