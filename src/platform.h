#include "./common.h"
#include "./intrinsics.h"
#include "./maps.h"
#include "./strings.h"
#include "./maths.h"
#include "./vectors.h"
#include "./keys.h"
#include "./fonts.h"
#include "./files.h"

//
// NOTE(michiel): Threading
//

struct TicketMutex
{
    u64 volatile ticket;
    u64 volatile serving;
};

struct WorkQueue;
#define WORK_QUEUE_CALLBACK(name)      void name(WorkQueue *queue, void *data)
typedef WORK_QUEUE_CALLBACK(WorkQueueCallback);

#define ADD_WORK_ENTRY(name)           void name(WorkQueue *queue, \
WorkQueueCallback *callback, \
void *data)
typedef ADD_WORK_ENTRY(AddWorkEntry);

#define COMPLETE_ALL_WORK(name)        void name(WorkQueue *queue)
typedef COMPLETE_ALL_WORK(CompleteAllWork);

typedef enum MouseButtons
{
    Mouse_Left      = 0x01,
    Mouse_Middle    = 0x02,
    Mouse_Right     = 0x04,
    Mouse_Extended1 = 0x08,
    Mouse_Extended2 = 0x10,
} MouseButtons;
typedef struct Mouse
{
    v2  pixelPosition;
    v2  relativePosition; // NOTE(michiel): 0, 0 is is topleft (or should it be 0, 1)
    s32 scroll; // NOTE(michiel): + for scroll up, - for down
    u32 mouseDowns;
} Mouse;

typedef struct Key
{
    b8 isDown;
    b8 isPressed;
    b8 isReleased;
    u8 edgeCount;
} Key;
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
is_down(Keyboard *keyboard, Keys key, b32 shouldBeFocused = true)
{
    i_expect(keyboard);
    b32 result = keyboard->keys[key].isDown;
    return result;
}

struct API
{
    FileAPI file;

    b32 closeProgram;

    Mouse mouse;
    Keyboard keyboard;
};
