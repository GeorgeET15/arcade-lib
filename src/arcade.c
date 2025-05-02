#include "arcade.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <errno.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_image_resize2.h"

/* =========================================================================
 * Internal State
 * ========================================================================= */
#ifdef _WIN32
typedef struct
{
    HWND hwnd;         /* Window handle */
    HDC hdc;           /* Device context */
    HBITMAP hbitmap;   /* Bitmap for double buffering */
    uint32_t *pixels;  /* Pixel buffer for rendering */
    int width, height; /* Window dimensions */
    uint32_t bg_color; /* Background color */
    HFONT hfont;       /* Font for text rendering */
    int running;       /* Game running state */
} ArcadeState;
#else
typedef struct
{
    Display *display;  /* X11 display connection */
    Window window;     /* Game window */
    int screen;        /* Default screen */
    uint32_t *pixels;  /* Pixel buffer for rendering */
    int width, height; /* Window dimensions */
    Atom wm_delete;    /* Window close event atom */
    XImage *image;     /* X11 image for rendering */
    GC gc;             /* Graphics context */
    uint32_t bg_color; /* Background color */
    XFontStruct *font; /* Font for text rendering */
    int running;       /* Game running state */
} ArcadeState;
#endif

static ArcadeState state = {0};
static int key_states[256] = {0};      /* Key states (0 = up, 1 = down) */
static int last_key_states[256] = {0}; /* Previous key states for single-press detection */
static int global_frame_counter = 0;   /* Frame counter for animations and blinking */

/* =========================================================================
 * Platform-Specific Input Handling (Windows Only)
 * ========================================================================= */
#ifdef _WIN32
static int arcade_to_vk(unsigned int arcade_key)
{
    switch (arcade_key)
    {
    case a_up:
        return VK_UP;
    case a_down:
        return VK_DOWN;
    case a_left:
        return VK_LEFT;
    case a_right:
        return VK_RIGHT;
    case a_w:
        return 'W';
    case a_a:
        return 'A';
    case a_s:
        return 'S';
    case a_d:
        return 'D';
    case a_r:
        return 'R';
    case a_p:
        return 'P';
    case a_space:
        return VK_SPACE;
    case a_esc:
        return VK_ESCAPE;
    default:
        return 0;
    }
}
#endif

/* =========================================================================
 * Platform-Specific Window Procedure (Windows Only)
 * ========================================================================= */
#ifdef _WIN32
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
    {
        int vk = (int)wParam;
        if (vk < 256)
            key_states[vk] = 1;
        break;
    }
    case WM_KEYUP:
    {
        int vk = (int)wParam;
        if (vk < 256)
            key_states[vk] = 0;
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        HDC memDC = CreateCompatibleDC(hdc);
        SelectObject(memDC, state.hbitmap);
        BitBlt(hdc, 0, 0, state.width, state.height, memDC, 0, 0, SRCCOPY);
        DeleteDC(memDC);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_DESTROY:
        state.running = 0;
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
#endif

/* =========================================================================
 * Core Functions
 * ========================================================================= */

int arcade_init(int window_width, int window_height, const char *window_title, uint32_t bg_color)
{
#ifdef _WIN32
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "ArcadeWindow";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    DWORD style = WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME;
    RECT rect = {0, 0, window_width, window_height};
    AdjustWindowRect(&rect, style, FALSE);
    state.hwnd = CreateWindow("ArcadeWindow", window_title, style,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              rect.right - rect.left, rect.bottom - rect.top,
                              NULL, NULL, wc.hInstance, NULL);
    if (!state.hwnd)
    {
        fprintf(stderr, "Cannot create window\n");
        return 1;
    }
    ShowWindow(state.hwnd, SW_SHOW);
    UpdateWindow(state.hwnd);

    state.hdc = GetDC(state.hwnd);
    state.width = window_width;
    state.height = window_height;
    state.bg_color = bg_color;
    state.running = 1;

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = window_width;
    bmi.bmiHeader.biHeight = -window_height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    state.hbitmap = CreateDIBSection(state.hdc, &bmi, DIB_RGB_COLORS, (void **)&state.pixels, NULL, 0);
    if (!state.hbitmap || !state.pixels)
    {
        ReleaseDC(state.hwnd, state.hdc);
        DestroyWindow(state.hwnd);
        fprintf(stderr, "Cannot create bitmap\n");
        return 1;
    }

    for (int i = 0; i < window_width * window_height; i++)
    {
        state.pixels[i] = bg_color;
    }

    state.hfont = CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "Courier New");
    if (!state.hfont)
    {
        DeleteObject(state.hbitmap);
        ReleaseDC(state.hwnd, state.hdc);
        DestroyWindow(state.hwnd);
        fprintf(stderr, "Cannot create font\n");
        return 1;
    }
#else
    state.display = XOpenDisplay(NULL);
    if (!state.display)
    {
        fprintf(stderr, "Cannot open display\n");
        return 1;
    }

    state.screen = DefaultScreen(state.display);
    state.window = XCreateSimpleWindow(state.display, RootWindow(state.display, state.screen),
                                       100, 100, window_width, window_height, 1,
                                       BlackPixel(state.display, state.screen),
                                       WhitePixel(state.display, state.screen));
    XStoreName(state.display, state.window, window_title);
    XSelectInput(state.display, state.window, ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask);
    state.wm_delete = XInternAtom(state.display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(state.display, state.window, &state.wm_delete, 1);
    XMapWindow(state.display, state.window);

    state.pixels = malloc(window_width * window_height * sizeof(uint32_t));
    if (!state.pixels)
    {
        XCloseDisplay(state.display);
        fprintf(stderr, "Cannot allocate pixels\n");
        return 1;
    }
    state.width = window_width;
    state.height = window_height;
    state.bg_color = bg_color;
    state.running = 1;

    state.font = XLoadQueryFont(state.display, "9x15");
    if (!state.font)
    {
        fprintf(stderr, "Cannot load font 9x15\n");
        XCloseDisplay(state.display);
        free(state.pixels);
        return 1;
    }

    state.image = XCreateImage(state.display, DefaultVisual(state.display, state.screen),
                               DefaultDepth(state.display, state.screen), ZPixmap, 0,
                               (char *)state.pixels, window_width, window_height, 32, 0);
    if (!state.image)
    {
        free(state.pixels);
        XCloseDisplay(state.display);
        fprintf(stderr, "Cannot create XImage\n");
        return 1;
    }

    state.gc = XCreateGC(state.display, state.window, 0, NULL);
    if (!state.gc)
    {
        XDestroyImage(state.image);
        XCloseDisplay(state.display);
        fprintf(stderr, "Cannot create GC\n");
        return 1;
    }

    for (int i = 0; i < state.width * state.height; i++)
    {
        state.pixels[i] = bg_color;
    }
#endif
    return 0;
}

void arcade_quit(void)
{
#ifdef _WIN32
    if (state.hfont)
    {
        DeleteObject(state.hfont);
        state.hfont = NULL;
    }
    if (state.hbitmap)
    {
        DeleteObject(state.hbitmap);
        state.hbitmap = NULL;
        state.pixels = NULL;
    }
    if (state.hdc)
    {
        ReleaseDC(state.hwnd, state.hdc);
        state.hdc = NULL;
    }
    if (state.hwnd)
    {
        DestroyWindow(state.hwnd);
        state.hwnd = NULL;
    }
#else
    if (state.font)
    {
        XFreeFont(state.display, state.font);
        state.font = NULL;
    }
    if (state.image)
    {
        XDestroyImage(state.image);
        state.image = NULL;
        state.pixels = NULL;
    }
    if (state.gc)
    {
        XFreeGC(state.display, state.gc);
        state.gc = NULL;
    }
    if (state.display && state.window)
    {
        XDestroyWindow(state.display, state.window);
        state.window = 0;
    }
    if (state.display)
    {
        XCloseDisplay(state.display);
        state.display = NULL;
    }
#endif
}

int arcade_update(void)
{
#ifdef _WIN32
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            state.running = 0;
            return 0;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#else
    XEvent event;
    while (XPending(state.display))
    {
        XNextEvent(state.display, &event);
        if (event.type == ClientMessage && event.xclient.data.l[0] == state.wm_delete)
        {
            state.running = 0;
            return 0;
        }
        else if (event.type == KeyPress)
        {
            KeySym keysym = XLookupKeysym(&event.xkey, 0);
            key_states[keysym & 0xFF] = 1;
        }
        else if (event.type == KeyRelease)
        {
            KeySym keysym = XLookupKeysym(&event.xkey, 0);
            key_states[keysym & 0xFF] = 0;
        }
    }
#endif
    global_frame_counter++;
    return 1;
}

int arcade_running(void)
{
    return state.running;
}

void arcade_set_running(int value)
{
    state.running = value;
}

void arcade_sleep(unsigned int milliseconds)
{
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000); /* Convert ms to microseconds */
#endif
}

/* =========================================================================
 * Input Handling
 * ========================================================================= */

int arcade_key_pressed(unsigned int key_val)
{
#ifdef _WIN32
    int vk = arcade_to_vk(key_val);
    return key_states[vk] ? 2 : 0;
#else
    return key_states[key_val & 0xFF] ? 2 : 0;
#endif
}

int arcade_key_pressed_once(unsigned int key_val)
{
#ifdef _WIN32
    int vk = arcade_to_vk(key_val);
    int current = key_states[vk];
    int last = last_key_states[vk];
    last_key_states[vk] = current;
    return current == 1 && last == 0 ? 2 : 0;
#else
    int key = key_val & 0xFF;
    int current = key_states[key];
    int last = last_key_states[key];
    last_key_states[key] = current;
    return current == 1 && last == 0 ? 2 : 0;
#endif
}

void arcade_clear_keys(void)
{
    memset(key_states, 0, sizeof(key_states));
    memset(last_key_states, 0, sizeof(last_key_states));
}

/* =========================================================================
 * Sprite Management
 * ========================================================================= */

static void move_sprite(ArcadeSprite *sprite, float gravity, int window_height)
{
    if (!sprite || !sprite->active)
        return;
    sprite->vy += gravity;
    sprite->y += sprite->vy;
    sprite->x += sprite->vx;
    if (sprite->y < 0.0f)
    {
        sprite->y = 0.0f;
        sprite->vy = 0.0f;
    }
    if (sprite->y > window_height - sprite->height)
    {
        sprite->y = window_height - sprite->height;
        sprite->vy = 0.0f;
    }
}

void arcade_move_sprite(ArcadeSprite *sprite, float gravity, int window_height)
{
    if (sprite)
        move_sprite(sprite, gravity, window_height);
}

void arcade_move_image_sprite(ArcadeImageSprite *sprite, float gravity, int window_height)
{
    if (!sprite || !sprite->active)
        return;
    sprite->vy += gravity;
    sprite->y += sprite->vy;
    sprite->x += sprite->vx;
    if (sprite->y < 0.0f)
    {
        sprite->y = 0.0f;
        sprite->vy = 0.0f;
    }
    if (sprite->y > window_height - sprite->height)
    {
        sprite->y = window_height - sprite->height;
        sprite->vy = 0.0f;
    }
}

static int check_collision(const ArcadeSprite *a, const ArcadeSprite *b)
{
    if (!a || !b || !a->active || !b->active)
        return 0;
    return (a->x < b->x + b->width &&
            a->x + a->width > b->x &&
            a->y < b->y + b->height &&
            a->y + a->height > b->y);
}

int arcade_check_collision(ArcadeSprite *a, ArcadeSprite *b)
{
    if (!a || !b)
        return 0;
    return check_collision(a, b);
}

int arcade_check_image_collision(ArcadeImageSprite *a, ArcadeImageSprite *b)
{
    if (!a || !b || !a->active || !b->active)
        return 0;
    return (a->x < b->x + b->width &&
            a->x + a->width > b->x &&
            a->y < b->y + b->height &&
            a->y + a->height > b->y);
}

static int load_image_sprite(ArcadeImageSprite *sprite, const char *filename, int target_width, int target_height)
{
    if (!sprite || !filename)
        return 1;
    int width, height, channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 4);
    if (!data)
    {
        fprintf(stderr, "Cannot load %s\n", filename);
        return 1;
    }
    unsigned char *resized_data = (unsigned char *)malloc(target_width * target_height * 4);
    if (!resized_data)
    {
        stbi_image_free(data);
        return 1;
    }
    if (stbir_resize_uint8_srgb(data, width, height, 0, resized_data, target_width, target_height, 0, 4) == 0)
    {
        fprintf(stderr, "Failed to resize %s to %dx%d\n", filename, target_width, target_height);
        stbi_image_free(data);
        free(resized_data);
        return 1;
    }
    sprite->image_width = target_width;
    sprite->image_height = target_height;
    sprite->pixels = malloc(target_width * target_height * sizeof(uint32_t));
    if (!sprite->pixels)
    {
        stbi_image_free(data);
        free(resized_data);
        return 1;
    }
    for (int y = 0; y < target_height; y++)
    {
        for (int x = 0; x < target_width; x++)
        {
            int idx = (y * target_width + x) * 4;
            sprite->pixels[y * target_width + x] =
                (resized_data[idx] << 16) | (resized_data[idx + 1] << 8) | resized_data[idx + 2] | (resized_data[idx + 3] << 24);
        }
    }
    stbi_image_free(data);
    free(resized_data);
    sprite->width = (float)target_width;
    sprite->height = (float)target_height;
    sprite->active = 1;
    return 0;
}

ArcadeImageSprite arcade_create_image_sprite(float x, float y, float w, float h, const char *filename)
{
    ArcadeImageSprite sprite = {
        .x = x, .y = y, .width = 0.0f, .height = 0.0f, .vx = 0.0f, .vy = 0.0f, .pixels = NULL, .image_width = 0, .image_height = 0, .active = 1};
    if (filename && load_image_sprite(&sprite, filename, (int)w, (int)h) != 0)
    {
        sprite.pixels = NULL;
    }
    return sprite;
}

void arcade_free_image_sprite(ArcadeImageSprite *sprite)
{
    if (sprite && sprite->pixels)
    {
        free(sprite->pixels);
        sprite->pixels = NULL;
        sprite->image_width = 0;
        sprite->image_height = 0;
        sprite->active = 0;
    }
}

ArcadeAnimatedSprite arcade_create_animated_sprite(float x, float y, float w, float h, const char **filenames, int frame_count, int frame_interval)
{
    ArcadeAnimatedSprite anim = {0};
    anim.frames = malloc(frame_count * sizeof(ArcadeImageSprite));
    anim.frame_count = frame_count;
    anim.frame_interval = frame_interval;
    for (int i = 0; i < frame_count; i++)
    {
        anim.frames[i] = arcade_create_image_sprite(x, y, w, h, filenames[i]);
        if (!anim.frames[i].pixels)
        {
            for (int j = 0; j < i; j++)
                arcade_free_image_sprite(&anim.frames[j]);
            free(anim.frames);
            return (ArcadeAnimatedSprite){0};
        }
    }
    anim.frames[0].active = 1;
    return anim;
}

void arcade_free_animated_sprite(ArcadeAnimatedSprite *anim)
{
    if (!anim || !anim->frames)
        return;
    for (int i = 0; i < anim->frame_count; i++)
        arcade_free_image_sprite(&anim->frames[i]);
    free(anim->frames);
    anim->frames = NULL;
    anim->frame_count = 0;
}

void arcade_move_animated_sprite(ArcadeAnimatedSprite *anim, float gravity, int window_height)
{
    if (!anim || !anim->frames[0].active)
        return;
    ArcadeImageSprite *current = &anim->frames[anim->current_frame];
    arcade_move_image_sprite(current, gravity, window_height);
    for (int i = 0; i < anim->frame_count; i++)
    {
        anim->frames[i].x = current->x;
        anim->frames[i].y = current->y;
        anim->frames[i].vx = current->vx;
        anim->frames[i].vy = current->vy;
    }
    if (++anim->frame_counter >= anim->frame_interval)
    {
        anim->current_frame = (anim->current_frame + 1) % anim->frame_count;
        anim->frame_counter = 0;
    }
}

int arcade_check_animated_collision(ArcadeAnimatedSprite *anim, ArcadeImageSprite *other)
{
    if (!anim || !other || !anim->frames[0].active || !other->active)
        return 0;
    return arcade_check_image_collision(&anim->frames[anim->current_frame], other);
}

/* =========================================================================
 * Rendering
 * ========================================================================= */

static void draw_sprite(ArcadeAnySprite *sprite, int type)
{
    if (!sprite)
        return;
    if (type == SPRITE_COLOR && sprite->sprite.active)
    {
        ArcadeSprite *s = &sprite->sprite;
        int x_start = (int)s->x;
        int y_start = (int)s->y;
        int x_end = x_start + (int)s->width;
        int y_end = y_start + (int)s->height;
        unsigned int color = s->color;
        for (int y = y_start; y < y_end && y < state.height; y++)
        {
            if (y < 0)
                continue;
            for (int x = x_start; x < x_end && x < state.width; x++)
            {
                if (x < 0)
                    continue;
                state.pixels[y * state.width + x] = color;
            }
        }
    }
    else if (type == SPRITE_IMAGE && sprite->image_sprite.active && sprite->image_sprite.pixels)
    {
        ArcadeImageSprite *s = &sprite->image_sprite;
        int x_start = (int)s->x;
        int y_start = (int)s->y;
        int x_end = x_start + (int)s->width;
        int y_end = y_start + (int)s->height;
        int iw = s->image_width;
        int ih = s->image_height;
        for (int y = y_start, sy = 0; y < y_end && y < state.height && sy < ih; y++, sy++)
        {
            if (y < 0)
                continue;
            for (int x = x_start, sx = 0; x < x_end && x < state.width && sx < iw; x++, sx++)
            {
                if (x < 0)
                    continue;
                uint32_t pixel = s->pixels[sy * iw + sx];
                if ((pixel >> 24) > 0)
                {
                    state.pixels[y * state.width + x] = pixel;
                }
            }
        }
    }
}

void arcade_render_scene(ArcadeAnySprite *sprites, int count, int *types)
{
    for (int i = 0; i < state.width * state.height; i++)
    {
        state.pixels[i] = state.bg_color;
    }
    for (int i = 0; i < count; i++)
    {
        draw_sprite(&sprites[i], types[i]);
    }
#ifdef _WIN32
    HDC memDC = CreateCompatibleDC(state.hdc);
    SelectObject(memDC, state.hbitmap);
    BitBlt(state.hdc, 0, 0, state.width, state.height, memDC, 0, 0, SRCCOPY);
    DeleteDC(memDC);
#else
    XPutImage(state.display, state.window, state.gc, state.image, 0, 0, 0, 0, state.width, state.height);
#endif
}

void arcade_render_text(const char *text, float x, float y, unsigned int color)
{
    if (!text)
        return;
#ifdef _WIN32
    if (!state.hfont)
    {
        fprintf(stderr, "arcade_render_text: Skipping (font=%p)\n", state.hfont);
        return;
    }
    HDC memDC = CreateCompatibleDC(state.hdc);
    SelectObject(memDC, state.hbitmap);
    SelectObject(memDC, state.hfont);
    SetTextColor(memDC, color);
    SetBkMode(memDC, TRANSPARENT);
    TextOut(memDC, (int)x, (int)y, text, strlen(text));
    BitBlt(state.hdc, 0, 0, state.width, state.height, memDC, 0, 0, SRCCOPY);
    DeleteDC(memDC);
#else
    if (!state.font)
    {
        fprintf(stderr, "arcade_render_text: Skipping (font=%p)\n", state.font);
        return;
    }
    XSetForeground(state.display, state.gc, color);
    XSetFont(state.display, state.gc, state.font->fid);
    XDrawString(state.display, state.window, state.gc, (int)x, (int)y, text, strlen(text));
    XFlush(state.display);
#endif
}

void arcade_render_text_centered(const char *text, float y, unsigned int color)
{
    if (!text)
        return;
#ifdef _WIN32
    if (!state.hfont)
        return;
    SIZE size;
    HDC memDC = CreateCompatibleDC(state.hdc);
    SelectObject(memDC, state.hfont);
    GetTextExtentPoint32(memDC, text, strlen(text), &size);
    float x = (state.width - size.cx) / 2.0f;
    arcade_render_text(text, x, y, color);
    DeleteDC(memDC);
#else
    if (!state.font)
        return;
    int text_width = XTextWidth(state.font, text, strlen(text));
    float x = (state.width - text_width) / 2.0f;
    arcade_render_text(text, x, y, color);
#endif
}

void arcade_render_text_centered_blink(const char *text, float y, unsigned int color, int blink_interval)
{
    if (!text)
        return;
    if ((global_frame_counter % (2 * blink_interval)) < blink_interval)
    {
        arcade_render_text_centered(text, y, color);
    }
}

/* =========================================================================
 * Sprite Groups
 * ========================================================================= */

void arcade_init_group(SpriteGroup *group, int capacity)
{
    group->sprites = malloc(capacity * sizeof(ArcadeAnySprite));
    group->types = malloc(capacity * sizeof(int));
    group->count = 0;
    group->capacity = capacity;
}

void arcade_add_sprite_to_group(SpriteGroup *group, ArcadeAnySprite sprite, int type)
{
    if (group->count < group->capacity)
    {
        group->sprites[group->count] = sprite;
        group->types[group->count] = type;
        group->count++;
    }
}

void arcade_add_animated_to_group(SpriteGroup *group, ArcadeAnimatedSprite *anim)
{
    if (!anim || !anim->frames[0].active)
        return;
    arcade_add_sprite_to_group(group, (ArcadeAnySprite){.image_sprite = anim->frames[anim->current_frame]}, SPRITE_IMAGE);
}

void arcade_render_group(SpriteGroup *group)
{
    arcade_render_scene(group->sprites, group->count, group->types);
}

void arcade_free_group(SpriteGroup *group)
{
    free(group->sprites);
    free(group->types);
    group->count = 0;
    group->capacity = 0;
}

/* =========================================================================
 * Audio
 * ========================================================================= */

int arcade_play_sound(const char *audio_file_path)
{
#ifdef _WIN32
    return PlaySound(audio_file_path, NULL, SND_FILENAME | SND_ASYNC) ? 0 : 1;
#else
    char command[256];
    snprintf(command, sizeof(command), "aplay -q %s &", audio_file_path);
    return system(command);
#endif
}

/* =========================================================================
 * Image Manipulation
 * ========================================================================= */

char *arcade_flip_image(const char *input_path, int flip_type)
{
    int width, height, channels;
    unsigned char *data = stbi_load(input_path, &width, &height, &channels, 4);
    if (!data)
    {
        fprintf(stderr, "Failed to load image %s for flipping\n", input_path);
        return NULL;
    }
    unsigned char *flipped_data = (unsigned char *)malloc(width * height * 4);
    if (!flipped_data)
    {
        stbi_image_free(data);
        fprintf(stderr, "Memory allocation failed for flipped image\n");
        return NULL;
    }
    if (flip_type == 1)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int src_idx = (height - 1 - y) * width * 4 + x * 4;
                int dst_idx = y * width * 4 + x * 4;
                for (int c = 0; c < 4; c++)
                {
                    flipped_data[dst_idx + c] = data[src_idx + c];
                }
            }
        }
    }
    else
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int src_idx = y * width * 4 + (width - 1 - x) * 4;
                int dst_idx = y * width * 4 + x * 4;
                for (int c = 0; c < 4; c++)
                {
                    flipped_data[dst_idx + c] = data[src_idx + c];
                }
            }
        }
    }
#ifdef _WIN32
    char temp_path[MAX_PATH];
    if (!GetTempFileName(".", "arc", 0, temp_path))
    {
        stbi_image_free(data);
        free(flipped_data);
        fprintf(stderr, "Failed to create temporary file\n");
        return NULL;
    }
    char full_path[MAX_PATH];
    snprintf(full_path, sizeof(full_path), "%s.png", temp_path);
#else
    char temp_path[] = "/tmp/arcade_flip_XXXXXX";
    int fd = mkstemp(temp_path);
    if (fd == -1)
    {
        stbi_image_free(data);
        free(flipped_data);
        fprintf(stderr, "Failed to create temporary file: %s\n", strerror(errno));
        return NULL;
    }
    close(fd);
    char *full_path = malloc(strlen(temp_path) + 5);
    if (!full_path)
    {
        remove(temp_path);
        stbi_image_free(data);
        free(flipped_data);
        fprintf(stderr, "Memory allocation failed for path\n");
        return NULL;
    }
    sprintf(full_path, "%s.png", temp_path);
#endif
    if (!stbi_write_png(full_path, width, height, 4, flipped_data, width * 4))
    {
        remove(full_path);
        stbi_image_free(data);
        free(flipped_data);
#ifdef _WIN32
        fprintf(stderr, "Failed to write flipped image to %s\n", full_path);
        return NULL;
#else
        free(full_path);
        fprintf(stderr, "Failed to write flipped image to %s\n", full_path);
        return NULL;
#endif
    }
    stbi_image_free(data);
    free(flipped_data);
#ifdef _WIN32
    char *result = strdup(full_path);
    if (!result)
    {
        DeleteFile(temp_path);
        fprintf(stderr, "Failed to duplicate path\n");
        return NULL;
    }
    DeleteFile(temp_path);
    return result;
#else
    char *result = strdup(full_path);
    free(full_path);
    if (!result)
    {
        remove(temp_path);
        fprintf(stderr, "Failed to duplicate path\n");
        return NULL;
    }
    return result;
#endif
}

char *arcade_rotate_image(const char *input_path, int degrees)
{
    int width, height, channels;
    unsigned char *data = stbi_load(input_path, &width, &height, &channels, 4);
    if (!data)
    {
        fprintf(stderr, "Failed to load image %s for rotation\n", input_path);
        return NULL;
    }
    int new_width = (degrees == 90 || degrees == 270) ? height : width;
    int new_height = (degrees == 90 || degrees == 270) ? width : height;
    unsigned char *rotated_data = (unsigned char *)malloc(new_width * new_height * 4);
    if (!rotated_data)
    {
        stbi_image_free(data);
        fprintf(stderr, "Memory allocation failed for rotated image\n");
        return NULL;
    }
    for (int y = 0; y < new_height; y++)
    {
        for (int x = 0; x < new_width; x++)
        {
            int src_x, src_y;
            if (degrees == 90)
            {
                src_x = y;
                src_y = new_width - 1 - x;
            }
            else if (degrees == 180)
            {
                src_x = width - 1 - x;
                src_y = height - 1 - y;
            }
            else if (degrees == 270)
            {
                src_x = new_height - 1 - y;
                src_y = x;
            }
            else
            {
                src_x = x;
                src_y = y;
            }
            int src_idx = src_y * width * 4 + src_x * 4;
            int dst_idx = y * new_width * 4 + x * 4;
            for (int c = 0; c < 4; c++)
            {
                rotated_data[dst_idx + c] = data[src_idx + c];
            }
        }
    }
#ifdef _WIN32
    char temp_path[MAX_PATH];
    if (!GetTempFileName(".", "arc", 0, temp_path))
    {
        stbi_image_free(data);
        free(rotated_data);
        fprintf(stderr, "Failed to create temporary file\n");
        return NULL;
    }
    char full_path[MAX_PATH];
    snprintf(full_path, sizeof(full_path), "%s.png", temp_path);
#else
    char temp_path[] = "/tmp/arcade_rotate_XXXXXX";
    int fd = mkstemp(temp_path);
    if (fd == -1)
    {
        stbi_image_free(data);
        free(rotated_data);
        fprintf(stderr, "Failed to create temporary file: %s\n", strerror(errno));
        return NULL;
    }
    close(fd);
    char *full_path = malloc(strlen(temp_path) + 5);
    if (!full_path)
    {
        remove(temp_path);
        stbi_image_free(data);
        free(rotated_data);
        fprintf(stderr, "Memory allocation failed for path\n");
        return NULL;
    }
    sprintf(full_path, "%s.png", temp_path);
#endif
    if (!stbi_write_png(full_path, new_width, new_height, 4, rotated_data, new_width * 4))
    {
        remove(full_path);
        stbi_image_free(data);
        free(rotated_data);
#ifdef _WIN32
        fprintf(stderr, "Failed to write rotated image to %s\n", full_path);
        return NULL;
#else
        free(full_path);
        fprintf(stderr, "Failed to write rotated image to %s\n", full_path);
        return NULL;
#endif
    }
    stbi_image_free(data);
    free(rotated_data);
#ifdef _WIN32
    char *result = strdup(full_path);
    if (!result)
    {
        DeleteFile(temp_path);
        fprintf(stderr, "Failed to duplicate path\n");
        return NULL;
    }
    DeleteFile(temp_path);
    return result;
#else
    char *result = strdup(full_path);
    free(full_path);
    if (!result)
    {
        remove(temp_path);
        fprintf(stderr, "Failed to duplicate path\n");
        return NULL;
    }
    return result;
#endif
}