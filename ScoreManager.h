#pragma once

// Score constants
constexpr int SCORE_RIDDLE_CORRECT = 50; // Points for correct riddle
constexpr int SCORE_RIDDLE_WRONG = 30;   // Points lost for wrong riddle
constexpr int SCORE_ROOM_FAST = 200;     // Bonus for finishing room quickly
constexpr int SCORE_ROOM_NORMAL = 50;    // Standard room completion score
constexpr int SCORE_FULL_LIFE_BONUS =
    500; // Bonus when both players have full life

class ScoreManager {
  int score = 0;

public:
  int getScore() const { return score; }
  void setScore(int s) { score = s; }
  void addPoints(int pts) { score += pts; }
  void subtractPoints(int pts) { score -= pts; }
  void reset() { score = 0; }
};
