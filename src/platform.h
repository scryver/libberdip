#include "./common.h"
#include "./maths.h"
#include "./vectors.h"
#include "./fonts.h"

typedef enum FileOpenType
{
    FileOpen_Read = 0x1,
    FileOpen_Write = 0x2,
} FileOpenType;
typedef struct ApiFile
{
    u64 handle;
    Buffer content;
} ApiFile;

typedef struct FileStream
{
    b32 verbose;
    u32 indent;
    ApiFile file;
} FileStream;
// TODO(michiel): This can be cleaned up if we have some file print version
#define stdfile2stream(f)  ((FileStream){0, 0, {u64_from_ptr(f), {0}}})
#define stream2stdfile(fs) ((FILE *)fs.file.handle)

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
    v2u pixelPosition;
    v2  relativePosition; // NOTE(michiel): 0, 0 is is topleft (or should it be 0, 1)
    u32 mouseDowns;
} Mouse;

// TODO(michiel): Use String
#define READ_ENTIRE_FILE(name) ApiFile name(char *filename)
typedef READ_ENTIRE_FILE(ReadEntireFile);

#define WRITE_ENTIRE_FILE(name) void name(char *filename, umm size, void *data)
typedef WRITE_ENTIRE_FILE(WriteEntireFile);

#define OPEN_FILE(name) ApiFile name(char *filename, u32 flags)
typedef OPEN_FILE(OpenFile);

// TODO(michiel): Should we return something instead of filling the buffer?
#define READ_FROM_FILE(name) void name(ApiFile file, umm size, void *buffer)
typedef READ_FROM_FILE(ReadFromFile);

#define WRITE_TO_FILE(name) void name(ApiFile file, umm size, void *data)
typedef WRITE_TO_FILE(WriteToFile);

#define CLOSE_FILE(name) void name(ApiFile *file)
typedef CLOSE_FILE(CloseFile);

typedef struct FileAPI
{
    ReadEntireFile *read_entire_file;
    WriteEntireFile *write_entire_file;
    OpenFile *open_file;
    ReadFromFile *read_from_file;
    WriteToFile *write_to_file;
    CloseFile *close_file;
} FileAPI;

struct API
{
    FileAPI file;
    
    b32 closeProgram;
    
    Mouse mouse;
};
