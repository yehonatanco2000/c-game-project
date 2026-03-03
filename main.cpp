
#include "FileInputSource.h"
#include "KeyboardInputSource.h"
#include "Menu.h"
#include "game.h"
#include <cstring>
#include <iostream>
#include <memory>

// Global flags for game mode (accessed by Menu)
bool g_saveMode = false;
bool g_loadMode = false;
bool g_silentMode = false;

// Parse command line and run game
int main(int argc, char *argv[]) {
  // Parse command line arguments
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-load") == 0) {
      g_loadMode = true;
    } else if (strcmp(argv[i], "-save") == 0) {
      g_saveMode = true;
    } else if (strcmp(argv[i], "-silent") == 0) {
      g_silentMode = true;
    }
  }

  // -load mode: run game from file, no menu
  if (g_loadMode) {
    game g;

    // Create file input source
    auto fileInput = std::make_unique<FileInputSource>("adv-world.steps");
    if (!fileInput->isValid()) {
      std::cout << "ERROR: Could not load adv-world.steps" << std::endl;
      return 1;
    }

    g.setInputSource(std::move(fileInput));
    g.setLoadMode(true);
    g.setSilentMode(g_silentMode);
    g.start_game();
    return 0;
  }

  // -save mode or normal: show menu (save flag applied when game starts)
  Menu menu;
  menu.show_menu();
  return 0;
}