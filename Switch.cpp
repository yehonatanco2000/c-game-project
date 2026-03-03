#include "Switch.h"

void Switch::draw(Screen &screen) const {
  char ch = getSymbol();              // Choose symbol based on state
  screen.setChar(getY(), getX(), ch); // Update board array
  gotoxy(getX(), getY());             // Move console cursor to switch position
  std::cout << ch << std::flush;      // Print symbol to screen
}

void Switch::toggle(Screen &screen) {
  on = !on;                           // Flip ON/OFF state
  char ch = getSymbol();              // Get new symbol after toggle
  screen.setChar(getY(), getX(), ch); // Update board cell
  if (!screen.isDark(getX(), getY()) || screen.isCellLitByTorch(getX(), getY()))
    screen.drawChar(getX(), getY(), ch, Color::CYAN); // Draw switch in cyan
}