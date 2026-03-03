#pragma once
#include <fstream>
#include <string>
#include <vector>

// Records game steps to file
class StepRecorder {
private:
  std::ofstream file;
  bool active = false;

public:
  // Start recording to file
  void start(const std::string &filename,
             const std::vector<std::string> &screenFiles, bool enableColor);

  // Record a single step
  void recordStep(int cycle, char key);

  // Stop recording
  void stop();

  // Check if recording
  bool isActive() const { return active; }
};
