#include "Obstacle.h"
#include "game.h"
#include <iostream>

Obstacle::Obstacle(int x, int y, Screen &screen, game *gp) : gameptr(gp) {
  cells.clear();
  size = 0;

  // Scan for connected parts of the obstacle (horizontal group)
  int tx = x;
  while (tx >= 0 && tx <= Screen::MAX_X && screen.board[y][tx] == '*') {
    cells.push_back(point(tx, y, {0, 0}, '*'));
    size++;
    tx++; // right
  }

  tx = x - 1;
  while (tx >= 0 && screen.board[y][tx] == '*') {
    cells.push_back(point(tx, y, {0, 0}, '*'));
    size++;
    tx--; // left
  }
}

bool Obstacle::contains(int x, int y) const {
  for (const auto &cell : cells) {
    if (cell.getX() == x && cell.getY() == y)
      return true;
  }
  return false;
}

bool Obstacle::canPush(Direction dir, Screen &screen) const {
  int dx = dir.dx();
  int dy = dir.dy();

  for (const auto &cell : cells) {
    int newX = cell.getX() + dx;
    int newY = cell.getY() + dy;

    if (newX < 0 || newX > Screen::MAX_X || newY < 0 || newY > Screen::MAX_Y) {
      return false;
    }

    char ch = screen.board[newY][newX];

    // Blocked by wall or spring
    if (ch == 'W' || ch == '#') {
      return false;
    }
  }
  return true;
}

void Obstacle::push(Direction dir, Screen &screen) {
  int dx = dir.dx();
  int dy = dir.dy();

  // 1. Clear old obstacle positions
  for (auto &cell : cells) {
    screen.setChar(cell.getY(), cell.getX(), ' ');
    if (gameptr && !gameptr->isSilent())
      screen.drawChar(cell.getX(), cell.getY(), ' ', Color::WHITE_BRIGHT);
  }

  // 2. For each cell of the obstacle
  for (auto &cell : cells) {
    int newX = cell.getX() + dx;
    int newY = cell.getY() + dy;

    if (newX < 0 || newX > Screen::MAX_X || newY < 0 || newY > Screen::MAX_Y) {
      continue;
    }

    char ch = screen.board[newY][newX];

    // --- NEW LOGIC FOR ITEMS ---
    // If there is an item (not empty, not wall, not obstacle, not spring)
    if (ch != ' ' && ch != 'W' && ch != '*' && ch != '#') {
      int itemNewX = newX + dx;
      int itemNewY = newY + dy;

      // If item cannot move safely -> STOP obstacle before it
      if (itemNewX < 0 || itemNewX > Screen::MAX_X || itemNewY < 0 ||
          itemNewY > Screen::MAX_Y) {
        // put obstacle back to old cell and continue to next cell
        screen.setChar(newY, newX, ch); // leave item as is
        // don't put '*' here - obstacle effectively stops
        continue;
      }

      char nextCh = screen.board[itemNewY][itemNewX];

      // If next cell is wall, spring, obstacle, or another item -> do not push
      if (nextCh == 'W' || nextCh == '#' || nextCh == '*' || (nextCh != ' ')) {
        // leave item in place, obstacle stops before it

        screen.setChar(newY, newX, nextCh);
        continue;
      }

      // Safe to move item one step forward
      screen.setChar(newY, newX, ' ');
      if (gameptr && !gameptr->isSilent())
        screen.drawChar(newX, newY, ' ', Color::WHITE_BRIGHT);

      screen.setChar(itemNewY, itemNewX, ch);
      if (gameptr && !gameptr->isSilent())
        screen.drawChar(itemNewX, itemNewY, ch, Color::WHITE_BRIGHT);

      // Now newX,newY is free for the obstacle '*'
    }

    // Place '*' in new position if not blocked
    screen.setChar(newY, newX, '*');
    if (gameptr && !gameptr->isSilent())
      screen.drawChar(newX, newY, '*', Color::LIGHT_GREEN);

    cell.x = newX;
    cell.y = newY;
  }
}
