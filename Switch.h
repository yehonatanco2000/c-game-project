#pragma once
#include "Screen.h"
#include "point.h"
#include "utils.h"
#include <iostream>


// Base class for all switches (abstract)
class Switch {
protected:
  int x, y;
  bool on;

public:
  Switch(int x, int y, bool is_on) : x(x), y(y), on(is_on) {}
  virtual ~Switch() = default;

  int getX() const { return x; }
  int getY() const { return y; }
  bool isOn() const { return on; }

  virtual char getSymbol() const = 0;    // Pure virtual
  virtual bool isDoorSwitch() const = 0; // Pure virtual

  void draw(Screen &screen) const;
  void toggle(Screen &screen);
};

// Door switch: / = ON, \ = OFF
class DoorSwitch : public Switch {
public:
  DoorSwitch(int x, int y, bool is_on) : Switch(x, y, is_on) {}

  char getSymbol() const override { return on ? '/' : '\\'; }
  bool isDoorSwitch() const override { return true; }
};

// Wall switch: + = ON, - = OFF
class WallSwitch : public Switch {
public:
  WallSwitch(int x, int y, bool is_on) : Switch(x, y, is_on) {}

  char getSymbol() const override { return on ? '+' : '-'; }
  bool isDoorSwitch() const override { return false; }
};
