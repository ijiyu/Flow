#ifndef COLOR_H
#define COLOR_H

struct color {
   int r, g, b, a;

   color() : r(255), g(255), b(255), a(255) {}

   color(int r, int g, int b, int a)
       : r(r), g(g), b(b), a(a) {}
};

#endif
