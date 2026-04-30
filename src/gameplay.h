#pragma once
#include "raylib.h"
#include "AudioSystem.h"
#include <string>


struct TargetNote {
    std::string type;
    std::string name;
    std::vector<std::string> notes;
};
extern TargetNote targetNote;

class Tile {
private:
    Vector2 position;
    Color color;
    bool isMatched;

public:
    Tile(float startX, float startY, Color color);
    void update();
};