#pragma once
#include "Bomb.h"
#include "Doors.h"
#include "GameInputSource.h"
#include "HUDRenderer.h"
#include "Menu.h"
#include "Obstacle.h"
#include "ResultRecorder.h"
#include "ResultVerifier.h"
#include "Riddle.h"
#include "RoomManager.h"
#include "ScoreManager.h"
#include "Screen.h"
#include "Spring.h"
#include "StepRecorder.h"
#include "Switch.h"
#include "Torch.h"
#include "keys.h"
#include "player.h"
#include <iostream>
#include <memory>
#include <vector>

struct ActiveBomb {
  int x, y;   // Bomb position on the board
  int timer;  // How many game cycles left before explosion
  bool alive; // True if bomb has not exploded yet
};
constexpr int TIME_PER_GAME = 100; // Total time for the game in seconds
constexpr int GAME_CLOCK_TICKS_PER_SECOND = 10;
constexpr int EXPLOSION_RADIUS = 3;     // Blast radius for bombs
constexpr int HUD_BASE_WIDTH = 62;      // HUD width when score < 100
constexpr int HUD_WIDE_WIDTH = 63;      // HUD width when score >= 100
constexpr int WALL_SWITCH_X_START = 29; // Wall switch segment start column
constexpr int WALL_SWITCH_X_END = 32;   // Wall switch segment end column

class game {
  Screen theScreen;              // The game screen (map and drawing)
  player p1;                     // First player
  player p2;                     // Second player
  std::vector<ActiveBomb> bombs; // List of active bombs counting down
  std::vector<keys> keys;        // All key items in the current room
  std::vector<Doors> doors;      // All doors in the current room
  std::vector<Bomb> bombItem;    // Bomb items lying on the floor (collectable)
  std::vector<Riddle> riddles;   // All riddles in the current room
  std::vector<std::unique_ptr<Switch>> switches; // Polymorphic switches
  std::vector<Torch> torches;      // All torches in the current room
  std::vector<Spring> springs;     // All springs in the current room
  std::vector<Obstacle> obstacles; // All obstacles in the current room
  bool openedDoor = false; // Is the door currently open? (mirrored in HUD)
  bool switchOn = false;   // Is the switch currently ON? (mirrored in HUD)
  RoomManager roomManager; // Manages rooms, riddles, file loading
  int playersExitedThisRoom = 0; // How many players already exited this room
  int doorTargetRoom = 1;        // Room index to go to when passing the door
  bool loadSuccess = true;
  std::string fatalError;
  ScoreManager scoreManager; // Manages the game score
  HUDRenderer hudRenderer;   // Renders the HUD display
  int TimeGame = 0;

  // Polymorphic input source (keyboard or file)
  std::unique_ptr<GameInputSource> inputSource;

  // Recording for save mode
  StepRecorder stepRecorder;
  ResultRecorder resultRecorder;
  ResultVerifier resultVerifier;

  // Game mode flags
  bool silentMode = false;
  bool saveMode = false;
  bool loadMode = false;

  // Game cycle counter for recording
  int gameCycle = 0;

public:
  game()                        // Game constructor
      : theScreen(getScreen()), // Use shared Screen instance
        p1(point(3, 9, {0, 0}, '$'), "dxaws", theScreen, ' '),  // Init player 1
        p2(point(68, 9, {0, 0}, '&'), "lmjik", theScreen, ' '), // Init player 2
        hudRenderer(theScreen, scoreManager) // Init HUD renderer
  {
    p1.setGamePtr(this); // Give player 1 pointer back to game
    p2.setGamePtr(this); // Give player 2 pointer back to game

    if (roomManager.loadScreensFromFiles(fatalError)) {
      theScreen.copyFrom(roomManager.getRoom(0));
    } else {
      loadSuccess = false;
      return;
    }

    if (!roomManager.loadRiddlesFromFile("riddles.txt")) {
      loadSuccess = false;
      fatalError = "CRITICAL ERROR: Failed to load 'riddles.txt' or file is "
                   "empty.\nPlease verify that the riddle file is present and "
                   "correctly formatted.";
      return;
    }

    initObjectsFromBoard(); // Scan board and create keys, doors, etc.
    int p1x, p1y, p2x, p2y;
    findPlayerStartPositions(p1x, p1y, p2x, p2y);
    setposition_player1(p1x, p1y);
    setposition_player2(p2x, p2y);
  }
  void start_game(); // Main game loop entry point
  void addBomb(int x, int y) {
    bombs.push_back(
        {x, y, 5, true}); // Add new active bomb at (x,y) with 5 cycles
  }
  bool
  isPlayerAt(int x, int y,
             const player *ignore) const; // Check if any player is at (x,y)
  void handleStepOnObjects(player &pl);   // Handle player stepping on items
  void goToNextRoom();                    // Move game to the next room
  bool isLastActivePlayer(
      const player &pl) const;       // Check if this is the last active player
  void playerPassedDoor(player &pl); // Logic when player passes a door
  void explodeAt(int x, int y);      // Trigger explosion at (x,y)
  void update_Internal_Wall_By_Switch(
      const Switch &sw); // Update walls affected by a switch
  void setposition_player1(int x, int y) {
    p1.setPosition(x, y); // Set position of player 1
  }
  void setposition_player2(int x, int y) {
    p2.setPosition(x, y); // Set position of player 2
  }
  player *getOtherPlayer(const player *pl);
  bool tryPushObstacle(point &target, Direction dir);
  void mergeObstaclesOnBoard();
  int getHudX() const { return theScreen.getHudX(); }
  int getHudY() const { return theScreen.getHudY(); }
  bool isHudZone(int x, int y) const {
    return theScreen.isHudActive() && theScreen.getHudY() != -1 &&
           isHudArea(x, y);
  }
  Screen &getCurrentRoomScreen() {
    return roomManager.isLoaded()
               ? roomManager.getRoom(roomManager.getCurrentIndex())
               : theScreen;
  }
  void drawHUD() {
    if (!silentMode) {
      hudRenderer.setDoorOpen(openedDoor);
      hudRenderer.setSwitchOn(switchOn);
      hudRenderer.draw();
    }
  }
  bool isHudArea(int x, int y) const { return hudRenderer.isInArea(x, y); }
  int getScore() const { return scoreManager.getScore(); }
  void setScore(int s) { scoreManager.setScore(s); }
  int getTimeGame() const { return TimeGame; }
  void setTimeGame(int t) { TimeGame = t; }

  // Set input source (polymorphism)
  void setInputSource(std::unique_ptr<GameInputSource> source) {
    inputSource = std::move(source);
  }

  // Set game modes
  void setSilentMode(bool silent) { silentMode = silent; }
  bool isSilent() const { return silentMode; }
  void setSaveMode(bool save) { saveMode = save; }
  void setLoadMode(bool load) { loadMode = load; }

  // Get game cycle for recording
  int getGameCycle() const { return gameCycle; }

  // Access recorders
  StepRecorder &getStepRecorder() { return stepRecorder; }
  ResultRecorder &getResultRecorder() { return resultRecorder; }

  // Get screen file names
  const std::vector<std::string> &getScreenFileNames() const {
    return roomManager.getFileNames();
  }

private:
  void initObjectsFromBoard(); // Read screen board and create all objects
  int pause_menu();            // Show pause menu and return user choice
  int countSpringCompressed(const player &pl) const;
  void activateSpringIfNeeded(player &pl);

  bool findLegendPosition(); // Find first L position
  void findPlayerStartPositions(int &p1x, int &p1y, int &p2x, int &p2y);

  // --- Sub-functions extracted from start_game() ---
  // Tracks state between game loop cycles (for lighting optimization and
  // timing)
  struct GameLoopState {
    int prevP1X, prevP1Y, prevP2X,
        prevP2Y; // Previous player positions (detect movement)
    bool prevP1HadTorch,
        prevP2HadTorch;  // Previous torch status (detect pickup/drop)
    int clockTicks;      // Tick counter (10 ticks = 1 game second)
    int currentHudWidth; // HUD width (changes when score >= 100)
  };
  void setupInputAndRecording();     // Setup input source and start recorders
  void initialDraw();                // Draw screen and HUD at game start
  GameLoopState initGameLoopState(); // Initialize per-cycle tracking state
  void updateLighting(
      GameLoopState &state); // Update torch info and redraw if needed
  void
  updateGameClock(GameLoopState &state); // Advance game clock and display time
  void updateBombs(); // Tick bomb timers and trigger explosions
  void movePlayers(); // Move active players
  bool processInput(
      GameLoopState &state); // Handle input, returns true if game should exit
  bool checkGameOver(); // Check game-over conditions, returns true if game ends

  // --- Sub-functions extracted from handleStepOnObjects() ---
  void handleSwitchInteraction(player &pl, int x, int y);
  void handleRiddleInteraction(player &pl, int x, int y);
  bool handleDoorInteraction(player &pl, int x,
                             int y); // returns true if handled (early return)
  void handleItemPickup(player &pl, int x, int y);
};
