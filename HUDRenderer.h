#pragma once
#include "ScoreManager.h"
#include "Screen.h"
#include "utils.h"
#include <iostream>
#include <string>


class HUDRenderer {
  const Screen &screen;
  const ScoreManager &scoreManager;
  bool doorOpen = false;
  bool switchOn = false;

public:
  HUDRenderer(const Screen &scr, const ScoreManager &sm)
      : screen(scr), scoreManager(sm) {}

  void draw() const {
    if (!screen.isHudActive())
      return;

    int hX = screen.getHudX();
    int hY = screen.getHudY();
    const int maxWidth = 62;

    {
      gotoxy(hX, hY);
      std::string line = "P1($) carry item:    life: ";
      if ((int)line.size() > maxWidth)
        line = line.substr(0, maxWidth);
      std::cout << line << std::flush;
    }

    {
      gotoxy(hX, hY + 1);
      std::string line = "P2(&) carry item:    life: ";
      if ((int)line.size() > maxWidth)
        line = line.substr(0, maxWidth);
      std::cout << line << std::flush;
    }

    {
      gotoxy(hX, hY + 2);
      std::string line = "Door open? : ";
      line += (doorOpen ? 'Y' : 'N');
      line += " || Switch open? : ";
      line += (switchOn ? 'Y' : 'N');
      line += " || Score: " + std::to_string(scoreManager.getScore());
      line += " || Time left: ";
      if ((int)line.size() > maxWidth)
        line = line.substr(0, maxWidth);
      std::cout << line << std::flush;
    }
  }

  bool isInArea(int x, int y) const {
    if (!screen.isHudActive())
      return false;
    int hX = screen.getHudX();
    int hY = screen.getHudY();
    return (x >= hX && x < hX + 67 && y >= hY && y < hY + 3);
  }

  void setDoorOpen(bool open) { doorOpen = open; }
  bool isDoorOpen() const { return doorOpen; }
  void setSwitchOn(bool on) { switchOn = on; }
  bool isSwitchOn() const { return switchOn; }
};
