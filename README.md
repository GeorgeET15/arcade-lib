# ARCADE: Awesome Rendering Control And Dynamics Engine

[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![GitHub Stars](https://img.shields.io/github/stars/GeorgeET15/arcade-lib)](https://github.com/GeorgeET15/arcade-lib)

ARCADE (Awesome Rendering Control And Dynamics Engine) is a lightweight, single-header C library for creating retro 2D games. Runs on Windows (Win32) and Linux (X11).

Visit the [ARCADE website](https://arcade-lib.dev) for examples, tutorials, and documentation.

## Features

- Window management with.
- Sprite rendering: color-based, image-based, and animated sprites.
- Keyboard input with continuous and single-press detection.
- AABB collision detection for sprites.
- WAV audio playback with.
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
- **STB Libraries**:
  - Download `stb_image.h`, `stb_image_write.h`, and `stb_image_resize2.h` from [STB](https://github.com/nothings/stb).

### Setup with Arcade CLI (Recommended)

The [Arcade CLI](https://github.com/GeorgeET15/arcade-cli) is a command-line tool that automates project setup, downloading `arcade.h`, STB headers, and generating a starter project with a `Makefile` and example code.

1. **Install Arcade CLI**:

   ```bash
   npm install -g arcade-cli
   ```

2. **Initialize a Project**:

   ```bash
   arcade init my-game
   cd my-game
   make
   make run
   ```

3. **Replace Assets**:
   - Replace `assets/player.png` and `assets/sound.wav` with valid files.

See the [Arcade CLI documentation](https://github.com/GeorgeET15/arcade-cli) for details.

### Manual Setup

1. **Download `arcade.h`**:

   - Get the self-contained `arcade.h` from [Releases](https://github.com/GeorgeET15/arcade-lib/releases).

2. **Create `arcade/` Folder**:

   - In your game project folder (e.g., `my-game/`), create a subfolder named `arcade/`:
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

   - In your game source file (e.g., `game.c`), define `ARCADE_IMPLEMENTATION` and include `arcade/arcade.h`:

     ```c
     #define ARCADE_IMPLEMENTATION
     #include "arcade/arcade.h"

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

5. **Compile**:
   - From your project folder (e.g., `my-game/`), compile with the `arcade/` subfolder included:
     ```bash
     gcc -o game game.c -Iarcade -lgdi32 -lwinmm # Windows (MinGW)
     gcc -o game game.c -Iarcade -lX11 -lm # Linux
     ```

## Folder Structure Example

After setup (manual or via CLI), your project folder should look like:

```
my-game/
├── game.c
└── arcade/
    ├── arcade.h
    ├── stb_image.h
    ├── stb_image_write.h
    ├── stb_image_resize2.h
```

If using image-based sprites or audio, add an `assets/` folder:

```
my-game/
├── game.c
├── arcade/
│   ├── arcade.h
│   ├── stb_image.h
│   ├── stb_image_write.h
│   ├── stb_image_resize2.h
└── assets/
    ├── player.png
    ├── sound.wav
```

With the Arcade CLI, you also get:

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
    ├── player.png
    ├── sound.wav
```

## Dependencies

- **arcade.h**: Self-contained, downloaded from [Releases](https://github.com/GeorgeET15/arcade-lib/releases).
- **STB Libraries**: `stb_image.h`, `stb_image_write.h`, `stb_image_resize2.h` (place in `arcade/`).
- **Windows**: `gdi32`, `winmm` (included with MinGW).
- **Linux**: `libX11`, `libm`, `aplay`.
- **Arcade CLI (optional)**: Node.js and `arcade-cli` (via npm).

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
