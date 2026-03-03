#pragma once
class Spring
{
    int x, y;       // one spring cell position
public:
    Spring(int x, int y) : x(x), y(y) {}

    int getX() const { return x; }
    int getY() const { return y; }
};

