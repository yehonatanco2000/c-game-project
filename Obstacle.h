#pragma once
#include "Screen.h"
#include "player.h"
#include "point.h"
#include <vector>

class game;

class Obstacle {
private:
  std::vector<point> cells; // All obstacle cells
  int size;                 // Number of cells (push strength)
  game *gameptr = nullptr;  // Pointer back to the main game object

public:
  Obstacle(int startX, int startY, Screen &screen,
           game *gp); // Builds connected * group

  int getSize() const { return size; }
  void setSize(int s) { size = s; } // **NEW: For size updates**
  bool contains(int x, int y) const;
  bool canPush(Direction dir, Screen &screen) const;
  void push(Direction dir, Screen &screen);
  const std::vector<point> &getCells() const { return cells; }
  std::vector<point> &getCells() { return cells; }
  void setGamePtr(game *gp) { gameptr = gp; }
};
