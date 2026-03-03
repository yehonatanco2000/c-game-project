#include "ResultRecorder.h"

// Start recording to file
void ResultRecorder::start(const std::string &filename) {
  file.open(filename);
  active = file.is_open();
}

// Record player room change
void ResultRecorder::recordRoomChange(int cycle, int playerNum, int newRoom) {
  if (!active)
    return;
  file << "ROOM," << cycle << ",P" << playerNum << "," << newRoom << "\n";
}

// Record player life lost
void ResultRecorder::recordLifeLost(int cycle, int playerNum, int livesLeft) {
  if (!active)
    return;
  file << "LIFE," << cycle << ",P" << playerNum << "," << livesLeft << "\n";
}

// Record riddle attempt
void ResultRecorder::recordRiddle(int cycle, int playerNum,
                                  const std::string &question,
                                  const std::string &answer, bool correct) {
  if (!active)
    return;
  file << "RIDDLE," << cycle << ",P" << playerNum << ","
       << (correct ? "CORRECT" : "WRONG") << "," << question << "," << answer
       << "\n";
}

// Record game end with score
void ResultRecorder::recordGameEnd(int cycle, int finalScore) {
  if (!active)
    return;
  file << "END," << cycle << ",SCORE," << finalScore << "\n";
}

// Stop recording and close file
void ResultRecorder::stop() {
  if (active) {
    file.close();
    active = false;
  }
}
