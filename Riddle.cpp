#include "Riddle.h"
#include "utils.h"
#include <conio.h>
#include <iostream>

bool Riddle::ask(bool silent) {
  if (silent)
    return false; // Should not be called in silent mode for user input
  cls();
  // 01234567890123456789012345678901234567890123456789012345678901234567890123456789//
  std::cout << "==================================== Riddle "
               "====================================\n"
            << std::endl;
  // Display the riddle question and possible answers
  std::cout << question << "\n\n";
  for (int i = 0; i < 4; ++i) {
    std::cout << "   " << (i + 1) << " - " << lines[i] << "\n";
  }
  std::cout << "==============================================================="
               "=================\n";

  int choice_int = 0;   // Renamed to avoid conflict with char choice
  char choice_char = 0; // Declare char for input
  while (true) {
    // std::cout << "Choose your answer (1-4): " << std::flush;
    if (_kbhit()) {
      choice_char = _getch();
      // Only accept valid digits 1-4 as answers
      if (choice_char >= '1' && choice_char <= '4') {
        choice_int = choice_char - '1'; // Convert char to int index
        break;
      }
    }
  }

  lastChoice = choice_char; // Store the user's choice (CHAR '1'-'4')
  return (choice_int == correctIndex);
}

// Auto-answer for load mode (no user input needed)
bool Riddle::askWithAnswer(char answer, bool silent) {
  if (!silent) {
    cls();
    std::cout << "==================================== Riddle "
                 "====================================\n"
              << std::endl;

    std::cout << question << "\n\n";
    for (int i = 0; i < 4; ++i) {
      std::cout << "   " << (i + 1) << " - " << lines[i] << "\n";
    }
    std::cout
        << "==============================================================="
           "=================\n";
    std::cout << "Auto-answer: " << answer << std::endl;
  }

  if (answer >= '1' && answer <= '4') {
    int choice = answer - '1';
    return (choice == correctIndex);
  }
  return false;
}
