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
#include "Edge.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 800;
constexpr int FPS = 60;
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
const Vector2<int> BOARD_START{100, 100};
const Vector2<int> BOARD_END{BOARD_START.x + BOARD_COLUMNS * SPACING, BOARD_START.y + BOARD_ROWS * SPACING};

std::vector<std::string> colors = {"R", "G", "B", "Y", "O"};

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

std::map<std::string, color> darkColorMap = {
    {"R", color(100, 0, 0, 100)},
    {"Y", color(100, 100, 0, 100)},
    {"G", color(0, 100, 0, 100)},
    {"B", color(0, 0, 100, 100)},
    {"O", color(100, 50, 0, 100)}
};

Vector2<int> gridToWorldSpace(Vector2<int> vec) {
    return {BOARD_START.x + vec.x * SPACING + SPACING/2, BOARD_START.y + vec.y * SPACING + SPACING/2};
}

Vector2<int> worldToGridSpace(Vector2<int> vec) {
    return {(vec.x - BOARD_START.x - SPACING/2) / SPACING, (vec.y - BOARD_START.y - SPACING/2) / SPACING};
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

std::vector<Edge> edges;
std::unordered_map<std::string, std::unique_ptr<Edge>> medges;

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

Vector2<int> getCurrentPos() {
    Vector2<int> currentPos = Vector2<int>((mousePos.x - BOARD_START.x) / SPACING, (mousePos.y - BOARD_START.y) / SPACING);
    currentPos = gridToWorldSpace(currentPos);
    return currentPos;
}


void start() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    if (!window || !renderer) {
        std::cerr << "SDL Error: " << SDL_GetError() << "\n";
        running = false;
    }
    for (std::string& col : colors) {
        auto& edge = medges[col];
        edge = std::make_unique<Edge>();

        edge->lineColor = colorMap[col];
        edge->backgroundColor = darkColorMap[col];
    }
}

std::string currentColor = "B";

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
                Vector2<int> gridPosition = worldToGridSpace(getCurrentPos());
                std::string boardColor = board[gridPosition.y][gridPosition.x];
                if (boardColor != " ")
                    currentColor = boardColor;
                if (mousePos.x >= BOARD_START.x && mousePos.y >= BOARD_START.y && mousePos.x <= BOARD_END.x && mousePos.y <= BOARD_END.y) {
                    medges[currentColor]->initialize(getCurrentPos());
                }
            }
        }
        else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                mouseHeldDown = false;
                medges[currentColor]->addEnd(getCurrentPos());
            }
        }
    }
    SDL_GetMouseState(&mousePos.x, &mousePos.y);

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);
}

void update() {
    handleInput();
    if (mouseHeldDown)
        medges[currentColor]->updatePositions(getCurrentPos());
    for (auto& [key, edge] : medges)
        edge->blockDisplay();
    drawGrid();
    drawBoard();
    for (auto& [key, edge] : medges) {
        edge->lineDisplay();
    }
    if (mouseHeldDown) {
        medges[currentColor]->ghostLineDisplay();
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
    Uint32 fpsTimer = SDL_GetTicks();
    int frameCount = 0;

    while (running) {
        Uint32 startFrame = SDL_GetTicks();

        update();
        frameCount++;

        Uint32 frameTime = SDL_GetTicks() - startFrame;
        if (frameDelay > frameTime)
            SDL_Delay(frameDelay - frameTime);

        if (SDL_GetTicks() - fpsTimer >= 1000) {
            std::cout << "FPS: " << frameCount << std::endl;
            frameCount = 0;
            fpsTimer = SDL_GetTicks();
        }
}
    return 0;
}
