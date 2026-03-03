#include "game.h"
#include "Directions.h"
#include "KeyboardInputSource.h"
#include "Menu.h"
#include "Riddle.h"
#include "Screen.h"
#include "player.h"
#include <Windows.h>
#include <algorithm>
#include <conio.h>

#include <fstream>
#include <iostream>
#include <queue>
#include <string>

void game::start_game() {
  if (!loadSuccess) {
    cls();
    std::cout << "==========================================================="
              << std::endl;
    std::cout << fatalError << std::endl;
    std::cout << "==========================================================="
              << std::endl;
    std::cout << "\nPress any key to return to menu..." << std::endl;
    _getch();
    cls();
    Menu return_menu;
    return_menu.show_menu();
    return;
  }

  hideCursor();
  setupInputAndRecording();
  initialDraw();
  GameLoopState state = initGameLoopState();

  // ===== MAIN GAME LOOP =====
  while (true) {
    updateLighting(state);
    updateGameClock(state);

    if (p1.getIsActive())
      p1.updateHUD();
    if (p2.getIsActive())
      p2.updateHUD();

    updateBombs();
    movePlayers();

    if (!silentMode) {
      Sleep(loadMode ? 50 : 100);
    }

    if (processInput(state))
      return; // Game exited via ESC->menu

    if (checkGameOver())
      return; // Game ended

    ++gameCycle;
  }
}

void game::setupInputAndRecording() {
  if (!inputSource) {
    inputSource = std::make_unique<KeyboardInputSource>();
  }
  if (loadMode && inputSource) {
    theScreen.enableColor = inputSource->getSavedColorMode();
    resultVerifier.load("adv-world.result");
  }
  if (saveMode) {
    stepRecorder.start("adv-world.steps", roomManager.getFileNames(),
                       theScreen.enableColor);
    resultRecorder.start("adv-world.result");
  }
}

void game::initialDraw() {
  if (!silentMode) {
    theScreen.draw();
    drawHUD();
    p1.updateHUD();
    p2.updateHUD();
  }
}

game::GameLoopState game::initGameLoopState() {
  GameLoopState state;
  state.prevP1X = p1.getX();
  state.prevP1Y = p1.getY();
  state.prevP2X = p2.getX();
  state.prevP2Y = p2.getY();
  state.prevP1HadTorch = (p1.getCarryItem() == '!');
  state.prevP2HadTorch = (p2.getCarryItem() == '!');
  state.clockTicks = 0;
  state.currentHudWidth = HUD_BASE_WIDTH;
  gameCycle = 0;
  TimeGame = 0;
  scoreManager.reset();
  return state;
}

// ============================================================
// Sub-functions extracted from start_game()
// ============================================================

void game::updateLighting(GameLoopState &state) {
  theScreen.setTorchInfo(p1.getCarryItem() == '!', p2.getCarryItem() == '!',
                         p1.getX(), p1.getY(), p2.getX(), p2.getY());

  bool needRedraw = false;
  bool p1Moved = (p1.getX() != state.prevP1X || p1.getY() != state.prevP1Y);
  bool p2Moved = (p2.getX() != state.prevP2X || p2.getY() != state.prevP2Y);
  bool p1HasTorch = (p1.getCarryItem() == '!');
  bool p2HasTorch = (p2.getCarryItem() == '!');

  if ((p1Moved && p1HasTorch) || (p2Moved && p2HasTorch))
    needRedraw = true;
  if (p1HasTorch != state.prevP1HadTorch || p2HasTorch != state.prevP2HadTorch)
    needRedraw = true;

  if (needRedraw && !silentMode) {
    theScreen.drawTorchArea(state.prevP1X, state.prevP1Y,
                            state.prevP2X, state.prevP2Y,
                            state.prevP1HadTorch, state.prevP2HadTorch);
    drawHUD();
  }

  state.prevP1X = p1.getX();
  state.prevP1Y = p1.getY();
  state.prevP2X = p2.getX();
  state.prevP2Y = p2.getY();
  state.prevP1HadTorch = p1HasTorch;
  state.prevP2HadTorch = p2HasTorch;
}

void game::updateGameClock(GameLoopState &state) {
  bool p1HasTorch = (p1.getCarryItem() == '!');
  bool p2HasTorch = (p2.getCarryItem() == '!');

  state.clockTicks++;
  if (state.clockTicks % GAME_CLOCK_TICKS_PER_SECOND == 0) {
    if (getTimeGame() < TIME_PER_GAME) {
      setTimeGame(getTimeGame() + 1);
    }
    if (getScore() >= 100)
      state.currentHudWidth = HUD_WIDE_WIDTH;
    int tx = getHudX() + state.currentHudWidth;
    int ty = getHudY() + 2;
    if (roomManager.getCurrentIndex() != 3 && !silentMode) {
      gotoxy(tx, ty);
      std::cout << (TIME_PER_GAME - getTimeGame()) << " " << std::flush;
    }
  }
}

void game::updateBombs() {
  for (auto &b : bombs) {
    if (!b.alive)
      continue;
    if (!silentMode) {
      gotoxy(b.x, b.y);
      std::cout << b.timer << std::flush;
      Sleep(50);
    }
    if (--b.timer == 0) {
      explodeAt(b.x, b.y);
      b.alive = false;
      if (!silentMode) {
        theScreen.draw();
        drawHUD();
      }
    }
  }
}

void game::movePlayers() {
  if (p1.getIsActive())
    p1.move();
  if (p2.getIsActive())
    p2.move();
}

bool game::processInput(GameLoopState &state) {
  constexpr char ESC = 27;
  inputSource->onCycle(gameCycle);

  if (!inputSource->hasInput())
    return false;

  char key = inputSource->getKey();

  if (stepRecorder.isActive()) {
    stepRecorder.recordStep(gameCycle, key);
  }

  if (key == ESC) {
    int action = pause_menu();
    if (action == 1) { // Return to main menu
      if (saveMode) {
        resultRecorder.recordGameEnd(gameCycle, getScore());
      }
      if (loadMode && resultVerifier.isActive()) {
        resultVerifier.verifyGameEnd(gameCycle, getScore());
        if (silentMode) {
          cls();
          if (resultVerifier.isPassed() && resultVerifier.allEventsConsumed())
            std::cout << "Test passed" << std::endl;
          else
            std::cout << "Test failed" << std::endl;
        }
      }
      stepRecorder.stop();
      resultRecorder.stop();
      if (!silentMode)
        cls();
      if (!loadMode) {
        Menu show_main_menu;
        show_main_menu.show_menu();
      }
      return true; // Signal game exit
    } else {       // Continue game
      if (!silentMode) {
        theScreen.draw();
        drawHUD();
        p1.updateHUD();
        p2.updateHUD();
      }
      state.prevP1X = p1.getX();
      state.prevP1Y = p1.getY();
      state.prevP2X = p2.getX();
      state.prevP2Y = p2.getY();
    }
  } else if ((unsigned char)key == 224 || (unsigned char)key == 0) {
    // Special key - consume extra byte immediately
    p1.keyPressed(key);
    p2.keyPressed(key);
    if (inputSource->hasInput()) {
      char secondByte = inputSource->getKey();
      if (stepRecorder.isActive()) {
        stepRecorder.recordStep(gameCycle, secondByte);
      }
      p1.keyPressed(secondByte);
      p2.keyPressed(secondByte);
    }
  } else {
    p1.keyPressed(key);
    p2.keyPressed(key);
  }
  return false;
}

bool game::checkGameOver() {
  bool bothDead = !p1.getIsActive() && !p2.getIsActive();
  bool timeOut = (getTimeGame() >= TIME_PER_GAME);
  bool stepsOut = (loadMode && !inputSource->isValid());

  if (!bothDead && !stepsOut)
    return false;

  if (saveMode && bothDead) {
    resultRecorder.recordGameEnd(gameCycle, getScore());
  }

  bool resultsPending =
      resultVerifier.isActive() && !resultVerifier.allEventsConsumed();
  bool cycleExceeded = false;

  if (resultsPending) {
    int nextCycle = resultVerifier.getNextExpectedCycle();
    if (nextCycle != -1) {
      if (bothDead || timeOut || gameCycle > nextCycle + 50)
        cycleExceeded = true;
    } else {
      resultsPending = false; // Only END remains
    }
  }

  if (!resultsPending || cycleExceeded) {
    if (!silentMode)
      theScreen.draw();

    if (resultVerifier.isActive()) {
      resultVerifier.verifyGameEnd(gameCycle, getScore());
    }

    if (loadMode && !silentMode) {
      cls();
      std::cout << "\n========================================" << std::endl;
      std::cout << "          REPLAY COMPLETED" << std::endl;
      std::cout << "========================================" << std::endl;
      std::cout << "Press any key to exit..." << std::endl;
      _getch();
    } else {
      if (resultVerifier.isActive()) {
        cls();
        if (resultVerifier.isPassed() && resultVerifier.allEventsConsumed())
          std::cout << "Test passed" << std::endl;
        else
          std::cout << "Test failed" << std::endl;
      }
    }
    return true; // Game ended
  }
  return false; // Game continues (waiting for more result events)
}

// ============================================================
// Object initialization
// ============================================================

void game::initObjectsFromBoard() {
  riddles.clear(); // Remove old riddles from previous room

  for (int y = 0; y <= Screen::MAX_Y; ++y) {
    for (int x = 0; x <= Screen::MAX_X; ++x) {
      char ch = theScreen.board[y][x]; // Character at current board cell

      if (ch == 'k') {
        keys.emplace_back(x, y); // Add key object at this position
      }
      if (ch == '?') {
        if (roomManager.hasRiddles()) {
          RiddleData rd = roomManager.getNextRiddle();
          riddles.emplace_back(x, y, rd);
        }
      } else if (ch == '/' || ch == '\\') {
        switches.push_back(std::make_unique<DoorSwitch>(x, y, ch == '/'));
      } else if (ch == '+' || ch == '-') {
        switches.push_back(std::make_unique<WallSwitch>(x, y, ch == '+'));

      } else if (ch == '@') {
        bombItem.emplace_back(x, y); // Bomb item on floor
      } else if (ch >= '1' && ch <= '9') {
        Doors newDoor(x, y, ch);
        // Look for overrides for this room
        int roomIdx = roomManager.getCurrentIndex();
        const auto &overrides = roomManager.getDoorKeyOverrides(roomIdx);
        for (const auto &p : overrides) {
          if (p.first == ch)
            newDoor.setRequiredKeys(p.second);
        }
        doors.push_back(newDoor);

      } else if (ch == '!') {
        torches.emplace_back(x, y); // Add torch at this position
      } else if (ch == '#') {
        springs.emplace_back(x, y); // add spring cell at this position
      } else if (ch == '*') {       // Obstacle
        // Check if an obstacle part is already here to group them
        bool alreadyExists = false;
        for (const auto &obs : obstacles) {
          if (obs.contains(x, y)) {
            alreadyExists = true;
            break;
          }
        }
        if (!alreadyExists) {
          obstacles.emplace_back(x, y, theScreen, this);
        }
      }
    }
  }
}

// ============================================================
// Handle objects when player steps on them (refactored)
// ============================================================

void game::handleStepOnObjects(player &pl) {
  int x = pl.getX();
  int y = pl.getY();

  handleSwitchInteraction(pl, x, y);
  handleRiddleInteraction(pl, x, y);
  if (handleDoorInteraction(pl, x, y))
    return; // Door interaction handled; skip item pickup
  handleItemPickup(pl, x, y);
  activateSpringIfNeeded(pl);
}

void game::handleSwitchInteraction(player &pl, int x, int y) {
  for (size_t i = 0; i < switches.size(); ++i) {
    if (switches[i]->getX() == x && switches[i]->getY() == y) {
      switches[i]->toggle(theScreen);
      switchOn = switches[i]->isOn();
      drawHUD();

      if (!switches[i]->isDoorSwitch()) {
        update_Internal_Wall_By_Switch(*switches[i]);
      }
      break; // Only one switch per tile
    }
  }
}

void game::handleRiddleInteraction(player &pl, int x, int y) {
  for (size_t i = 0; i < riddles.size(); ++i) {
    if (riddles[i].getX() == x && riddles[i].getY() == y) {

      bool solved;
      char riddleAnswer = '0';

      // In load mode, get answer from input source
      if (loadMode) {
        if (inputSource) {
          riddleAnswer = inputSource->getRiddleAnswer();
          if (riddleAnswer != 0) {
            solved = riddles[i].askWithAnswer(riddleAnswer, silentMode);
          } else {
            solved = false;
          }
        } else {
          solved = false;
        }
      } else {
        // Normal mode - ask player
        solved = riddles[i].ask(silentMode);
        riddleAnswer =
            char('1' + riddles[i].getCorrectIndex()); // Default if correct
      }

      // Record riddle answer in save mode
      if (stepRecorder.isActive() && !loadMode) {
        char answerChar = riddles[i].getLastChoice();
        stepRecorder.recordStep(gameCycle, answerChar);
      }

      // Determine player number for recording
      int playerNum = (&pl == &p1) ? 1 : 2;

      if (solved) {
        scoreManager.addPoints(SCORE_RIDDLE_CORRECT);

        if (resultRecorder.isActive()) {
          resultRecorder.recordRiddle(gameCycle, playerNum,
                                      riddles[i].getQuestion(),
                                      riddles[i].getAnswer(), true);
        }
        if (loadMode && resultVerifier.isActive()) {
          resultVerifier.verifyRiddle(gameCycle, playerNum,
                                      riddles[i].getQuestion(),
                                      riddles[i].getAnswer(), true);
        }

        if (!silentMode) {
          std::cout << "Correct answer!" << std::endl;
          std::cout << "you get 50 points! Your current score is: "
                    << getScore() << std::endl;
          Sleep(2500);
        }
        // Correct answer: remove riddle mark from screen and vector
        theScreen.setChar(y, x, ' ');
        riddles.erase(riddles.begin() + i);
      } else {
        scoreManager.subtractPoints(SCORE_RIDDLE_WRONG);

        if (resultRecorder.isActive()) {
          resultRecorder.recordRiddle(gameCycle, playerNum,
                                      riddles[i].getQuestion(), "", false);
        }
        if (loadMode && resultVerifier.isActive()) {
          resultVerifier.verifyRiddle(gameCycle, playerNum,
                                      riddles[i].getQuestion(), "", false);
        }

        if (!silentMode) {
          std::cout << "Wrong answer! Returning to start position."
                    << std::endl;
          std::cout << "you lose 30 points! Your current score is: "
                    << getScore() << std::endl;
          Sleep(3000);
        }
        // Wrong answer: reset player to starting position in this room
        pl.setPosition(pl.get_startpointX(), pl.get_startpointY());
      }
      if (!silentMode) {
        cls();
        theScreen.draw();
        drawHUD();
      }
    }
  }
}

bool game::handleDoorInteraction(player &pl, int x, int y) {
  for (size_t i = 0; i < doors.size(); ++i) {
    if (doors[i].getX() == x && doors[i].getY() == y) {

      // If player holds a key, give it to the door
      if (pl.iscarrying_item() && pl.getCarryItem() == 'k') {
        pl.useKey();
        doors[i].addKey();
      }
      bool canopen = doors[i].isOpen();
      bool hasSwitchPermission = false;

      bool needSwitch = false;
      for (const auto &sw : switches) {
        if (sw->isDoorSwitch()) {
          needSwitch = true;
          if (sw->isOn())
            hasSwitchPermission = true;
        }
      }
      if (needSwitch)
        canopen = canopen && hasSwitchPermission;

      if (canopen) {
        openedDoor = true;
        pl.deactivate();
        playerPassedDoor(pl);
        if (!silentMode) {
          theScreen.draw();
          drawHUD();
        }
      }

      return true; // Door found ׳’ג‚¬ג€ stop further object checks
    }
  }
  return false; // No door at this position
}

void game::handleItemPickup(player &pl, int x, int y) {
  char before = theScreen.board[y][x]; // What was on this tile
  if (pl.pickupIfPossible()) {
    if (before == 'k') {
      for (size_t i = 0; i < keys.size(); ++i) {
        if (keys[i].getX() == x && keys[i].getY() == y) {
          keys.erase(keys.begin() + i);
          break;
        }
      }
    } else if (before == '@') {
      for (size_t i = 0; i < bombItem.size(); ++i) {
        if (bombItem[i].getX() == x && bombItem[i].getY() == y) {
          bombItem.erase(bombItem.begin() + i);
          break;
        }
      }
    } else if (before == '!') {
      for (size_t i = 0; i < torches.size(); ++i) {
        if (torches[i].getX() == x && torches[i].getY() == y) {
          torches.erase(torches.begin() + i);
          break;
        }
      }
    }
  }
}

// ============================================================
// Room transitions
// ============================================================

void game::goToNextRoom() {
  if (getTimeGame() < TIME_PER_GAME) {
    scoreManager.addPoints(SCORE_ROOM_FAST); // Bonus for finishing room quickly
    if (!silentMode) {
      cls();
      std::cout << "Room completed quickly! Bonus 200 points! Current score: "
                << getScore() << std::endl;
      Sleep(2500);
    }
  } else {
    scoreManager.addPoints(SCORE_ROOM_NORMAL); // Standard room completion score
    if (!silentMode) {
      cls();
      std::cout << "Room completed! You get 50 points! Current score: "
                << getScore() << std::endl;
      Sleep(2500);
    }
  }
  if (p1.getLife() == 5 && p2.getLife() == 5) {
    bool Full_Life;
    scoreManager.addPoints(SCORE_FULL_LIFE_BONUS);
    if (!silentMode) {
      cls();
      std::cout << "you finish the room with full life! Bonus 500 points! "
                   "Current score: "
                << getScore() << std::endl;
      Sleep(2500);
    }
  }
  roomManager.advanceRoom(); // Move to next room index

  // Check if we reached the end room or beyond
  if (roomManager.isLastRoom()) {
    theScreen.setHudInfo(0, 21, false);
    // Load the last room (end screen) from files
    if (roomManager.getCurrentIndex() < roomManager.getRoomCount()) {
      roomManager.copyCurrentRoomTo(theScreen);
    }
    if (!silentMode) {
      theScreen.draw();
      gotoxy(38, 22);
      std::cout << getScore() << std::endl;
    }
    // Record game end
    if (resultRecorder.isActive()) {
      resultRecorder.recordGameEnd(gameCycle, getScore());
      resultRecorder.stop();
      stepRecorder.stop();
    }
    if (!silentMode && !loadMode) {
      Sleep(100000);
    } else if (!silentMode) {
      Sleep(2000);
    }
    p1.deactivate(); // Deactivate player 1
    p2.deactivate(); // Deactivate player 2
    return;
  }

  // Load the next room from the vector (read from external files)
  roomManager.copyCurrentRoomTo(theScreen);
  p1.reactivate(); // Reactivate player 1
  p1.setCarryItem(' ');
  p2.reactivate(); // Reactivate player 2
  p2.setCarryItem(' ');
  keys.clear(); // Clear all old objects
  doors.clear();
  bombItem.clear();
  switches.clear();
  obstacles.clear();      // clear all old objects
  initObjectsFromBoard(); // Scan new board for objects

  setTimeGame(0);     // Reset game time for new room
  openedDoor = false; // Reset door opened flag
  switchOn = false;   // Reset switch state
  // Reset player positions for new room
  int p1x, p1y, p2x, p2y;
  findPlayerStartPositions(p1x, p1y, p2x, p2y);
  setposition_player1(p1x, p1y);
  p1.setStartPosition(p1x, p1y);
  setposition_player2(p2x, p2y);
  p2.setStartPosition(p2x, p2y);
  if (!silentMode) {
    theScreen.draw(); // Draw whole new room
    p1.draw('$');     // Draw player 1 character
    p2.draw('&');     // Draw player 2 character
    drawHUD();
    p1.updateHUD();
    p2.updateHUD();
  }
}

// ============================================================
// Pause menu
// ============================================================

int game::pause_menu() {
  if (!silentMode) {
    cls(); // Clear screen and show pause menu
    std::cout << "============ PAUSED ============" << std::endl;
    std::cout << "      ESC - continue game       " << std::endl;
    std::cout << "       H  - return to main menu " << std::endl;
    std::cout << "       I - view instructions    " << std::endl;
    std::cout << "================================" << std::endl;
  }

  while (true) {
    if (loadMode) {
      // In load mode, wait for input is simulated by file source
      if (!inputSource->hasInput()) {
        Sleep(10);
        continue; // Wait for next step to become available (if any)
      }
      if (!silentMode)
        Sleep(1000); // Visual delay to see menu choice
    } else {
      // In keyboard mode, we busy wait or use _kbhit via inputSource
      if (!inputSource->hasInput()) {
        continue;
      }
    }

    // Get key from source (keyboard or file)
    char ch = inputSource->getKey();

    // Record key if saving
    if (stepRecorder.isActive() && !loadMode) {
      stepRecorder.recordStep(gameCycle, ch);
    }

    if (ch == 27) { // ESC -> continue game
      return 0;
    } else if (ch == 'h' || ch == 'H') { // h/H -> go back to main menu
      return 1;
    } else if (ch == 'i' || ch == 'I') { // i/I -> show instructions
      if (silentMode)
        continue;
      cls();
      std::cout << "=============================== Game Instructions "
                   "=============================="
                << std::endl;
      std::cout << "|                                                          "
                   "                    |"
                << std::endl;
      std::cout << "|1.There are two players participating in the game world.  "
                   "                    |"
                << std::endl;
      std::cout << "|2.The goal of each player is to reach the end of the "
                   "level and solve all      |"
                << std::endl;
      std::cout << "|  challenges.                                             "
                   "                    |"
                << std::endl;
      std::cout << "|3.Players can move using the following keys:              "
                   "                    |"
                << std::endl;
      std::cout << "|     Player 1: W (Up), A (Left), S (Stay), D (Right), X "
                   "(Down), E (Dispose)   |"
                << std::endl;
      std::cout << "|     Player 2: I (Up), J (Left), K (Stay), L (Right), M "
                   "(Down), O (Dispose)   |"
                << std::endl;
      std::cout << "|4.You will encounter obstacles, doors, keys, "
                   "springs,bombs,and other game     |"
                << std::endl;
      std::cout << "|  elements.                                               "
                   "                    |"
                << std::endl;
      std::cout << "|                                                          "
                   "                    |"
                << std::endl;
      std::cout << "==========================================================="
                   "===================="
                << std::endl;
      std::cout << "\nPress 'b' to return to pause menu...";

      // Wait for 'b' key to return to pause menu
      // We must record ALL keys pressed here to sync the loop
      while (true) {
        if (loadMode && !inputSource->hasInput()) {
          Sleep(10);
          continue;
        }
        if (!loadMode && !inputSource->hasInput()) {
          continue;
        }

        if (loadMode && !silentMode)
          Sleep(100); // Small delay for visual effect

        char key = inputSource->getKey();

        if (stepRecorder.isActive() && !loadMode) {
          stepRecorder.recordStep(gameCycle, key);
        }

        if (key == 'b' || key == 'B') {
          break;
        }
      }
      // Re-draw pause menu
      cls();
      std::cout << "============ PAUSED ============" << std::endl;
      std::cout << "      ESC - continue game       " << std::endl;
      std::cout << "       H  - return to main menu " << std::endl;
      std::cout << "       I - view instructions    " << std::endl;
      std::cout << "================================" << std::endl;
    }
  }
}

// ============================================================
// Player/collision helpers
// ============================================================

// perplexity AI
bool game::isPlayerAt(int x, int y, const player *ignore) const {
  const player *other = nullptr; // Pointer to the other player

  if (ignore == &p1)
    other = &p2;
  else if (ignore == &p2)
    other = &p1;
  else
    return false; // ignore is not p1 or p2

  return other->getIsActive() && // Other player must be active
         other->getX() == x &&   // Same x
         other->getY() == y;     // Same y
}

bool game::isLastActivePlayer(const player &pl) const {
  int activeCount = 0; // Count how many players are active
  if (p1.getIsActive())
    ++activeCount;
  if (p2.getIsActive())
    ++activeCount;

  return (activeCount == 1) &&
         pl.getIsActive(); // True if only this player is active
}

void game::playerPassedDoor(player &pl) {
  // Hide player from current position on screen
  pl.draw(' ');

  // Record room change
  int playerNum = (&pl == &p1) ? 1 : 2;
  if (resultRecorder.isActive()) {
    resultRecorder.recordRoomChange(gameCycle, playerNum,
                                    roomManager.getCurrentIndex() + 1);
  }
  if (loadMode && resultVerifier.isActive()) {
    resultVerifier.verifyRoomChange(gameCycle, playerNum,
                                    roomManager.getCurrentIndex() + 1);
  }

  ++playersExitedThisRoom; // Increment counter of players who left

  // If first player exited, keep room; play continues with remaining player
  if (playersExitedThisRoom == 1) {
    return;
  }

  // If second player also exited, load next room
  if (playersExitedThisRoom == 2) {
    goToNextRoom();
    playersExitedThisRoom = 0; // Reset counter for the new room
  }
}

// ============================================================
// Explosions
// ============================================================

void game::explodeAt(int x, int y) {
  // First handle visual and board effects
  theScreen.explodeAt(x, y, theScreen.getHudY());

  // Then remove riddles within distance 3 from explosion center
  for (int dy = -3; dy <= 3; ++dy) {
    for (int dx = -3; dx <= 3; ++dx) {
      int nx = x + dx;
      int ny = y + dy;

      if (nx < 0 || nx > Screen::MAX_X || ny < 0 || ny > Screen::MAX_Y)
        continue; // Skip out-of-bounds cells

      for (size_t i = 0; i < riddles.size(); ++i) {
        if (riddles[i].getX() == nx && riddles[i].getY() == ny) {
          riddles.erase(riddles.begin() + i); // Remove riddle in blast radius
          break;
        }
      }
      for (size_t i = 0; i < doors.size(); ++i) {
        if (doors[i].getX() == nx && doors[i].getY() == ny) {
          doors.erase(doors.begin() + i); // Remove door in blast radius
          break;
        }
      }

      for (size_t i = 0; i < obstacles.size(); ++i) {
        if (obstacles[i].contains(nx, ny)) {
          obstacles.erase(obstacles.begin() + i);
          break;
        }
      }
    }
  }
  // Check player damage within EXPLOSION_RADIUS
  // For player 1
  int dx1 = p1.getX() - x;
  int dy1 = p1.getY() - y;
  int life_p1 = p1.getLife();
  if (abs(dx1) + abs(dy1) <= EXPLOSION_RADIUS) {
    p1.setLife(life_p1 - 1);
    p1.updateHUD();
    life_p1 = p1.getLife();

    // Record life lost
    if (resultRecorder.isActive()) {
      resultRecorder.recordLifeLost(gameCycle, 1, life_p1);
    }
    if (loadMode && resultVerifier.isActive()) {
      resultVerifier.verifyLifeLost(gameCycle, 1, life_p1);
    }

    if (life_p1 <= 0)
      p1.deactivate();
  }

  // For player 2
  int dx2 = p2.getX() - x;
  int dy2 = p2.getY() - y;
  int life_p2 = p2.getLife();
  if (abs(dx2) + abs(dy2) <= EXPLOSION_RADIUS) {
    p2.setLife(life_p2 - 1);
    p2.updateHUD();
    life_p2 = p2.getLife();

    // Record life lost
    if (resultRecorder.isActive()) {
      resultRecorder.recordLifeLost(gameCycle, 2, life_p2);
    }
    if (loadMode && resultVerifier.isActive()) {
      resultVerifier.verifyLifeLost(gameCycle, 2, life_p2);
    }

    if (life_p2 <= 0)
      p2.deactivate();
  }
}

// ============================================================
// Wall switch logic
// ============================================================

void game::update_Internal_Wall_By_Switch(const Switch &sw) {
  int y = 15; // Fixed row of internal wall (default)
  const int xStart = WALL_SWITCH_X_START; // Start x of wall segment
  const int xEnd = WALL_SWITCH_X_END;     // End x of wall segment

  if (roomManager.getCurrentIndex() != 0)
    return; // Safety: only in room 0

  for (size_t i = 0; i < doors.size(); ++i) {
    if (doors[i].getSymbol() == '2' && theScreen.getHudY() == 0) {
      y = doors[i].getY() - 2;
      break;
    }
  }
  if (sw.isOn()) {
    // ON -> remove wall (replace with spaces)
    for (int x = xStart; x <= xEnd; ++x) {
      theScreen.setChar(y, x, ' ');
      gotoxy(x, y);
      std::cout << ' ' << std::flush;
    }
  } else {
    // OFF -> restore wall (character 'W')
    for (int x = xStart; x <= xEnd; ++x) {
      theScreen.setChar(y, x, 'W');
      theScreen.drawChar(x, y, 'W', Color::LIGHT_CYAN);
    }
  }
}

// ============================================================
// Other player helpers
// ============================================================

player *game::getOtherPlayer(const player *pl) {
  if (pl == &p1)
    return &p2;
  if (pl == &p2)
    return &p1;
  return nullptr;
}

// ============================================================
// Spring mechanics
// ============================================================

int game::countSpringCompressed(const player &pl) const {
  Direction d = pl.getPoint().getDir();
  int dx = d.dx();
  int dy = d.dy();

  if (dx == 0 && dy == 0)
    return 0;

  int x = pl.getX();
  int y = pl.getY();

  int count = 1;

  while (true) {
    x -= dx;
    y -= dy;

    if (x < 0 || x >= Screen::MAX_X || y < 0 || y >= Screen::MAX_Y)
      break;

    char ch = theScreen.board[y][x];

    if (ch == '#') {
      ++count;
    } else {
      break;
    }
  }

  return count;
}

void game::activateSpringIfNeeded(player &pl) {
  if (pl.isUnderSpringEffect())
    return;

  int x = pl.getX();
  int y = pl.getY();

  // 1. must stand on spring cell
  if (theScreen.board[y][x] != '#')
    return;

  Direction inDir = pl.getPoint().getDir();
  int dx = inDir.dx();
  int dy = inDir.dy();

  if (dx == 0 && dy == 0)
    return; // not moving into spring

  int nextX = x + dx;
  int nextY = y + dy;

  // 2. if next cell is also '#', we are still compressing -> do nothing yet
  if (nextX >= 0 && nextX <= Screen::MAX_X && nextY >= 0 &&
      nextY <= Screen::MAX_Y && theScreen.board[nextY][nextX] == '#') {
    return;
  }
  if (theScreen.board[nextY][nextX] == 'W') {
    int k = countSpringCompressed(
        pl); // Check how many spring cells are behind the player
    if (k <= 0)
      return;

    // release direction = opposite to compression
    int outDx = -dx;
    int outDy = -dy;
    Direction outDir(outDx, outDy);

    int speed = k;
    int ticks = k * k;

    pl.setSpringEffect(speed, ticks, outDir);
    pl.setDirection(outDir);
  }
}

// ============================================================
// Obstacle pushing
// ============================================================

// perplexity AI
bool game::tryPushObstacle(point &target, Direction dir) {
  // Try to find if an obstacle is at the target position
  Obstacle *found = nullptr;
  for (auto &obs : obstacles) {
    if (obs.contains(target.getX(), target.getY())) {
      found = &obs;
      break;
    }
  }
  if (!found)
    return false;

  Obstacle &obs = *found;
  int dx = dir.dx();
  int dy = dir.dy();

  int totalForce = 0;

  // Calculate total force: how many players are pushing?
  auto contributes = [&](player &pl) -> int {
    if (!pl.getIsActive())
      return 0;

    // Check if player is moving in the push direction
    if (pl.getPoint().getDir().dx() != dx || pl.getPoint().getDir().dy() != dy)
      return 0;

    // Check "front" cell (cell in front of player)
    int fx = pl.getX() + dx;
    int fy = pl.getY() + dy;

    // If front cell is not part of this obstacle, player doesn't push it
    if (!obs.contains(fx, fy))
      return 0;

    // Return push force (spring effect increases force)
    return pl.isUnderSpringEffect() ? pl.getSpringSpeed() : 1;
  };

  totalForce += contributes(p1);
  totalForce += contributes(p2);

  // If there is any force
  if (totalForce <= 0)
    return false;

  // If total force is enough and obstacle can be pushed
  if (totalForce >= obs.getSize() && obs.canPush(dir, theScreen)) {
    obs.push(dir, theScreen);
    mergeObstaclesOnBoard();
    return true;
  }

  return false;
}

// perplexity AI
void game::mergeObstaclesOnBoard() {
  // Re-scan the board to merge adjacent obstacle cells
  obstacles.clear();

  // Keep track of visited cells to form clusters
  bool visited[Screen::MAX_Y + 1][Screen::MAX_X + 1] = {false};

  for (int y = 0; y <= Screen::MAX_Y; ++y) {
    for (int x = 0; x <= Screen::MAX_X; ++x) {
      if (theScreen.board[y][x] == '*' && !visited[y][x]) {
        // BFS to find all connected parts
        std::vector<point> cluster;
        std::queue<std::pair<int, int>> q;
        q.push({x, y});
        visited[y][x] = true;

        while (!q.empty()) {
          auto [cx, cy] = q.front();
          q.pop();
          cluster.emplace_back(cx, cy, Direction::directions[Direction::STAY],
                               '*');

          const int dx[4] = {1, -1, 0, 0};
          const int dy[4] = {0, 0, 1, -1};

          for (int k = 0; k < 4; ++k) {
            int nx = cx + dx[k];
            int ny = cy + dy[k];
            if (nx < 0 || nx > Screen::MAX_X || ny < 0 || ny > Screen::MAX_Y)
              continue;
            if (theScreen.board[ny][nx] == '*' && !visited[ny][nx]) {
              visited[ny][nx] = true;
              q.push({nx, ny});
            }
          }
        }

        // Store cluster into a new Obstacle object
        Obstacle newObs(0, 0, theScreen, this); // Create temporary obstacle
        newObs.getCells().clear();
        newObs.setSize(0);

        for (auto &p : cluster) {
          newObs.getCells().push_back(p);
        }
        newObs.setSize((int)cluster.size());

        obstacles.push_back(newObs);
      }
    }
  }
}

// ============================================================
// HUD
// ============================================================

bool game::findLegendPosition() {
  Screen &currentScreen = getCurrentRoomScreen();
  for (int y = 0; y <= Screen::MAX_Y; ++y) {
    for (int x = 0; x <= Screen::MAX_X; ++x) {
      if (currentScreen.board[y][x] == 'L') {
        currentScreen.setHudInfo(x, y, true);
        return true;
      }
    }
  }
  currentScreen.setHudInfo(0, 0, false);
  return false; // No L found
}
void game::findPlayerStartPositions(int &p1x, int &p1y, int &p2x, int &p2y) {
  bool foundP1 = false, foundP2 = false;

  for (int y = 1; y < Screen::MAX_Y && (!foundP1 || !foundP2); ++y) {
    for (int x = 1; x < Screen::MAX_X && (!foundP1 || !foundP2); ++x) {
      char ch = theScreen.board[y][x];
      if (ch == '$' && !foundP1) {
        p1x = x;
        p1y = y;
        foundP1 = true;
        theScreen.setChar(y, x, ' ');
      } else if (ch == '&' && !foundP2) {
        p2x = x;
        p2y = y;
        foundP2 = true;
        theScreen.setChar(y, x, ' ');
      }
    }
  }

  if (!foundP1 || !foundP2) {
    for (int y = 1; y < Screen::MAX_Y && (!foundP1 || !foundP2); ++y) {
      for (int x = 1; x < Screen::MAX_X && (!foundP1 || !foundP2); ++x) {
        if (theScreen.board[y][x] == ' ' && !isHudArea(x, y)) {
          if (!foundP1) {
            p1x = x;
            p1y = y;
            foundP1 = true;
          } else if (!foundP2 && (x != p1x || y != p1y)) {
            p2x = x;
            p2y = y;
            foundP2 = true;
          }
        }
      }
    }
  }
  if (!foundP1) {
    p1x = 2;
    p1y = 2;
  }
  if (!foundP2) {
    p2x = 3;
    p2y = 2;
  }
}

