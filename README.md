# C++ Adventure Game (MTA 2026A)

A terminal-based puzzle adventure game developed in C++.

## Overview

This project is a two-player console game where players navigate through various rooms, solving coding-related riddles, avoiding obstacles, and collecting items to progress. The game features a custom level format, save/load functionality, and a replay verification system.

## Features

- **Multiplayer Support**: Two players can play simultaneously on the same keyboard.
- **Interactive Objects**:
  - **Keys & Doors**: Collect keys to unlock doors matching the key's ID.
  - **Riddles**: Solve C++ programming riddles to unlock paths or bonuses.
  - **Bombs**: Clear obstacles or enemies (timer-based).
  - **Springs**: Jump over walls or cover large distances.
  - **Torches**: Light up dark areas.
- **Level Loading**: Levels are loaded from `.screen` files, allowing for custom map creation.
- **Save & Load**:
  - Save your progress and resume later.
  - `-save` and `-load` command-line arguments.
- **Replay System**:
  - **Silent Mode**: Run the game without visual output for testing (`-silent`).
  - **Step Verification**: Records player inputs and verifies them against expected results.

## Controls

### Player 1 (`$`)
- **W**: Move Up
- **X**: Move Down
- **A**: Move Left
- **D**: Move Right
- **S**: Stay (Skip turn)
- **E**: Interact / Drop Item

### Player 2 (`&`)
- **I**: Move Up
- **M**: Move Down
- **J**: Move Left
- **L**: Move Right
- **K**: Stay (Skip turn)
- **O**: Interact / Drop Item

## How to Build

1. Open the solution file `c++ game project.sln` in **Visual Studio**.
2. Select the **Release** or **Debug** configuration.
3. Build the solution (**Ctrl+Shift+B**).

## How to Run

Run the generated executable from the command line:

```bash
# Normal mode (Standard gameplay)
"c++ game project.exe"

# Save mode (Starts with recording enabled)
"c++ game project.exe" -save

# Load mode (Plays back a recorded session)
"c++ game project.exe" -load

# Silent mode (Runs verification without visual output)
"c++ game project.exe" -load -silent
```

## Credits

Created as part of the **Advanced C++ Programming** course (MTA 2026A).
