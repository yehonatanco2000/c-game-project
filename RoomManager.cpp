#include "RoomManager.h"

bool RoomManager::loadSingleScreen(const std::string &filename, int roomIndex) {
  std::ifstream in(filename);
  if (!in) {
    return false;
  }
  Screen room;

  std::string line;
  for (int y = 0; y <= Screen::MAX_Y; ++y) {
    if (!std::getline(in, line)) {
      line = "";
    }

    if ((int)line.size() < Screen::MAX_X)
      line.append(Screen::MAX_X - line.size(), ' ');
    else if ((int)line.size() > Screen::MAX_X)
      line = line.substr(0, Screen::MAX_X);

    for (int x = 0; x <= Screen::MAX_X; ++x) {
      char ch = line[x];

      if (ch == 'L') {
        room.setHudInfo(x, y, true);
        ch = ' ';
      }

      if (ch == 'D') {
        if (roomIndex == 0) {
          const int DARK_WIDTH = 10;
          const int DARK_HEIGHT = 10;
          room.markDarkRect(x, y, DARK_WIDTH, DARK_HEIGHT);
          ch = ' ';
        } else if (roomIndex == 2) {
          const int DARK_WIDTH = 15;
          const int DARK_HEIGHT = 7;
          room.markDarkRect(x, y, DARK_WIDTH, DARK_HEIGHT);
          ch = ' ';
        }
      }

      room.board[y][x] = ch;
    }
  }

  // Read configuration lines after the board
  while (std::getline(in, line)) {
    if (!line.empty() && line.back() == '\r')
      line.pop_back();
    if (line.empty())
      continue;
    if (line.rfind("KEYS:", 0) == 0 && line.size() >= 8) {
      size_t eq = line.find('=');
      if (eq != std::string::npos && eq > 5) {
        doorKeyOverrides[roomIndex].push_back(
            {line[5], std::stoi(line.substr(eq + 1))});
      }
    }
  }

  if ((int)allRooms.size() <= roomIndex)
    allRooms.resize(roomIndex + 1);

  allRooms[roomIndex] = room;
  return true;
}

bool RoomManager::loadScreensFromFiles(std::string &errorMsg) {
  allRooms.clear();
  screenFileNames.clear();
  doorKeyOverrides.clear();
  doorKeyOverrides.resize(100);

  for (int i = 1; i <= 99; ++i) {
    std::string filename = (i < 10)
                               ? "adv-world0" + std::to_string(i) + ".screen"
                               : "adv-world" + std::to_string(i) + ".screen";

    std::ifstream test(filename);
    if (!test)
      break;
    test.close();

    if (!loadSingleScreen(filename, i - 1)) {
      errorMsg = "Failed to load: " + filename;
      return false;
    }
    screenFileNames.push_back(filename);
  }

  if (screenFileNames.empty()) {
    errorMsg = "No screen files found.";
    return false;
  }
  return true;
}

bool RoomManager::loadRiddlesFromFile(const std::string &filename) {
  std::ifstream in(filename);
  if (!in) {
    return false;
  }

  all_riddles.clear();
  std::string line;
  RiddleData currentRiddle;
  int optionIndex = 0;
  bool inRiddle = false;

  while (std::getline(in, line)) {
    if (line.empty())
      continue;

    if (!line.empty() && line.back() == '\r')
      line.pop_back();

    if (line == "[RIDDLE]") {
      if (inRiddle) {
        all_riddles.push_back(currentRiddle);
      }
      currentRiddle = RiddleData();
      currentRiddle.correctIndex = 0;
      optionIndex = 0;
      inRiddle = true;
    } else if (inRiddle) {
      if (line[0] == '*') {
        if (optionIndex < 4) {
          currentRiddle.options[optionIndex] = line.substr(1);
          optionIndex++;
        }
      } else if (line.rfind("Correct:", 0) == 0) {
        int val = std::stoi(line.substr(8));
        currentRiddle.correctIndex = val - 1;
      } else {
        if (!currentRiddle.question.empty()) {
          currentRiddle.question += "\n";
        }
        currentRiddle.question += line;
      }
    }
  }
  if (inRiddle) {
    all_riddles.push_back(currentRiddle);
  }

  return !all_riddles.empty();
}
