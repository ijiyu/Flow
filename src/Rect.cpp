#include "Rect.h"

extern SDL_Renderer *renderer;

Rect::Rect(const float x, const float y, const float w, const float h, const color c)
    : x(x), y(y), w(w), h(h), c(c) { }

Rect::Rect(const float x, const float y, const float w, const float h)
    : x(x), y(y), w(w), h(h) {
        c = color(0, 0, 0, 255);
}

void Rect::display() const {
    const SDL_Rect r = {
        static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h)
    };
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(renderer, &r);
}
