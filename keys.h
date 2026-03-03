#pragma once
class keys
{
    int x, y;                      // Key position on the board
public:
    keys(int x, int y) : x(x), y(y) {} // Constructor: create key at (x,y)
    int getX() const { return x; }     // Return key x-coordinate
    int getY() const { return y; }     // Return key y-coordinate
};

