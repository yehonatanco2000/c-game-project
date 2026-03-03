#pragma once
#include <fstream>
#include <string>

// Records game results to file
class ResultRecorder {
private:
  std::ofstream file;
  bool active = false;

public:
  // Start recording results
  void start(const std::string &filename);

  // Record room change
  void recordRoomChange(int cycle, int playerNum, int newRoom);

  // Record life lost
  void recordLifeLost(int cycle, int playerNum, int livesLeft);

  // Record riddle attempt
  void recordRiddle(int cycle, int playerNum, const std::string &question,
                    const std::string &answer, bool correct);

  // Record game end
  void recordGameEnd(int cycle, int finalScore);

  // Stop recording
  void stop();

  // Check if recording
  bool isActive() const { return active; }
};
