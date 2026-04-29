#ifndef CIRCLE_H
#define CIRCLE_H

#include <SDL2/SDL.h>
#include "color.h"

class Circle {
public:
    float x, y, r;
    color c;

    Circle(const float x, const float y, const float r, color c);
    void display() const;
};

#endif
