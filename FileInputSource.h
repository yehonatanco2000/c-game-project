#pragma once
#include "GameInputSource.h"
#include <string>
#include <vector>

// Single recorded step
struct GameStep {
  int cycle;             // Game cycle when key was pressed
  char key;              // The key that was pressed
  bool consumed = false; // Has this step been processed?
};

// File input for replay mode
class FileInputSource : public GameInputSource {
private:
  std::vector<GameStep> steps; // All loaded steps
  size_t nextStepIndex = 0;    // Next step to process
  int currentCycle = 0;        // Current game cycle
  bool valid = false;          // File loaded successfully?
  bool enableColor = false;    // Saved color setting

public:
  // Load steps from file
  FileInputSource(const std::string &filename);

  bool hasInput() override;
  char getKey() override;
  void onCycle(int cycle) override;
  bool isValid() const override;
  char getRiddleAnswer() override;
  bool getSavedColorMode() const override { return enableColor; }

  // Get loaded screen files header
  std::string getScreensHeader() const { return screensHeader; }

private:
  std::string screensHeader; // Screen files from header
};
