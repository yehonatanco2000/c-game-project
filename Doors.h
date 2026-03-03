#pragma once
class Doors {
  int x, y;         // Door position on the board
  char symbol;      // Door symbol on the map: '1','2','3',...
  int requiredKeys; // How many keys are needed to open this door
  int currentKeys;  // How many keys have already been delivered

public:
  Doors(int x, int y,
        char symbol) // Constructor: create door at (x,y) with given symbol
      : x(x), y(y), symbol(symbol),
        requiredKeys(symbol -
                     '0'), // Convert digit character (e.g. '3') to int 3
        currentKeys(0) {}

  int getX() const { return x; }                       // Get door x-coordinate
  int getY() const { return y; }                       // Get door y-coordinate
  int getRequiredKeys() const { return requiredKeys; } // Get total keys needed
  int getCurrentKeys() const {
    return currentKeys;
  }                                // Get keys already collected
  void addKey() { ++currentKeys; } // Add one key to the door's counter
  bool isOpen() const {
    return currentKeys >= requiredKeys;
  } // Check if door is open
  char getSymbol() const {
    return symbol; // Return original symbol of the door
  }
  void setRequiredKeys(int keys) {
    requiredKeys = keys;
  } // Override default key requirement
};
