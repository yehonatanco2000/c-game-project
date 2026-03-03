
#include "Menu.h"
#include "KeyboardInputSource.h"
#include "game.h"
#include "utils.h"
#include <conio.h>
#include <iostream>
#include <memory>

// Access global save mode flag from main.cpp
extern bool g_saveMode;

const enum options {
  start = 1,
  back = 4,
  instruction = 8,
  Exit = 9,
  toggleColor = 7
};

void Menu::show_menu() {
  while (true) { // Loop instead of recursion
    Screen &s = getScreen();
    // 01234567890123456789012345678901234567890123456789012345678901234567890123456789//
    std::cout << "           ========================    MENU    "
                 "=======================                    "
              << std::endl;
    std::cout
        << "           |                                                   "
           "      |                    "
        << std::endl;
    std::cout
        << "           |                                                   "
           "      |                    "
        << std::endl;
    std::cout
        << "           |                    1 - Start a new game           "
           "      |                    "
        << std::endl;
    std::cout
        << "           |                                                   "
           "      |                    "
        << std::endl;
    std::cout << "           |                    7 - Colors (ON/OFF):"
              << (s.enableColor ? "ON " : "OFF") << "              |"
              << std::endl;
    std::cout
        << "           |                                                   "
           "      |                    "
        << std::endl;
    std::cout << "           |                    8 - Present instruction and "
                 "keys     |                    "
              << std::endl;
    std::cout
        << "           |                                                   "
           "      |                    "
        << std::endl;
    std::cout
        << "           |                    9 - EXIT                       "
           "      |                    "
        << std::endl;
    std::cout
        << "           |                                                   "
           "      |                    "
        << std::endl;
    std::cout
        << "           "
           "===========================================================    "
           "                "
        << std::endl;

    int choice = waitForMenuInput();

    switch (choice) {
    case options::start: {
      cls();
      game play;
      play.setInputSource(std::make_unique<KeyboardInputSource>());
      if (g_saveMode) {
        play.setSaveMode(true);
      }
      play.start_game();
      cls(); // Clear after game ends, redraw menu
      break;
    }
    case options::instruction:
      cls();
      show_instructions(); // Shows instructions, waits for 'b', returns here
      cls();               // Redraw menu after returning
      break;
    case options::toggleColor: {
      Screen &scr = getScreen();
      scr.enableColor = !scr.enableColor;
      cls(); // Redraw menu with updated color status
      break;
    }
    case options::Exit:
      cls();
      return; // Exit the loop and end program
    default:
      break;
    }
  }
}

int Menu::waitForMenuInput() {
  while (true) {
    int ch = _getch(); // Read a single key from the user (no Enter needed)

    switch (ch) {
    case '1':
      return 1; // Start a new game
    case '7':
      return 7; // Toggle colors ON/OFF
    case '8':
      return 8; // Show instructions
    case '9':
      return 9; // Exit game
    default:
      break; // Ignore any other key and wait again
    }
  }
}

void Menu::show_instructions() {
  // 01234567890123456789012345678901234567890123456789012345678901234567890123456789//
  std::cout << "=============================== Game Instructions "
               "=============================="
            << std::endl;
  std::cout << "|                                                              "
               "                |"
            << std::endl;
  std::cout << "|1.There are two players participating in the game world.      "
               "                |"
            << std::endl;
  std::cout << "|2.The goal of each player is to reach the end of the level "
               "and solve all      |"
            << std::endl;
  std::cout << "|  challenges.                                                 "
               "                |"
            << std::endl;
  std::cout << "|3.Players can move using the following keys:                  "
               "                |"
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
  std::cout << "|  elements.                                                   "
               "                |"
            << std::endl;
  std::cout << "|5.Collaboration is required to overcome obstacles and open "
               "new paths.         |"
            << std::endl;
  std::cout << "|6.Good luck!                                                  "
               "                |"
            << std::endl;
  std::cout << "|                                                              "
               "                |"
            << std::endl;
  std::cout << "| *for back press b*                                           "
               "                |"
            << std::endl;
  std::cout << "==============================================================="
               "================="
            << std::endl;

  while (true) {
    int ch = _getch();
    if (ch == 'b') {
      return; // Just return — the loop in show_menu() redraws the menu
    }
  }
}
