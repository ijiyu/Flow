#include "Rect.h"

extern SDL_Renderer *renderer;

Rect::Rect(const int x, const int y, const int w, const int h, const color c)
    : x(x), y(y), w(w), h(h), c(c) { }

Rect::Rect(const int x, const int y, const int w, const int h)
    : x(x), y(y), w(w), h(h) {
        c = color(0, 0, 0, 255);
}

void Rect::display() const {
    const SDL_Rect r = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(renderer, &r);
}
