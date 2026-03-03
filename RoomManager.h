#pragma once
#include "Riddle.h"
#include "Screen.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class RoomManager {
  std::vector<Screen> allRooms;
  std::vector<std::string> screenFileNames;
  std::vector<std::vector<std::pair<char, int>>> doorKeyOverrides;
  std::vector<RiddleData> all_riddles;
  int currentRoom = 0;
  int nextRiddleIndex = 0;
  bool useFileRooms = false;

public:
  // --- Loading ---
  bool loadScreensFromFiles(std::string &errorMsg);
  bool loadRiddlesFromFile(const std::string &filename);

  // --- Room access ---
  int getCurrentIndex() const { return currentRoom; }
  void setCurrentIndex(int idx) { currentRoom = idx; }
  void advanceRoom() { ++currentRoom; }
  bool isLastRoom() const { return currentRoom >= (int)allRooms.size() - 1; }
  bool isLoaded() const { return useFileRooms; }
  int getRoomCount() const { return (int)allRooms.size(); }

  // Get a room by index
  Screen &getRoom(int idx) { return allRooms[idx]; }
  const Screen &getRoom(int idx) const { return allRooms[idx]; }

  // Copy current room to the game screen
  void copyCurrentRoomTo(Screen &target) const {
    if (currentRoom < (int)allRooms.size())
      target.copyFrom(allRooms[currentRoom]);
  }

  // --- Riddles ---
  bool hasRiddles() const { return !all_riddles.empty(); }
  RiddleData getNextRiddle() {
    if (all_riddles.empty())
      return RiddleData();
    int idx = nextRiddleIndex % all_riddles.size();
    ++nextRiddleIndex;
    return all_riddles[idx];
  }
  const std::vector<RiddleData> &getAllRiddles() const { return all_riddles; }
  int getNextRiddleIndex() const { return nextRiddleIndex; }

  // --- Door key overrides ---
  const std::vector<std::pair<char, int>> &
  getDoorKeyOverrides(int roomIdx) const {
    static const std::vector<std::pair<char, int>> empty;
    if (roomIdx < 0 || roomIdx >= (int)doorKeyOverrides.size())
      return empty;
    return doorKeyOverrides[roomIdx];
  }

  // --- File names ---
  const std::vector<std::string> &getFileNames() const {
    return screenFileNames;
  }

private:
  bool loadSingleScreen(const std::string &filename, int roomIndex);
};
