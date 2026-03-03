#pragma once
#include "Screen.h" // Definition of class Screen
#include "point.h"  // Definition of class point
#include <cstring>  // For memcpy

class game;
class player {
  static constexpr size_t NUM_MOVES =
      5;                   // Number of movement keys for this player
  char Moves[NUM_MOVES];   // Array of movement keys (e.g. "dxaws")
  point p;                 // Current position and drawing data of the player
  Screen &screen;          // Reference to the game screen used for drawing
  char carry_item = ' ';   // Item currently carried (' ' means nothing)
  bool isactive = true;    // True if player is still active in the game
  game *gameptr = nullptr; // Pointer back to the main game object
  int startpointX;         // Starting X position for reset
  int startpointY;         // Starting Y position for reset
  int springSpeed = 0;     // speed given by spring (0 = no spring)
  int springTicksLeft = 0; // how many moves left under spring effect
  Direction springDir = Direction::directions[Direction::STAY];
  int life = 5;

public:
  player(const point &start_point, const char (&the_moves)[NUM_MOVES + 1],
         Screen &the_screen, char carry_item1)
      : screen(the_screen),
        carry_item(carry_item1) // Initialize screen reference and carried item
  {
    memcpy(Moves, the_moves,
           sizeof(Moves[0]) * NUM_MOVES); // Copy movement keys into Moves
    p = start_point;                      // Set current position to start point
    startpointX = start_point.getX();     // Store starting X for future resets
    startpointY = start_point.getY();     // Store starting Y for future resets
  }
  int get_startpointX() const {
    return startpointX; // Return stored starting X position
  }
  int get_startpointY() const {
    return startpointY; // Return stored starting Y position
  }
  bool iscarrying_item() const {
    return carry_item != ' '; // True if player is currently holding an item
  }
  bool pickupIfPossible(); // Try to pick up an item from the current tile
  bool getIsActive() const {
    return isactive; // Check if player is active
  }
  void reactivate() {
    isactive = true; // Reactivate player (after reset, etc.)
  }
  void deactivate() {
    isactive = false; // Deactivate player (e.g. dead or finished)
  }
  void disposeItem(); // Drop or use the carried item

  // perplexity AI
  void setGamePtr(game *gp) {
    gameptr = gp; // Store pointer to the game object
  }
  //
  game *getGamePtr() const {
    return gameptr; // Return pointer to the game object
  }
  void move();              // Perform movement based on current input
  void keyPressed(char ch); // Handle a key press for this player
  int getX() const {
    return p.getX(); // Get current X position from point
  }
  int getY() const {
    return p.getY(); // Get current Y position from point
  }
  void draw(char c) {
    p.draw(c); // Draw the player using the point's draw function
  }
  void setCarryItem(char item) {
    carry_item = item; // Set the item currently carried by the player
  }
  char getCarryItem() {
    return carry_item;
  } // Return the item currently carried
  void useKey() {
    carry_item = ' '; // Consume key (or item) and clear carried item
  }
  void setPosition(int x, int y) {
    p = point(x, y, {0, 0},
              p.c); // Set new position while keeping current character
  }
  void setStartPosition(int x,
                        int y); // Change starting position (for new rooms)
  bool isUnderSpringEffect() const { return springTicksLeft > 0; }
  void setSpringEffect(int speed, int ticks, Direction dir) {
    springSpeed = speed;
    springTicksLeft = ticks;
    springDir = dir;
  }
  void clearSpringEffect() {
    springSpeed = 0;
    springTicksLeft = 0;
    springDir = Direction::directions[Direction::STAY];
  }
  bool moveWithSpring(); // Move player according to spring effect
  const point &getPoint() const { return p; }
  void setDirection(const Direction &dir) { p.changeDir(dir); }
  int getSpringSpeed() const { return springSpeed; }
  Direction getDir() const { return p.getDir(); }
  int getLife() const { return life; }
  void setLife(int l) { life = l; }
  void updateHUD();
};
