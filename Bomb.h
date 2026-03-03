#pragma once
class Bomb
{
    int x, y;                      // Bomb position on the board (x = column, y = row)
public:
    Bomb(int x, int y) : x(x), y(y) {}   // Constructor: create bomb at position (x, y)
    int getX() const { return x; }       // Return bomb x-coordinate
    int getY() const { return y; }       // Return bomb y-coordinate
};

