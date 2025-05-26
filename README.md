# CH3AT_Linux

**CH3AT_Linux** is a **Skate 3** trainer for **Linux** using **RPCS3**. This version is inspired by the original **CH3AT** for **Windows** created by [ckosmic](https://github.com/ckosmic/CH3AT), and replicates similar functionality by utilizing a **Cheat Engine** table to modify various game values.

## Features

- Works with **Skate 3** running on **RPCS3**.
- Compatible with **Cheat Engine tables** (including the provided `Skate 3.CT`).
- Allows modifying in-game values in real-time via an **OpenGL**-based interface using **GLFW**.

### Supported Value Types:
- Byte
- 2 Byte Big Endian
- 4 Byte Big Endian
- Float Big Endian
- String

### Known Limitations:
- Only the mentioned value types are supported.
- Sub-cheats are currently ignored.

## Usage
1. Launch RPCS3 and load Skate 3.
2. After the game is running, open a terminal and navigate to the project directory.
3. Run the following command to launch the trainer:
 ./CH3AT
  The CH3AT trainer menu will appear in a separate window, allowing you to modify values from the provided Cheat Engine table.
  You can scroll through the available values and change them as desired when clicking on the "Patch" button.
  To restore a default value, press the Reset button next to the value you want to reset.

## Building

1. Clone this repository:
   git clone https://github.com/irolup/CH3AT_Linux.git
   cd CH3AT_Linux
2. Install the necessary dependencies (tested with GLFW 3.4) :
   sudo apt-get install libglfw3-dev libglew-dev
4. Build the project: After installing the dependencies, simply run:
   make
5. Once the build is complete, run the program (as ROOT if not wokring as non-root):
