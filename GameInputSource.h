#pragma once
#include <string>

// Abstract base class for game input (polymorphism)
class GameInputSource {
public:
  virtual ~GameInputSource() = default;

  // Check if input is available
  virtual bool hasInput() = 0;

  // Get next key press
  virtual char getKey() = 0;

  // Called each game cycle
  virtual void onCycle(int cycle) = 0; // Called every game cycle
  virtual bool
  isValid() const = 0; // Check if source is valid (e.g. file loaded)
  virtual char getRiddleAnswer() {
    return 0;
  } 
  virtual bool getSavedColorMode() const {
    return false;
  } 
};
