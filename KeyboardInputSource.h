#pragma once
#include "GameInputSource.h"
#include <conio.h>

// Keyboard input for normal play
class KeyboardInputSource : public GameInputSource {
public:
  bool hasInput() override;
  char getKey() override;
  void onCycle(int cycle) override;
  bool isValid() const override;
};
