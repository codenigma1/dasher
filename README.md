# 🏃‍♂️ Dapper Dasher - A 2D Side-Scrolling Endless Runner in C++

A fun, beginner-friendly 2D platformer made using C++ and [Raylib](https://www.raylib.com/) — inspired by retro arcade games. You dodge enemies, collect score, survive levels, and activate a shield to escape danger! ⚔️

![Game Demo](media/demo.gif) <!-- Replace this with your actual gif path -->

---

## 🎮 Features

- 🧠 Side-scrolling enemy AI with increasing difficulty
- 🧍 Player animation with jump and gravity physics
- 🛡 Temporary shield activation (flashing player + countdown)
- 📈 Score tracking and saved high score with file I/O
- 👹 Boss level at stage 10 with red-tinted nebula
- 🔊 Jump, win, and lose sound effects
- 🎵 Background music playback
- 🔁 Restart logic and responsive controls

---

## 🔧 Installation

### Option 1: Using `vcpkg` (recommended for beginners)

```bash
# Install vcpkg (if not already)
git clone https://github.com/microsoft/vcpkg
cd vcpkg
./bootstrap-vcpkg.sh  # or .\bootstrap-vcpkg.bat on Windows

# Install raylib
./vcpkg install raylib

# Then, when compiling your project:

```bash
g++ -std=c++17 main.cpp -o dapper -I/path/to/vcpkg/installed/include -L/path/to/vcpkg/installed/lib -lraylib
./dapper


# Option 2: Manual Setup
1. Download `raylib.h` and `libraylib.a` for your platform.
2. Place both files in your project directory.

Compile like this: 

```bash
g++ -std=c++17 main.cpp -o dapper -I. libraylib.a -lopengl32 -lgdi32 -lwinmm
./dapper


# 🧪 How to Run
- Press SPACE to jump
- Press S to activate shield (lasts 10 seconds, shows countdown)
- Press R to restart after game over
- Avoid red nebula in Boss Level (Level 10)