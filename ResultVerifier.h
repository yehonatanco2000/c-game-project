#pragma once
#include <deque>
#include <fstream>
#include <string>
#include <vector>

// Structure to hold a parsed event from the result file
struct ExpectedEvent {
  std::string type; // ROOM, LIFE, RIDDLE, END
  int cycle;        // When it should happen
  std::string data; // Provide raw data string for comparison
};

class ResultVerifier {
private:
  std::deque<ExpectedEvent> expectedEvents;
  bool active = false;
  bool testFailed = false;
  std::string failureReason;

  // Helper to verify a generated raw string event against the next expected
  // lines
  bool verifyRawEvent(const std::string &eventStr);

public:
  // Load expected results from file
  void load(const std::string &filename);

  // Verify events (returns true if match, false if mismatch)
  bool verifyRoomChange(int cycle, int playerNum, int newRoom);
  bool verifyLifeLost(int cycle, int playerNum, int livesLeft);
  bool verifyRiddle(int cycle, int playerNum, const std::string &question,
                    const std::string &answer, bool correct);
  bool verifyGameEnd(int cycle, int finalScore);

  // Final status check
  bool isPassed() const { return !testFailed; }
  std::string getFailureReason() const { return failureReason; }
  bool isActive() const { return active; }

  // Check if all expected events were consumed
  bool allEventsConsumed() { return expectedEvents.empty(); }

  // Get the cycle of the next expected event (for timeout)
  int getNextExpectedCycle() const;
};
