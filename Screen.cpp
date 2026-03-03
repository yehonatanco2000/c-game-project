#include "Screen.h"
#include "Menu.h"
#include <iostream>
#include <vector>
#include <windows.h>

void Screen::copyFrom(const Screen &other) {
  for (int y = 0; y <= MAX_Y; ++y)
    for (int x = 0; x <= MAX_X; ++x) {
      board[y][x] = other.board[y][x];
      dark[y][x] = other.dark[y][x];
    }
  hudX = other.hudX;
  hudY = other.hudY;
  hudActive = other.hudActive;
}

void Screen::draw() {
  HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(h, &csbi); // Save original attributes

  for (int y = 0; y <= MAX_Y; ++y) {
    gotoxy(0, y); // Move cursor to start of row - ONLY ONCE per row

    Color lastColor = Color::WHITE_BRIGHT;
    if (enableColor)
      SetConsoleTextAttribute(h, (WORD)lastColor);

    for (int x = 0; x <= MAX_X; ++x) {
      char ch = board[y][x];

      // Handle lighting
      if (!isCellLitByTorch(x, y)) {
        ch = ' ';
      }

      Color col = Color::WHITE_BRIGHT;

      // Determine color
      if (ch == 'W')
        col = Color::LIGHT_CYAN;
      else if (ch == '$' || ch == '&')
        col = Color::WHITE_BRIGHT;
      else if (ch == '@')
        col = Color::RED;
      else if (ch == 'k')
        col = Color::YELLOW;
      else if (ch == '?')
        col = Color::GREEN;
      else if (ch == '/' || ch == '\\')
        col = Color::CYAN;
      else if (ch >= '1' && ch <= '9' && y != getHudY() && y != getHudY() + 1)
        col = Color::MAGENTA;
      else if (ch == '!')
        col = Color::BLUE;
      else if (ch == '*')
        col = Color::LIGHT_GREEN;
      else if (ch == '#')
        col = Color::BROWN;

      // Change color only if needed
      if (enableColor && col != lastColor) {
        SetConsoleTextAttribute(h, (WORD)col);
        lastColor = col;
      }

      std::cout << ch;
    }
  }
  std::cout << std::flush; // Flush once at the end

  // Restore original attributes
  if (enableColor)
    SetConsoleTextAttribute(h, csbi.wAttributes);
}

// Redraw only cells near old and new torch positions (optimization).
// Instead of redrawing all 2,025 cells, this redraws only ~400 cells
// that could have changed visibility when a torch-carrying player moved.
void Screen::drawTorchArea(int oldP1X, int oldP1Y, int oldP2X, int oldP2Y,
                           bool oldP1HadTorch, bool oldP2HadTorch) {
  HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(h, &csbi);

  const int RADIUS = 14; // Slightly larger than TORCH_RADIUS (13) for margin

  for (int y = 0; y <= MAX_Y; ++y) {
    for (int x = 0; x <= MAX_X; ++x) {
      // Skip cells that are NOT in a dark area ג€” they never change
      if (!dark[y][x])
        continue;

      // Check if this cell is near any torch position (old or new)
      bool nearTorch = false;
      // Near CURRENT torch position (if player currently has torch)
      if (p1HasTorch && manhattanDist(x, y, p1x, p1y) <= RADIUS)
        nearTorch = true;
      if (p2HasTorch && manhattanDist(x, y, p2x, p2y) <= RADIUS)
        nearTorch = true;
      // Near OLD torch position (if player HAD torch — covers torch drop)
      if (oldP1HadTorch && manhattanDist(x, y, oldP1X, oldP1Y) <= RADIUS)
        nearTorch = true;
      if (oldP2HadTorch && manhattanDist(x, y, oldP2X, oldP2Y) <= RADIUS)
        nearTorch = true;

      if (!nearTorch)
        continue; // This cell didn't change ג€” skip it

      // Determine what to show: lit or dark
      char ch = board[y][x];
      if (!isCellLitByTorch(x, y)) {
        ch = ' '; // Cell is dark
      }

      // Determine color (same logic as draw())
      Color col = Color::WHITE_BRIGHT;
      if (ch == 'W')
        col = Color::LIGHT_CYAN;
      else if (ch == '$' || ch == '&')
        col = Color::WHITE_BRIGHT;
      else if (ch == '@')
        col = Color::RED;
      else if (ch == 'k')
        col = Color::YELLOW;
      else if (ch == '?')
        col = Color::GREEN;
      else if (ch == '/' || ch == '\\')
        col = Color::CYAN;
      else if (ch >= '1' && ch <= '9' && y != getHudY() && y != getHudY() + 1)
        col = Color::MAGENTA;
      else if (ch == '!')
        col = Color::BLUE;
      else if (ch == '*')
        col = Color::LIGHT_GREEN;
      else if (ch == '#')
        col = Color::BROWN;

      // Draw this single cell
      gotoxy(x, y);
      if (enableColor)
        SetConsoleTextAttribute(h, static_cast<WORD>(col));
      std::cout << ch;
    }
  }
  std::cout << std::flush;

  // Restore original attributes
  if (enableColor)
    SetConsoleTextAttribute(h, csbi.wAttributes);
}

void Screen::explodeAt(int x, int y, int HUDY) {
  // Remove the bomb character itself
  setChar(y, x, ' ');
  drawChar(x, y, ' ', Color::WHITE_BRIGHT);

  // Radius 3 explosion in all directions
  for (int dy = -3; dy <= 3; ++dy) {
    for (int dx = -3; dx <= 3; ++dx) {
      int nx = x + dx;
      int ny = y + dy;

      if (nx < 0 || nx > MAX_X || ny < 0 || ny > MAX_Y)
        continue;

      char ch = board[ny][nx];

      if (ch != ' ') {
        // Avoid exploding the HUD borders
        if (ny == 0 || ny == 24 || nx == 0 || nx == 79 || ny == HUDY - 1 ||
            ny == HUDY + 3) {
          continue;
        }

        board[ny][nx] = ' ';
        gotoxy(nx, ny);
        drawChar(nx, ny, ' ', Color::WHITE_BRIGHT);
      }
    }
  }
}

// perplexity AI.
void Screen::drawChar(int x, int y, char ch, Color color) {
  if (!enableColor) {
    gotoxy(x, y);
    std::cout << ch << std::flush;
    return;
  }

  // Optimized drawChar: Do not save/restore attributes every single char if
  // possible, but here we must to be safe for random access drawing. For better
  // performance, we'll keep it simple but correct.

  HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO info;
  GetConsoleScreenBufferInfo(h, &info);
  WORD oldAttr = info.wAttributes;

  gotoxy(x, y);
  SetConsoleTextAttribute(h, static_cast<WORD>(color));
  std::cout << ch << std::flush;
  SetConsoleTextAttribute(h, oldAttr);
}

bool Screen::isCellLitByTorch(int x, int y) const {
  // Cell is not in a dark area
  if (!dark[y][x])
    return true;

  // Torch item itself is always visible
  if (board[y][x] == '!')
    return true;

  const int TORCH_RADIUS = 13;

  // Check player 1
  if (p1HasTorch && manhattanDist(x, y, p1x, p1y) <= TORCH_RADIUS)
    return true;

  // Check player 2
  if (p2HasTorch && manhattanDist(x, y, p2x, p2y) <= TORCH_RADIUS)
    return true;

  // Dark cell with no torch in range
  return false;
}

void Screen::markDarkRect(int startX, int startY, int width, int height) {
  for (int y = startY; y < startY + height && y <= MAX_Y; ++y) {
    for (int x = startX; x < startX + width && x <= MAX_X; ++x) {
      dark[y][x] = true;
    }
  }
}

Screen &getScreen() {
  static Screen s; // Single shared Screen instance
  return s;        // Return reference to it
}
