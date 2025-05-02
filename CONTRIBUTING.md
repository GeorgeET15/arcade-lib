# Contributing to ARCADE: Awesome Rendering Control And Dynamics Engine

Thank you for your interest in contributing to ARCADE (Awesome Rendering Control And Dynamics Engine)! We welcome contributions to improve this lightweight C library for retro 2D game development.

## How to Contribute

1. **Fork the Repository**:
   - Fork [arcade-lib](https://github.com/GeorgeET15/arcade-lib) to your GitHub account.
2. **Clone Your Fork**:
   ```bash
   git clone https://github.com/your-username/arcade-lib.git
   cd arcade-lib
   ```
3. **Create a Branch**:
   ```bash
   git checkout -b feature/your-feature
   ```
4. **Make Changes**:
   - Modify `include/arcade.h` (declarations) or `src/arcade.c` (implementation).
   - Follow the coding style (4-space indentation, clear comments).
5. **Test Your Changes**:
   - Write a test program:
     ```c
     #include "include/arcade.h"
     int main() {
         arcade_init(800, 600, "Test", 0x000000);
         while (arcade_running() && arcade_update()) {
             arcade_sleep(16);
         }
         arcade_quit();
         return 0;
     }
     ```
   - Compile:
     ```bash
     gcc -o test test.c src/arcade.c -Iinclude -lgdi32 -lwinmm # Windows
     gcc -o test test.c src/arcade.c -Iinclude -lX11 -lm # Linux
     ```
6. **Update Release `arcade.h`** (if needed):
   - If changes affect `arcade.h` or `arcade.c`, update the self-contained `arcade.h` for releases.
   - Copy `include/arcade.h` and `src/arcade.c` into the release `arcade.h`’s `#ifdef ARCADE_IMPLEMENTATION` block.
7. **Commit and Push**:
   ```bash
   git add .
   git commit -m "Add your-feature: description"
   git push origin feature/your-feature
   ```
8. **Submit a Pull Request**:
   - Go to [arcade-lib](https://github.com/GeorgeET15/arcade-lib).
   - Create a pull request and describe your changes.

## Contribution Ideas

- Optimize rendering (`arcade_render_scene`).
- Support additional image formats in `arcade_create_image_sprite`.
- Improve audio handling (e.g., preloading WAV files).
- Enhance documentation in `arcade.h` or `README.md`.
- Add platform or input device support.

## Reporting Issues

- Open an issue on [GitHub Issues](https://github.com/GeorgeET15/arcade-lib/issues).
- Include OS, compiler, steps to reproduce, and expected behavior.

## Code Style

- Use 4-space indentation.
- Add clear comments, especially for platform-specific code.
- Keep functions concise.

## Contact

Questions? Email GeorgeET15 at georgeemmanuelthomas@gmail.com or open an issue.

Let’s make ARCADE better together! 🎮
