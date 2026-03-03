#pragma once
class Torch
{
    int x, y;        // Torch position on the board

public:
    Torch(int x, int y) : x(x), y(y) {}

    int getX() const { return x; }   // Get X position
    int getY() const { return y; }   // Get Y position

};

