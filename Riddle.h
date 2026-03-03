#pragma once

#include <string>
#include <vector>

struct RiddleData {
  std::string question;   // Full text of the riddle question
  std::string options[4]; // Four possible answers to choose from
  int correctIndex;       // Index (0-3) of the correct answer
};

class Riddle {
  int x, y;                       // Position of the riddle trigger on the board
  std::string question;           // Stored question text for this riddle
  std::vector<std::string> lines; // Stored answer options for this riddle
                                  // (changed from array to vector)
  int correctIndex;               // Index of the correct answer
  char lastChoice = '0';          // Store last choice

public:
  Riddle(int x, int y,
         const RiddleData &data) // Constructor: build riddle from RiddleData
      : x(x), y(y), question(data.question), correctIndex(data.correctIndex) {
    lines.reserve(4); // Pre-allocate space for 4 options
    for (int i = 0; i < 4; ++i)
      lines.push_back(data.options[i]); // Copy all answer options
  }
  int getX() const { return x; } // Get riddle x-coordinate
  int getY() const { return y; } // Get riddle y-coordinate
  const std::string &getQuestion() const {
    return question;
  } // Get question text
  const std::string &getAnswer() const {
    return lines[correctIndex];
  } // Get correct answer
  int getCorrectIndex() const {
    return correctIndex;
  } // Get correct answer index
  // Display riddle on screen and wait for user choice (numeric 1-4)
  bool
  ask(bool silent = false); // Show riddle to player and return true if correct
  // Auto-answer for load mode (no user input needed)
  bool askWithAnswer(char answer,
                     bool silent = false);          // Auto-answer for load mode
  char getLastChoice() const { return lastChoice; } // Get last user choice
};
