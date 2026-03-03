#include "FileInputSource.h"
#include <fstream>
#include <sstream>

// Load steps from file
FileInputSource::FileInputSource(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    valid = false;
    return;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    // Check for headers
    if (line.find("COLORS:") == 0) {
      enableColor = (line.substr(7, 1) == "1");
      continue;
    }
    if (line.find("SCREENS:") == 0) {
      screensHeader = line;
      continue;
    }

    // Parse steps: "cycle,key"
    size_t commaPos = line.find(',');
    if (commaPos != std::string::npos) {
      int cycle = std::stoi(line.substr(0, commaPos));
      char key = ' ';
      if (commaPos + 1 < line.length()) {
        key = line[commaPos + 1];
      }
      steps.push_back({cycle, key});
    }
  }

  valid = true;
}

// Check if next step is ready
bool FileInputSource::hasInput() {
  // Skip consumed steps
  size_t tempIndex = nextStepIndex;
  while (tempIndex < steps.size() && steps[tempIndex].consumed) {
    tempIndex++;
  }

  if (tempIndex >= steps.size())
    return false;
  return steps[tempIndex].cycle <= currentCycle;
}

// Get next step key
char FileInputSource::getKey() {
  // Skip consumed steps
  while (nextStepIndex < steps.size() && steps[nextStepIndex].consumed) {
    nextStepIndex++;
  }

  if (nextStepIndex >= steps.size())
    return 0;
  return steps[nextStepIndex++].key;
}

// Search for and retrieve a riddle answer from the input stream
char FileInputSource::getRiddleAnswer() {
  // Scan forward from current position
  for (size_t i = nextStepIndex; i < steps.size(); ++i) {
    if (!steps[i].consumed) {
      char k = steps[i].key;
      // Check if it's a valid riddle answer (0-4)
      // 0 is recorded for wrong/timeout, 1-4 for answers
      if (k >= '0' && k <= '4') {
        steps[i].consumed = true;
        return k;
      }
    }
  }
  return 0; // Default/Not found
}

// Track current game cycle
void FileInputSource::onCycle(int cycle) { currentCycle = cycle; }

// Check if file was loaded and has more steps
bool FileInputSource::isValid() const {
  return valid && (nextStepIndex < steps.size());
}
