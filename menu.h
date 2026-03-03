#pragma once

class Menu {
public:
  void show_menu(); // Display the main menu and handle all choices in a loop
private:
  int waitForMenuInput();   // Read and return the user's menu choice
  void show_instructions(); // Show game instructions screen
};
