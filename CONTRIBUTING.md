# Contributing to Arcade Library

Thank you for your interest in contributing to Arcade Library! We welcome contributions to improve this lightweight C library for retro 2D game development.

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
   - Add features, fix bugs, or improve documentation.
   - Follow the coding style in `arcade.c` (e.g., 4-space indentation, clear comments).
5. **Test Your Changes**:
   - Write a test program using `arcade.h` and compile:
     ```bash
     gcc -o test test.c src/arcade.c -Iinclude -lgdi32 -lwinmm # Windows
     gcc -o test test.c src/arcade.c -Iinclude -lX11 -lm # Linux
     ./test
     ```
6. **Commit and Push**:
   ```bash
   git add .
   git commit -m "Add your-feature: description"
   git push origin feature/your-feature
   ```
7. **Submit a Pull Request**:
   - Go to the [arcade-lib repo](https://github.com/GeorgeET15/arcade-lib).
   - Create a pull request from your branch.
   - Describe your changes and link to any related issues.

## Contribution Ideas

- Optimize rendering (`arcade_render_scene`) for large sprite groups.
- Support additional image formats in `arcade_create_image_sprite`.
- Improve audio handling (e.g., preloading WAV files on Windows).
- Enhance `arcade.h` documentation with detailed comments.
- Add support for new platforms or input devices.

## Reporting Issues

- Open an issue on [GitHub Issues](https://github.com/GeorgeET15/arcade-lib/issues).
- Include details: OS, compiler, steps to reproduce, and expected behavior.

## Code Style

- Use 4-space indentation.
- Add comments for clarity, especially for platform-specific code.
- Keep functions concise and focused.

## Contact

Questions? Email GeorgeET15 at georgeemmanuelthomas@gmail.com or open an issue.

Let’s make Arcade Library better together! 🎮
