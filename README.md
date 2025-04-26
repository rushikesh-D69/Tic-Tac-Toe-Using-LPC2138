# Tic Tac Toe on LPC2138 with 7-Segment Displays

This project implements a Tic Tac Toe (X-O) game on the LPC2138 ARM7 microcontroller, using 9 shift-register-controlled 7-segment displays and tactile buttons for player input.

## Features

- Two-player local gameplay
- Displays moves using 7-segment displays (H for Player 1, 0 for Player 2)
- Win and draw detection logic
- Button-based input with debounce handling
- Flashing display pattern to indicate a draw
- Efficient pin usage through shift register communication

## System Overview

- Each 7-segment display is controlled via a shift register using GPIO pins (data, clock, and master reset).
- Players press hardware buttons to mark their moves.
- Game logic checks for winning combinations or a draw after each move.
- A reset button clears the game state.

## Hardware Requirements

- LPC2138 Development Board
- 9 x 7-Segment Displays
- 9 x Shift Registers (e.g., 74HC595)
- 10 x Push Buttons
- Breadboard, jumper wires, and power supply

## Software Requirements

- Keil µVision or any ARM-supported IDE
- Proteus (for simulation, optional)
- C Compiler for ARM7

## File Structure

- `src/` – Contains the main C source code
- `project/` – Project files for Keil or Proteus
- `assets/` – Images, schematics, or diagrams (optional)

## Getting Started

1. Open the `.pdsprj` or `.uvproj` file in your development environment.
2. Set up the hardware according to the schematic.
3. Compile and flash the code to the LPC2148 board.
4. Interact with the buttons to play the game.

## Authors

- Rushikesh D. – ECE Department, ASEB
- Chandan Sai Pavan – ECE Department, ASEB


