# ARCADE: Awesome Rendering Control And Dynamics Engine

[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![GitHub Stars](https://img.shields.io/github/stars/GeorgeET15/arcade-lib)](https://github.com/GeorgeET15/arcade-lib)

ARCADE (Awesome Rendering Control And Dynamics Engine) is a lightweight, single-header C library for creating retro 2D games. Runs on Windows (Win32) and Linux (X11).

Visit the [ARCADE website](https://arcade-lib.dev) for examples, tutorials, and documentation.

## Features

- Window management.
- Sprite rendering: color-based, image-based, and animated sprites.
- Keyboard input with continuous and single-press detection.
- AABB collision detection for sprites.
- WAV and MP3 audio playback.
- Text rendering with fixed fonts and blinking effects.
- Image manipulation (flip, rotate).

## Getting Started

1. **Install the Arcade CLI** (recommended):

   ```bash
   npm install -g arcade-cli
   ```

   Place a `background_music.mp3` file in the CLI’s `./assets/` directory (e.g., `./node_modules/arcade-cli/assets/`).

2. **Initialize a Project**:

   - Full project with a game demo:
     ```bash
     arcade init my-game
     cd my-game
     make
     make run
     ```
   - Blank project (library headers only):
     ```bash
     arcade init my-game -b
     ```

3. **Explore the Demo** (full project):
   - `main.c` includes a red square movable with arrow keys, a start screen, background music (`assets/background_music.mp3`), and detailed comments.
   - Modify `main.c` or add assets to build your game. See the [ARCADE Wiki](https://github.com/GeorgeET15/arcade-lib/wiki).

## Installation

### Prerequisites

- **Windows**:
  - GCC (e.g., MinGW-w64).
  - Libraries: `gdi32`, `winmm` (included with MinGW).
- **Linux**:
  - GCC.
  - Libraries: `libX11`, `libm` (install with `sudo apt install libx11-dev`).
  - `aplay` for audio (install with `sudo apt install alsa-utils`).
- **STB Libraries**:
  - Download `stb_image.h`, `stb_image_write.h`, and `stb_image_resize2.h` from [STB](https://github.com/nothings/stb).
  - Autmoatically installed if using the cli tool.

### Setup with Arcade CLI (Recommended)

The [Arcade CLI](https://github.com/GeorgeET15/arcade-cli) automates project setup, downloading `arcade.h`, STB headers, and generating a beginner-friendly project with a `Makefile`, `.gitignore`, and a `main.c` demo featuring sprite movement, text, and background music.

1. **Install Arcade CLI**:

   ```bash
   npm install -g arcade-cli
   ```

2. **Place Static Asset**:

   - Create an `assets/` directory in the CLI’s installation directory (e.g., `./node_modules/arcade-cli/assets/`).
   - Add a valid `background_music.mp3` file (e.g., from [freesound.org](https://freesound.org)).

3. **Initialize a Project**:

   - Full project:
     ```bash
     arcade init my-game
     cd my-game
     make
     make run
     ```
   - Blank project:
     ```bash
     arcade init my-lib -b
     cd my-lib
     # Add your own source files and Makefile
     ```

4. **Explore `main.c`** (full project):
   - Features a red square (color-based sprite) movable with arrow keys, a start screen, and looping background music (`assets/background_music.mp3`).
   - Includes detailed comments for beginners.
   - See the [Arcade CLI documentation](https://github.com/GeorgeET15/arcade-cli) for details.

### Manual Setup

1. **Download `arcade.h`**:

   - Get the self-contained `arcade.h` from [Releases](https://github.com/GeorgeET15/arcade-lib/releases).

2. **Create `arcade/` Folder**:

   - In your project folder (e.g., `my-game/`), create a subfolder named `arcade/`:
     ```bash
     mkdir my-game/arcade
     ```

3. **Place Header Files**:

   - Copy `arcade.h` to `my-game/arcade/`.
   - Copy `stb_image.h`, `stb_image_write.h`, and `stb_image_resize2.h` to `my-game/arcade/`:
     ```bash
     cp arcade.h my-game/arcade/
     cp stb_image.h stb_image_write.h stb_image_resize2.h my-game/arcade/
     ```

4. **Use in Code**:

   - In your source file (e.g., `game.c`), define `ARCADE_IMPLEMENTATION` and include `arcade/arcade.h`:

     ```c
     // game.c: A simple ARCADE game moving a red square with left/right keys
      #define ARCADE_IMPLEMENTATION
      #include "arcade/arcade.h"
      #include <stdio.h>

      int main() {
         // Initialize window (800x600, black background)
         if (arcade_init(800, 600, "My Game", 0x000000) != 0) {
            return 1;
         }

         // Initialize player sprite (50x50 red square at center)
         ArcadeSprite player = {
            .x = 400.0f, .y = 300.0f, // Center (800/2, 600/2)
            .width = 50.0f, .height = 50.0f,
            .vx = 0.0f, .vy = 0.0f,
            .color = 0xFF0000, // Red
            .active = 1
         };

         // Initialize sprite group for rendering
         SpriteGroup group;
         arcade_init_group(&group, 1);

         // Main game loop
         while (arcade_running() && arcade_update()) {
            // Optional: Play background music (requires assets/background_music.mp3)
            arcade_play_sound("assets/background_music.mp3");

            // Reset sprite group each frame
            group.count = 0;

            // Handle input: move left/right
            player.vx = 0.0f;
            if (arcade_key_pressed(a_right) == 2) player.vx = 5.0f;
            if (arcade_key_pressed(a_left) == 2) player.vx = -5.0f;

            // Update player position
            player.x += player.vx;
            if (player.x < 0) player.x = 0;
            if (player.x > 750) player.x = 750; // 800 - 50

            // Add player to render group
            arcade_add_sprite_to_group(&group, (ArcadeAnySprite){.sprite = player}, SPRITE_COLOR);

            // Render sprites and text
            arcade_render_group(&group);
            arcade_render_text("Move: Left/Right", 10.0f, 10.0f, 0xFFFFFF);

            // Maintain ~60 FPS
            arcade_sleep(16);
         }

         // Clean up
         arcade_free_group(&group);
         arcade_quit();
         return 0;
      }
     ```

5. **Compile**:
   - From your project folder (e.g., `my-game/`), compile with the `arcade/` subfolder included:
     ```bash
     gcc -o game game.c -Iarcade -lgdi32 -lwinmm # Windows (MinGW)
     gcc -o game game.c -Iarcade -lX11 -lm # Linux
     ```

## Folder Structure Example

### Manual Setup

```
my-game/
├── game.c
└── arcade/
    ├── arcade.h
    ├── stb_image.h
    ├── stb_image_write.h
    ├── stb_image_resize2.h
```

If using audio, add an `assets/` folder:

```
my-game/
├── game.c
├── arcade/
│   ├── arcade.h
│   ├── stb_image.h
│   ├── stb_image_write.h
│   ├── stb_image_resize2.h
└── assets/
    ├── background_music.mp3
```

### Arcade CLI (Full Project)

```
my-game/
├── main.c
├── Makefile
├── .gitignore
├── arcade/
│   ├── arcade.h
│   ├── stb_image.h
│   ├── stb_image_write.h
│   ├── stb_image_resize2.h
└── assets/
    ├── background_music.mp3
```

### Arcade CLI (Blank Project, `-b`)

```
my-lib/
├── arcade/
│   ├── arcade.h
│   ├── stb_image.h
│   ├── stb_image_write.h
│   ├── stb_image_resize2.h
```

## Dependencies

- **arcade.h**: Self-contained, downloaded from [Releases](https://github.com/GeorgeET15/arcade-lib/releases).
- **STB Libraries**: `stb_image.h`, `stb_image_write.h`, `stb_image_resize2.h` (place in `arcade/`).
- **Windows**: `gdi32`, `winmm` (included with MinGW).
- **Linux**: `libX11`, `libm`, `aplay`.
- **Arcade CLI (optional)**: Node.js, `arcade-cli` (via npm), and a `background_music.mp3` in the CLI’s `./assets/`.

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
