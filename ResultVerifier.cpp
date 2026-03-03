#include "ResultVerifier.h"
#include <algorithm>
#include <iostream>
#include <sstream>

// Load the result file into memory line-by-line
void ResultVerifier::load(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    testFailed = true;
    failureReason = "Results file not found: " + filename;
    return;
  }

  expectedEvents.clear();
  std::string line;
  while (std::getline(file, line)) {
    if (!line.empty() && line.back() == '\r')
      line.pop_back(); // Fix Windows CR
    if (line.empty())
      continue;

    // Parse cycle from line (ROOM,100,...)
    int cycle = 0;
    size_t firstComma = line.find(',');
    size_t secondComma = line.find(',', firstComma + 1);
    if (firstComma != std::string::npos && secondComma != std::string::npos) {
      try {
        cycle = std::stoi(
            line.substr(firstComma + 1, secondComma - firstComma - 1));
      } catch (...) {
        cycle = 0;
      }
    }

    expectedEvents.push_back({"", cycle, line});
  }

  active = !expectedEvents.empty();
  if (!active) {
    testFailed = true;
    failureReason = "Results file is empty";
  }
}

int ResultVerifier::getNextExpectedCycle() const {
  if (expectedEvents.empty())
    return 999999;

  // If the next event is END, return -1 to signal it's a special final state
  if (expectedEvents.front().data.find("END,") == 0) {
    return -1;
  }

  return expectedEvents.front().cycle;
}

// Helper to verify a generated raw string event
bool ResultVerifier::verifyRawEvent(const std::string &eventStr) {
  if (!active || testFailed)
    return false;

  std::stringstream ss(eventStr);
  std::string segment;

  // Split the generated event into lines (handle newline in riddle)
  while (std::getline(ss, segment)) {
    if (segment.empty())
      continue;
    if (!segment.empty() && segment.back() == '\r')
      segment.pop_back();

    if (expectedEvents.empty()) {
      testFailed = true;
      failureReason = "Game generated extra event: " + segment;
      return false;
    }

    const auto &expected = expectedEvents.front();
    if (expected.data != segment) {
      testFailed = true;
      failureReason =
          "Mismatch!\nExpected: " + expected.data + "\nActual:   " + segment;
      return false;
    }

    expectedEvents.pop_front();
  }
  return true;
}

bool ResultVerifier::verifyRoomChange(int cycle, int playerNum, int newRoom) {
  std::stringstream ss;
  ss << "ROOM," << cycle << ",P" << playerNum << "," << newRoom;
  return verifyRawEvent(ss.str());
}

bool ResultVerifier::verifyLifeLost(int cycle, int playerNum, int livesLeft) {
  std::stringstream ss;
  ss << "LIFE," << cycle << ",P" << playerNum << "," << livesLeft;
  return verifyRawEvent(ss.str());
}

bool ResultVerifier::verifyRiddle(int cycle, int playerNum,
                                  const std::string &question,
                                  const std::string &answer, bool correct) {
  std::stringstream ss;
  ss << "RIDDLE," << cycle << ",P" << playerNum << ","
     << (correct ? "CORRECT" : "WRONG") << "," << question << "," << answer;
  return verifyRawEvent(ss.str());
}

bool ResultVerifier::verifyGameEnd(int cycle, int finalScore) {
  std::stringstream ss;
  ss << "END," << cycle << ",SCORE," << finalScore;
  return verifyRawEvent(ss.str());
}
