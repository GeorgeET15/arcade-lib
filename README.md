# ARCADE: Awesome Rendering Control And Dynamics Engine

[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![GitHub Stars](https://img.shields.io/github/stars/GeorgeET15/arcade-lib)](https://github.com/GeorgeET15/arcade-lib)

ARCADE (Awesome Rendering Control And Dynamics Engine) is a lightweight C library for creating retro 2D games like Flappy Bird or Pong. It supports window management, sprite rendering, animation, collision detection, WAV audio, text rendering, and image manipulation. End users can download the self-contained `arcade.h` from [Releases](https://github.com/GeorgeET15/arcade-lib/releases) and use `#include <arcade.h>`. Developers can use the repo’s `include/arcade.h` and `src/arcade.c`. Runs on Windows (Win32) and Linux (X11).

Visit the [ARCADE website](https://arcade-lib.dev) for examples, tutorials, and documentation.

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

### For End Users (Release)

1. **Download `arcade.h`**:
   - Get the self-contained `arcade.h` from [Releases](https://github.com/GeorgeET15/arcade-lib/releases).
2. **Install**:
   - **System-Wide** (Recommended):
     ```bash
     sudo cp arcade.h /usr/local/include/ # Linux
     cp arcade.h /c/MinGW/include/ # Windows (MinGW)
     ```
   - **Project-Local**:
     ```bash
     cp arcade.h my-game/
     ```
3. **Use**:
   ```c
   #define ARCADE_IMPLEMENTATION
   #include <arcade.h>
   int main() {
       arcade_init(800, 600, "My Game", 0x000000);
       ArcadeImageSprite player = arcade_create_image_sprite(100.0f, 100.0f, 50.0f, 50.0f, "player.png");
       SpriteGroup group;
       arcade_init_group(&group, 1);
       while (arcade_running() && arcade_update()) {
           if (arcade_key_pressed(a_right)) player.vx = 5.0f;
           else if (arcade_key_pressed(a_left)) player.vx = -5.0f;
           else player.vx = 0.0f;
           arcade_move_image_sprite(&player, 0.1f, 600);
           arcade_add_sprite_to_group(&group, (ArcadeAnySprite){.image_sprite = player}, SPRITE_IMAGE);
           arcade_render_group(&group);
           arcade_render_text("Move: Left/Right", 10.0f, 10.0f, 0xFFFFFF);
           arcade_sleep(16); // ~60 FPS
       }
       arcade_free_image_sprite(&player);
       arcade_free_group(&group);
       arcade_quit();
       return 0;
   }
   ```
4. **Compile**:
   - System-wide or project-local:
     ```bash
     gcc -o game game.c -lgdi32 -lwinmm # Windows
     gcc -o game game.c -lX11 -lm # Linux
     ```

### For Developers (Repo)

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/GeorgeET15/arcade-lib.git
   cd arcade-lib
   ```
2. **Use**:
   ```c
   #include "include/arcade.h"
   int main() {
       arcade_init(800, 600, "My Game", 0x000000);
       // ... (same as above)
       arcade_quit();
       return 0;
   }
   ```
3. **Compile**:
   ```bash
   gcc -o game game.c src/arcade.c -Iinclude -lgdi32 -lwinmm # Windows
   gcc -o game game.c src/arcade.c -Iinclude -lX11 -lm # Linux
   ```

## Dependencies

- **Release**: None (all code in `arcade.h`).
- **Repo**: STB libraries (in `include/`).
- **Windows**: `gdi32`, `winmm`.
- **Linux**: `libX11`, `libm`, `aplay`.

## Giving Credit

ARCADE is licensed under the [MIT License](LICENSE). Include in your project’s documentation:

```
ARCADE: Awesome Rendering Control And Dynamics Engine (https://github.com/GeorgeET15/arcade-lib)
Copyright (c) 2025 GeorgeET15
Licensed under the MIT License
```

Include a copy of the [LICENSE](LICENSE) file in distributions.

## Contributing

Contributions are welcome! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License

Released under the [MIT License](LICENSE).

## Contact

Questions or ideas? Open an issue on [GitHub](https://github.com/GeorgeET15/arcade-lib/issues) or email GeorgeET15 at georgeemmanuelthomas@gmail.com.

Build retro games with ARCADE! 🎮
