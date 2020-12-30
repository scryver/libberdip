#include "./common.h"
#include "./intrinsics.h"
#include "./maths.h"
#include "./memory.h"
#include "./strings.h"
#include "./vectors.h"
#include "./keys.h"
#include "./fonts.h"
#include "./files.h"
#include "./threading.h"

typedef struct Key
{
    b8 isDown;
    b8 isPressed;
    b8 isReleased;
    u8 edgeCount;
} Key;

typedef enum MouseButtons
{
    Mouse_None,
    Mouse_Left,
    Mouse_Middle,
    Mouse_Right,
    Mouse_Extended1,
    Mouse_Extended2,
    MouseButtonCount
} MouseButtons;
typedef struct Mouse
{
    v2  pixelPosition;
    v2  relativePosition; // NOTE(michiel): (0, 0) is the center, (-1, -1) is top left, (1, 1) bottom right
    s32 scroll;           // NOTE(michiel): + for scroll up, - for down
    Key buttons[MouseButtonCount];
} Mouse;

typedef enum KeyModifiers
{
    KeyMod_None = 0x00,
    KeyMod_LeftCtrl = 0x01,
    KeyMod_RightCtrl = 0x02,
    KeyMod_Ctrl = 0x03,
    KeyMod_LeftShift = 0x04,
    KeyMod_RightShift = 0x08,
    KeyMod_Shift = 0x0C,
    KeyMod_LeftAlt = 0x10,
    KeyMod_RightAlt = 0x20,
    KeyMod_Alt = 0x30,
} KeyModifiers;
typedef struct Keyboard
{
    u8 lastInputData[128];
    Key keys[256];
    u32 modifiers;
    String lastInput;
} Keyboard;

internal b32
is_down(Mouse *mouse, MouseButtons button)
{
    i_expect(mouse);
    b32 result = mouse->buttons[button].isDown;
    return result;
}

internal b32
is_pressed(Mouse *mouse, MouseButtons button)
{
    i_expect(mouse);
    b32 result = mouse->buttons[button].isPressed;
    return result;
}

internal b32
is_released(Mouse *mouse, MouseButtons button)
{
    i_expect(mouse);
    b32 result = mouse->buttons[button].isReleased;
    return result;
}

internal b32
is_down(Keyboard *keyboard, Keys key)
{
    i_expect(keyboard);
    b32 result = keyboard->keys[key].isDown;
    return result;
}

internal b32
is_pressed(Keyboard *keyboard, Keys key)
{
    i_expect(keyboard);
    b32 result = keyboard->keys[key].isPressed;
    return result;
}

internal b32
is_released(Keyboard *keyboard, Keys key)
{
    i_expect(keyboard);
    b32 result = keyboard->keys[key].isReleased;
    return result;
}

internal b32
is_modified(Keyboard *keyboard, KeyModifiers mod)
{
    i_expect(keyboard);
    b32 result = keyboard->modifiers & (u32)mod;
    return result;
}

struct API
{
    MemoryAPI    memory;
    FileAPI      file;
    ThreadingAPI threading;

    b32 closeProgram;

    Mouse mouse;
    Keyboard keyboard;
};
