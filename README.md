# Fluid Simulation

A water physics simulation built with C and SDL2. Can watch water flow, settle under gravity, and interact with mouse to add/remove walls, obstacles, and water sources in real-time.

## Requirements

- GCC or Clang
- SDL2 development libraries

**macOS:**
```bash
brew install sdl2
```

## Build and Run
```bash
make  # compile
make run  # Compile and run
make clean  # Remove build artifacts
```

## Controls

- **W** - Paint water
- **S** - Paint solid walls
- **E** - Erase cells
- **T** - Paint tap (water source)
- **+/-** - Adjust tap emission rate
- **ESC** - Quit