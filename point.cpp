#pragma once

#include "point.h"
#include "Screen.h"
#include <iostream>

void point::draw(char ch) {
  gotoxy(x, y);                  // Move console cursor to the point's position
  std::cout << ch << std::flush; // Print given character and flush output
}

void point::move() {

  if (x + dir.dx() < 0 || x + dir.dx() > Screen::MAX_X) {
    dir.setDirX(
        -dir.dx()); // Reverse horizontal direction when hitting screen edge
  }
  if (y + dir.dy() < 0 || y + dir.dy() > Screen::MAX_Y) {
    dir.setDirY(
        -dir.dy()); // Reverse vertical direction when hitting screen edge
  }
  if (dir.dx() == 0 && dir.dy() == 0)
    return; // Do nothing if direction is STAY

  x += dir.dx(); // Move point in x by current direction
  y += dir.dy(); // Move point in y by current direction
}
