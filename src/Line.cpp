#include "Line.h"

extern SDL_Renderer *renderer;

Line::Line(const float x1, const float y1, const float x2, const float y2, const color c)
    : x1(x1), y1(y1), x2(x2), y2(y2), c(c) { }

Line::Line(const float x1, const float y1, const float x2, const float y2)
    : x1(x1), y1(y1), x2(x2), y2(y2) {
        c = color(0, 0, 0, 255);
}

void Line::display() const {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawLine(renderer, static_cast<int>(x1), static_cast<int>(y1), static_cast<int>(x2), static_cast<int>(y2));
}
