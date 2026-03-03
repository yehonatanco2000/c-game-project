# C++ Console Adventure Game

## Overview
A colorful, terminal-based puzzle adventure game developed in C++. This project features a custom level parsing system, robust save/load functionality, and an advanced replay verification architecture, all running directly within the command-line console.

## Technical Architecture & Methodologies
* **Language & Core:** C++17, Standard Template Library (STL).
* **Design Principles:** Strict adherence to Object-Oriented Programming (OOP) concepts, including Polymorphism and Encapsulation.
* **Console UI:** Implemented colored terminal output to enhance user experience and visually distinguish game entities, utilizing console-specific output manipulation.
* **Architecture:** Developed a modular system separating game logic, rendering, and file I/O operations. 
* **Data Management:** Implemented dynamic parsing for custom `.screen` level files and structured data handling for saving/loading game states.

## Key Game Features
* **Multiplayer Support:** Two players can play simultaneously on the same keyboard.
* **Interactive Colored Environment:** Includes dynamic entities such as riddles, timing-based bombs, springs, and keys/doors with matching ID mechanisms, all visually enhanced with console colors.
* **Advanced Replay System:** Records player inputs step-by-step for testing and verification, including a "Silent Mode" for running automated tests without visual output.

## Controls
### Player 1 (`$`)
* **W/A/S/D:** Move (Up/Left/Stay/Right)
* **X:** Move Down
* **E:** Interact / Drop Item

### Player 2 (`&`)
* **I/J/K/L:** Move (Up/Left/Stay/Right)
* **M:** Move Down
* **O:** Interact / Drop Item

## How to Build & Run (Visual Studio)
Ensure all data files (`.screen`, `record`, `step`) are in the same directory as your source code.

1. Create an empty C++ project in Visual Studio and add all `.cpp` and `.h` files.
2. Build the project (`Ctrl + Shift + B`).
3. To play the standard game, simply Run the project (`F5` or `Ctrl + F5`) without any arguments.

### Running Special Modes (Save / Load / Silent)
To run the game with specific command-line arguments in Visual Studio:
1. Go to **Project** > **Properties** (or right-click your project in the Solution Explorer and select Properties).
2. Navigate to **Configuration Properties** > **Debugging**.
3. In the **Command Arguments** field, enter one of the following:
   * `-save` (Starts the game and records your inputs)
   * `-load` (Plays back a previously recorded session)
   * `-load -silent` (Runs the verification process in the background without opening the graphical console)
4. Click **OK** and Run the project.


## Credits & Tools
Parts of the file I/O serialization and replay architecture were generated and integrated with the assistance of Gen-AI tools (Antigravity / Copilot).
