#include "Line.h"

extern SDL_Renderer *renderer;

Line::Line(const int x1, const int y1, const int x2, const int y2, const color c)
    : x1(x1), y1(y1), x2(x2), y2(y2), c(c) { }

Line::Line(const int x1, const int y1, const int x2, const int y2)
    : x1(x1), y1(y1), x2(x2), y2(y2) {
        c = color(0, 0, 0, 255);
}

void Line::display() const {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2); 
}
