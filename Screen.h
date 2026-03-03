#pragma once

#include "point.h"
#include "utils.h"
#include <cstring>

class point;
class Screen {
public:
  static constexpr int MAX_X = 80;  // Maximum columns of the screen
  static constexpr int MAX_Y = 24;  // Maximum rows of the screen
  char board[MAX_Y + 1][MAX_X + 1]; // 2D array representing the room layout

private:
  bool dark[MAX_Y + 1][MAX_X + 1]{}; // which cells belong to dark area
  bool p1HasTorch = false;
  bool p2HasTorch = false;
  int p1x = 0, p1y = 0;
  int p2x = 0, p2y = 0;
  int hudX = 0, hudY = 0;
  bool hudActive = false;

public:
  Screen() {
    for (int y = 0; y <= MAX_Y; ++y)
      for (int x = 0; x <= MAX_X; ++x) {
        board[y][x] = ' ';
        dark[y][x] = false;
      }
  }
  void copyFrom(const Screen &other);

  char charAt(const point &p) const {
    return board[p.getY()][p.getX()]; // Get character at point position
  }

  bool isWall(const point &p) const {
    return charAt(p) == 'W'; // Check if given point is a wall
  }
  bool isobstacle(const point &p) { return (charAt(p) == '*'); }

  bool isInteractable(const point &p) const {
    return (
        charAt(p) >= '1' && charAt(p) <= '9' // Check if point is any object:
        || charAt(p) == '@'                  // numbers, key, riddles, etc.
        || charAt(p) == 'k' || charAt(p) == '?' || charAt(p) == '/' ||
        charAt(p) == '\\' || charAt(p) == '+' || charAt(p) == '-' || // Switches
        charAt(p) == '!' || charAt(p) == '#' || charAt(p) == '*');
  }

  void explodeAt(int x, int y, int HUDY); // Handle explosion centered at (x,y)
  void draw();                            // Draw entire board to the console
  void
  drawTorchArea(int oldP1X, int oldP1Y, int oldP2X, int oldP2Y,
                bool oldP1HadTorch,
                bool oldP2HadTorch); // Redraw only cells near torch positions

  void setChar(int y, int x, char c) {
    board[y][x] = c; // Set specific cell in the board
  }

  void clearChar(int y, int x) {
    board[y][x] = ' '; // Clear specific cell (set to space)
  }

  bool enableColor = false; // Global flag: draw with colors or not

  void drawChar(int x, int y, char ch,
                Color color); // Draw single char at (x,y) with color

  void setDark(int x, int y, bool value) { dark[y][x] = value; }
  bool isDark(int x, int y) const { return dark[y][x]; }

  void setTorchInfo(bool has1, bool has2, int x1, int y1, int x2, int y2) {
    p1HasTorch = has1;
    p2HasTorch = has2;
    p1x = x1;
    p1y = y1;
    p2x = x2;
    p2y = y2;
  }
  // perplexity AI
  static int manhattanDist(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
  }
  bool isCellLitByTorch(int x, int y) const;
  void markDarkRect(int startX, int startY, int width, int height);

  // HUD management
  int getHudX() const { return hudX; }
  int getHudY() const { return hudY; }
  bool isHudActive() const { return hudActive; }
  void setHudInfo(int x, int y, bool active) {
    hudX = x;
    hudY = y;
    hudActive = active;
  }
};

// perplexity AI.
Screen &getScreen(); // Return reference to the shared Screen instance used by
                     // the whole game
