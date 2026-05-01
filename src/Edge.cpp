#include "Edge.h"

void Edge::lineDisplay() {
    if (lines.size() == 0) return;
    for (int i = 0; i < lines.size()-1; i++) {
        Line l{lines[i].x, lines[i].y, lines[i+1].x, lines[i+1].y, lineColor};
        l.display();
    }
}

void Edge::blockDisplay() {
    if (path.size() == 0)
        return;
    for (int i = 0; i < path.size(); i++) {
        Rect r{path[i].x-SPACING/2, path[i].y-SPACING/2, SPACING, SPACING, backgroundColor};
        r.display();
    }
}

void Edge::ghostLineDisplay() {
    if (lines.empty())
        return;
    Vector2<int> lastDot = lines.back();
    Vector2<int> diff{mousePos.x - lastDot.x, mousePos.y - lastDot.y};
    if (std::abs(diff.y) > std::abs(diff.x)) {
        Line l(lastDot.x, lastDot.y, lastDot.x, mousePos.y, lineColor);
        l.display();
    } else {
        Line l(lastDot.x, lastDot.y, mousePos.x, lastDot.y, lineColor);
        l.display();
    }

}

void Edge::removeDuplicates(std::vector<Vector2<int>> &vec, std::unordered_set<Vector2<int>> &uset, Vector2<int> currentPos) {
    if (uset.contains(currentPos)) {
        while (vec.back() != currentPos && !vec.empty()) {
            Vector2<int> popped = vec.back();
            vec.pop_back();
            uset.erase(popped);
        }
    }
}

void Edge::updatePositions(Vector2<int> currentPos) {
    if (lines.empty())
        return;

    Vector2<int> lastDot = lines.back();
    Vector2<int> lastBlock = path.back();

    removeDuplicates(lines, linesSet, currentPos);
    removeDuplicates(path, pathSet, currentPos);

    if (currentPos != lastBlock) {
        path.push_back(currentPos);
        pathSet.insert(currentPos);
        Vector2<int> candidate = path[path.size()-2];
        if (lines.empty() || lines.back() != candidate) {
            lines.push_back(candidate);
            linesSet.insert(candidate);
        }
    }
    if (lastDot.x == currentPos.x || lastDot.y == currentPos.y) {
        bool down = currentPos.y > lastDot.y && mousePos.y >= currentPos.y;
        bool up = currentPos.y < lastDot.y && mousePos.y <= currentPos.y;
        bool left = currentPos.x < lastDot.x && mousePos.x <= currentPos.x;
        bool right = currentPos.x > lastDot.x && mousePos.x >= currentPos.x;
        bool sided = up || down || left || right;
        if (currentPos != lastDot && sided) {
            lines.push_back(currentPos);
            linesSet.insert(currentPos);
        }
    }
}

void Edge::addEnd(Vector2<int> endPos) {
    if (lines.back() != endPos)
        lines.push_back(endPos);
}

void Edge::initialize(Vector2<int> currentPos) {
    path = {currentPos};
    lines = {currentPos};
    pathSet.clear();
    linesSet.clear();
    pathSet.insert(currentPos);
    linesSet.insert(currentPos);
}
