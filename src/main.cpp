#include <SDL2/SDL.h>
#include <cmath>
#include <algorithm>
#include <ostream>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_set>

#include "color.h"
#include "Vector2.h"
#include "Line.h"
#include "Rect.h"
#include "Circle.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 800;
constexpr int FPS = 120;
constexpr int frameDelay = 1000 / FPS;
constexpr float GRAVITY = 1500.0f;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

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


Vector2<int> gridToWorldSpace(Vector2<int> vec) {
    return {BOARD_START.x + vec.x * SPACING + SPACING/2, BOARD_START.y + vec.y * SPACING + SPACING/2};
}

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
    std::vector<Vector2<int>> path, lines;
    std::unordered_set<Vector2<int>> pathSet, linesSet;

    void lineDisplay() {
        if (lines.size() == 0) return;
        for (int i = 0; i < lines.size()-1; i++) {
            Line l{lines[i].x, lines[i].y, lines[i+1].x, lines[i+1].y, color{255, 255, 255, 255}};
            l.display();
        }
    }

    void blockDisplay() {
        if (path.size() == 0)
            return;
        std::cout << "A: ";
        for (int i = 0; i < path.size(); i++) {
            Rect r{path[i].x-SPACING/2, path[i].y-SPACING/2, SPACING, SPACING, color{100, 0, 0, 10}};
            std::cout << "(" << path[i].x << ", " << path[i].y << ") ";
            r.display();
        }
        std::cout << std::endl;
    }

    void ghostLineDisplay() {
        if (lines.empty())
            return;
        Vector2<int> lastDot = lines.back();
        Vector2<int> diff{mousePos.x - lastDot.x, mousePos.y - lastDot.y};
        if (std::abs(diff.y) > std::abs(diff.x)) {
            Line l(lastDot.x, lastDot.y, lastDot.x, mousePos.y, color(255,255,255,255));
            l.display();
        } else {
            Line l(lastDot.x, lastDot.y, mousePos.x, lastDot.y, color(255,255,255,255));
            l.display();
        }

    }

    void removeDuplicates(std::vector<Vector2<int>> &vec, std::unordered_set &uset, Vector2<int> currentPos) {
        if (uset.contains(currentPos)) {
            while (vec.back() != currentPos && !vec.empty()) {
                Vector2<int> popped = vec.back();
                vec.pop_back();
                uset.erase(popped);
            }
        }
    }

    void updatePositions() {
        if (lines.empty())
            return;

        Vector2<int> currentPos {(mousePos.x - BOARD_START.x) / SPACING, (mousePos.y - BOARD_START.y) / SPACING};
        Vector2<int> lastDot = lines.back();
        Vector2<int> lastBlock = path.back();
        currentPos = gridToWorldSpace(currentPos);

        removeDuplicates(lines, linesSet, currentPos);
        removeDuplicates(path, pathSet, currentPos);

        if (currentPos != lastBlock) {
            path.push_back(currentPos);
            pathSet.insert(currentPos);
            Vector2<int> candidate = path[path.size()-2];
            if (lines.empty() || lines.back() != candidate) {
                lines.push_back(currentPos);
                linesSet.insert(currentPos);
            }
        }
        if (lastDot.x == currentPos.x || lastDot.y == currentPos.y) {
            bool down = currentPos.y > lastDot.y && mousePos.y >= currentPos.y);
            bool up = currentPos.y < lastDot.y && mousePos.y <= currentPos.y);
            bool left = currentPos.x < lastDot.x && mousePos.x <= currentPos.x);
            bool right = currentPos.x > lastDot.x && mousePos.x >= currentPos.x);
            bool sided = up || down || left || right;
            if (currentPos != lastDot && sided) {
                lines.push_back(currentPos);
                linesSet.insert(currentPos);
            }
        }
    }

    void initialize(Vector2<int> currentPos) {
        path = {currentPos};
        lines = {currentPos};
        pathSet.insert(currentPos);
        linesSet.insert(currentPos);
    }
};

Edge ee;

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

void handleInput() {
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
                    Vector2<int> currentPos = Vector2<int>((mousePos.x - BOARD_START.x) / SPACING, (mousePos.y - BOARD_START.y) / SPACING);
                    currentPos = gridToWorldSpace(currentPos);
                    ee.initialize(currentPos);
                }
            }
        }
        else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                mouseHeldDown = false;
            }
        }
    }
    SDL_GetMouseState(&mousePos.x, &mousePos.y);

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);
}

void update() {
    handleInput();

    if (mouseHeldDown) {
        ee.updatePositions();
    }


    ee.blockDisplay();
    drawGrid();
    drawBoard();
    ee.lineDisplay();
    if (mouseHeldDown) {
        ee.ghostLineDisplay();
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
