#include <SDL2/SDL.h>
#include <cmath>
#include <algorithm>
#include <ostream>
#include <iostream>
#include <vector>
#include <map>

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 800;
constexpr int FPS = 120;
constexpr int frameDelay = 1000 / FPS;
constexpr float GRAVITY = 1500.0f;

struct color {
   int r, g, b, a;

   color() : r(0), g(0), b(0), a(255) {}

   color(int r, int g, int b, int a)
       : r(r), g(g), b(b), a(a) {}
};

template<typename T>
struct Vector2 {
    T x, y;

    Vector2() : x(0), y(0) { }

    Vector2(T x, T y) : x(x), y(y) { }

    template<typename U>
    explicit Vector2(const Vector2<U> &other)
        : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) { }

    Vector2 operator+(const Vector2 &o) const { return {x + o.x, y + o.y}; }
    Vector2 operator-(const Vector2 &o) const { return {x - o.x, y - o.y}; }
    Vector2 operator*(T s) const { return {x * s, y * s}; }

    [[nodiscard]] T dot(const Vector2 &o) const { return x * o.x + y * o.y; }
    [[nodiscard]] T magnitude() const { return std::sqrt(x * x + y * y); }

    [[nodiscard]] Vector2 normalized() const {
        T m = magnitude();
        return (m > 0) ? Vector2(x / m, y / m) : Vector2();
    }

    bool operator==(const Vector2 &o) const {
        return x == o.x && y == o.y;
    }

    bool operator!=(const Vector2 &o) const {
        return !(*this == o);
    }
};

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

class Line {
public:
    float x1, y1, x2, y2;
    color c{};

    Line(const float x1, const float y1, const float x2, const float y2, const color c)
        : x1(x1), y1(y1), x2(x2), y2(y2), c(c) {
    }

    Line(const float x1, const float y1, const float x2, const float y2)
        : x1(x1), y1(y1), x2(x2), y2(y2) {
        c = color(0, 0, 0, 255);
    }

    void display() const {
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_RenderDrawLine(renderer, static_cast<int>(x1), static_cast<int>(y1),
                           static_cast<int>(x2), static_cast<int>(y2));
    }
};

class Rect {
public:
    float x, y, w, h;
    color c{};

    Rect(const float x, const float y, const float w, const float h, const color c)
        : x(x), y(y), w(w), h(h), c(c) {
    }

    Rect(const float x, const float y, const float w, const float h)
        : x(x), y(y), w(w), h(h) {
        c = color(0, 0, 0, 255);
    }

    void display() const {
        const SDL_Rect r = {
            static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h)
        };
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_RenderFillRect(renderer, &r);
    }
};

class Circle {
public:
    float x, y, r;
    color c;

    Circle(const float x, const float y, const float r, color c)
        : x(x), y(y), r(r), c(c) {
    }

    void display() const {
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        const int radius = static_cast<int>(r);
        for (int dy = -radius; dy <= radius; dy++)
            for (int dx = -radius; dx <= radius; dx++)
                if (dx * dx + dy * dy <= radius * radius)
                    SDL_RenderDrawPoint(renderer, static_cast<int>(x) + dx,
                                        static_cast<int>(y) + dy);
    }
};

class Dot {
public:
    float x, y, r;
    color c;
    Circle circle;
    Dot(const float x, const float y, const float r, const color c)
        : x(x), y(y), r(r), c(c), circle(x, y, r, c) {
    }
    void display() const {
        circle.display();
    }
};

class Edge {
public:
    std::vector<Vector2<int>> path;
    void display() const {
        if (path.size() < 2) return;
        for (int node = 0; node < path.size() - 1; node++) {
            color c(255, 255, 255, 255);
            Vector2<int> start = path[node];
            Vector2<int> end = path[node+1];
            Line l(start.x, start.y, end.x, end.y, c);
            l.display();
        }
    }
};

bool running = true;
SDL_Event event;

Vector2<int> mousePos, clickPos, releasePos;

bool mouseHeldDown = false;

const int BOARD_COLUMNS = 5;
const int BOARD_ROWS = 5;
const int SPACING = 100;
const int BOARD_WIDTH = BOARD_COLUMNS * SPACING;
const int BOARD_HEIGHT = BOARD_ROWS * SPACING;
const int DOT_RADIUS = 40;
Vector2 BOARD_START{100, 100};

std::vector<std::vector<std::string>> board = 
    {{"R", " ", "G", " ", "Y"},
    {" ", " ", "B", " ", "O"},
    {" ", " ", " ", " ", " "},
    {" ", "G", " ", "Y", " "},
    {" ", "R", "B", "O", " "}
};
std::map<std::string, color> colorMap = {
    {"R", color(255, 0, 0, 255)},
    {"Y", color(255, 255, 0, 255)},
    {"G", color(0, 255, 0, 255)},
    {"B", color(0, 0, 255, 255)},
    {"O", color(255, 100, 0, 255)}
};


Vector2<int> currentDot;
Edge ee;
std::string test = "";

Vector2<int> gridToWorldSpace(Vector2<int> vec) {
    return {BOARD_START.x + vec.x * SPACING + SPACING/2, BOARD_START.y + vec.y * SPACING + SPACING/2};
}

void drawGrid() {
    constexpr int spacing = 100;
    for (int x = SCREEN_WIDTH * -10; x < SCREEN_WIDTH * 10; x += spacing) {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderDrawLine(renderer, x, 0, x, SCREEN_HEIGHT);
    }
    for (int y = SCREEN_HEIGHT * -10; y < SCREEN_HEIGHT * 10; y += spacing) {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }
}

void drawBoard() {
    for (int col = 0; col <= BOARD_COLUMNS; col++) {
        int x = BOARD_START.x + col * SPACING;
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderDrawLine(renderer, x, BOARD_START.y, x, BOARD_WIDTH + BOARD_START.y);
    }
    for (int row = 0; row <= BOARD_ROWS; row++) {
        int y = BOARD_START.y + row * SPACING;
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderDrawLine(renderer, BOARD_START.x, y, BOARD_HEIGHT + BOARD_START.x, y);
    }
    for (int row = 0; row < BOARD_ROWS; row++) {
        for (int col = 0; col < BOARD_COLUMNS; col++) {
            std::string colorChar = board[row][col];
            if (colorMap.find(colorChar) == colorMap.end())
                continue;
            color dotColor = colorMap[colorChar];
            Circle c(col* SPACING + BOARD_START.x + SPACING/2, row * SPACING + BOARD_START.y + SPACING/2, DOT_RADIUS, dotColor);
            c.display();
        }
    }
}

void start() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    if (!window || !renderer) {
        std::cerr << "SDL Error: " << SDL_GetError() << "\n";
        running = false;
    }
}

void update() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                mouseHeldDown = true;
                if (mousePos.x < BOARD_START.x || mousePos.y < BOARD_START.y || mousePos.x > BOARD_START.x + BOARD_COLUMNS * SPACING || mousePos.y > BOARD_START.y + BOARD_ROWS * SPACING)
                    std::cout << "out of bounds\n" << std::flush;
                else {
                    currentDot = Vector2<int>((mousePos.x - BOARD_START.x) / SPACING, (mousePos.y - BOARD_START.y) / SPACING);
                    ee.path = {};
                    ee.path.push_back(gridToWorldSpace(currentDot));
                }
            }
        }
        else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                mouseHeldDown = false;
            }
        }
    }

    if (mouseHeldDown) {
        Vector2<int> pos;
        currentDot = Vector2<int>((mousePos.x - BOARD_START.x) / SPACING, (mousePos.y - BOARD_START.y) / SPACING);
        pos = currentDot;
        currentDot = gridToWorldSpace(currentDot);
        Vector2<int> lastDot = *ee.path.rbegin();
        if (lastDot.x == currentDot.x || lastDot.y == currentDot.y) {
            if (std::ranges::find(ee.path, currentDot) != ee.path.end()) {
                while (*ee.path.rbegin() != currentDot) {
                    ee.path.pop_back();
                    if (ee.path.empty())
                        break;
                }
            }
            if (currentDot != lastDot && ((currentDot.y > lastDot.y && mousePos.y >= currentDot.y) || 
                        (currentDot.y < lastDot.y && mousePos.y <= currentDot.y) ||
                        (currentDot.x > lastDot.x && mousePos.x >= currentDot.x) ||
                        (currentDot.x < lastDot.x && mousePos.x <= currentDot.x))) {
                ee.path.push_back(currentDot);
            }
        }
        int dx = mousePos.x - lastDot.x;
        int dy = mousePos.y - lastDot.y;

        const int DEADZONE = 10;

        Vector2<int> check;
        if (std::abs(dy) > std::abs(dx)) {
            if (dy > DEADZONE) {
                test = "down";
                check = {check.x, check.y-1};
                if (std::ranges::find(ee.path, check) != ee.path.end()) {
                    ee.path.push_back(check);
                }
            }
            else if (dy < -DEADZONE) {
                test = "up";
                check = {check.x, check.y+1};
                if (std::ranges::find(ee.path, check) != ee.path.end()) {
                    ee.path.push_back(check);
                }
            }
            else {
                test = "";
            }
        } else {
            if (dx > DEADZONE) {
                test = "right";
                check = {check.x-1, check.y};
                if (std::ranges::find(ee.path, check) != ee.path.end()) {
                    ee.path.push_back(check);
                }
            }
            else if (dx < -DEADZONE) {
                test = "left";
                check = {check.x+1, check.y};
                if (std::ranges::find(ee.path, check) != ee.path.end()) {
                    ee.path.push_back(check);
                }
            }
            else {
                test = "";
            }
        }
    }

    SDL_GetMouseState(&mousePos.x, &mousePos.y);

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);

    drawGrid();
    drawBoard();
    ee.display();
    if (mouseHeldDown && !ee.path.empty()) {
        Vector2<int> lastDot = *ee.path.rbegin();
        int dx = mousePos.x - lastDot.x;
        int dy = mousePos.y - lastDot.y;

        if (std::abs(dy) > std::abs(dx)) {
            Line l(lastDot.x, lastDot.y,
                    lastDot.x, mousePos.y,
                    color(255,255,255,255));
            l.display();
        } else {
            Line l(lastDot.x, lastDot.y,
                    mousePos.x, lastDot.y,
                    color(255,255,255,255));
            l.display();
        }
    }
    SDL_RenderPresent(renderer);
}


void end() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    start();
    while (running) {
        const Uint32 startFrame = SDL_GetTicks();
        update();
        if (const Uint32 frameTime = SDL_GetTicks() - startFrame; frameDelay > frameTime)
            SDL_Delay(frameDelay - frameTime);
    }
    end();
    return 0;
}
