# Tetris Game  

A simple **Tetris** game written in **C++**, playable on the **macOS terminal** without any external libraries. Rotate, move, and drop tetrominoes to clear lines and score points!  

## Features:  
- **Classic Tetris Gameplay** – Rotate and move pieces to form complete lines.  
- **Line Clearing** – Complete lines disappear, increasing your score.  
- **Increasing Difficulty** – The game speeds up as you progress.  
- **High Score Tracking** – Keeps track of your highest score.  
- **Simple Controls** – Move, rotate, and drop pieces easily.  
- **Exit Anytime** – Press `Q` to quit the game.  

## How to Run:  
1. **Download** the C++ source file (`tetris.cpp`).  
2. **Compile** the code using a C++ compiler:  
   ```sh
   g++ -std=c++11 tetris.cpp -o tetris
   ```  
3. **Run the game** in the terminal:  
   ```sh
   ./tetris
   ```  

## Controls:  
- **A** – Move Left  
- **D** – Move Right  
- **W** – Rotate Tetromino  
- **S** – Fast Drop  
- **Q** – Quit the Game  

## How to Play:  
1. **Run the game** and start with a random tetromino.  
2. **Move and rotate** the falling pieces to complete horizontal lines.  
3. **Clear lines** to gain points and level up.  
4. The **speed increases** as you progress.  
5. The game ends if the tetrominoes stack to the top of the screen.  

## Libraries Used:  
The game uses standard C++ libraries:  
1. **`#include <iostream>`** – For input and output operations.  
2. **`#include <vector>`** – Used to handle game board and tetrominoes.  
3. **`#include <random>`** – Generates random tetrominoes.  
4. **`#include <chrono>`** – Controls game timing and speed.  
5. **`#include <thread>`** – Used for delays and smooth movement.  
6. **`#include <termios.h>`** – Enables real-time keyboard input.  
7. **`#include <unistd.h>`** – Used for non-blocking input.  

