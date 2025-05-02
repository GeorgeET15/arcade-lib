# Arcade Library

[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![GitHub Stars](https://img.shields.io/github/stars/GeorgeET15/arcade-lib)](https://github.com/GeorgeET15/arcade-lib)

Arcade Library is a lightweight C library for creating retro 2D games like Flappy Bird or Pong. It supports window management, sprite rendering (color and image-based), animation, collision detection, WAV audio playback, text rendering, and image manipulation. Designed for simplicity, it runs on Windows (Win32) and Linux (X11) with minimal dependencies.

Visit the [Arcade Library website](https://arcade-lib.dev) for examples, tutorials, and full documentation.

## Features

- Window management with `arcade_init`.
- Sprite rendering: color-based, image-based, and animated sprites.
- Keyboard input with continuous and single-press detection.
- AABB collision detection for sprites.
- WAV audio playback with `arcade_play_sound`.
- Text rendering with fixed fonts and blinking effects.
- Image manipulation (flip, rotate).

## Installation

### Prerequisites

- **Windows**:
  - GCC (e.g., MinGW-w64).
  - Libraries: `gdi32`, `winmm` (included with MinGW).
- **Linux**:
  - GCC.
  - Libraries: `libX11`, `libm` (install with `sudo apt install libx11-dev`).
  - `aplay` for audio (install with `sudo apt install alsa-utils`).
- **Both**:
  - STB libraries (included in `include/`).

### Clone the Repository

```bash
git clone https://github.com/GeorgeET15/arcade-lib.git
cd arcade-lib
```

## Usage

1. **Include the Library**:
   ```c
   #include "include/arcade.h"
   ```
2. **Write Your Game**:
   Use `arcade.h` functions to create a game. See the [Arcade Library website](https://arcade-lib.dev) for examples and API documentation.
3. **Compile**:
   - **Windows** (MinGW):
     ```bash
     gcc -o game game.c src/arcade.c -Iinclude -lgdi32 -lwinmm
     ```
   - **Linux**:
     ```bash
     gcc -o game game.c src/arcade.c -Iinclude -lX11 -lm
     ```
4. **Run**:
   ```bash
   ./game
   ```

## Dependencies

- **STB Libraries**: Included in `include/` (`stb_image.h`, `stb_image_write.h`, `stb_image_resize2.h`).
- **Windows**: `gdi32` (rendering), `winmm` (audio).
- **Linux**: `libX11` (rendering), `libm` (math), `aplay` (audio).

## Giving Credit

Arcade Library is licensed under the [MIT License](LICENSE). If you use the library, please include in your project’s documentation or README:

```
Arcade Library (https://github.com/GeorgeET15/arcade-lib)
Copyright (c) 2025 GeorgeET15
Licensed under the MIT License
```

Include a copy of the [LICENSE](LICENSE) file in distributions.

## Contributing

Contributions are welcome! To get started:

1. Read [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.
2. Fork the repo, create a branch, and submit a pull request.
3. Report issues or suggest features on [GitHub Issues](https://github.com/GeorgeET15/arcade-lib/issues).

## License

Released under the [MIT License](LICENSE).

## Contact

Questions or ideas? Open an issue on [GitHub](https://github.com/GeorgeET15/arcade-lib/issues) or email GeorgeET15 at georgeemmanuelthomas@gmail.com.

Build retro games with Arcade Library! 🎮
