#include "KeyboardInputSource.h"

// Check if a key was pressed
bool KeyboardInputSource::hasInput() { return _kbhit() != 0; }

// Read and return next key
char KeyboardInputSource::getKey() { return _getch(); }

// Nothing to do for keyboard each cycle
void KeyboardInputSource::onCycle(int cycle) {
  // Keyboard doesn't need cycle tracking
}

// Keyboard is always available
bool KeyboardInputSource::isValid() const { return true; }
