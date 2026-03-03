#include "StepRecorder.h"

// Start recording to file
void StepRecorder::start(const std::string &filename,
                         const std::vector<std::string> &screenFiles,
                         bool enableColor) {
  file.open(filename);
  if (!file.is_open())
    return;

  // Write color setting
  file << "COLORS:" << (enableColor ? "1" : "0") << "\n";

  // Write header with screen files
  file << "SCREENS:";
  for (const auto &sf : screenFiles) {
    file << sf << ";";
  }
  file << "\n";

  active = true;
}

// Record a step: cycle,key
void StepRecorder::recordStep(int cycle, char key) {
  if (!active)
    return;
  file << cycle << "," << key << std::endl;
}

// Stop recording and close file
void StepRecorder::stop() {
  if (active) {
    file.close();
    active = false;
  }
}
