//
// NOTE(michiel): Files
//

// TODO(michiel): Add an atomic replace file function (write to temp and then a rename)

#define MAKE_MAGIC(a, b, c, d)    ((d << 24) | (c << 16) | (b << 8) | a)

typedef enum FileOpenType
{
    FileOpen_Read = 0x1,
    FileOpen_Write = 0x2,
    FileOpen_Append = 0x4, // NOTE(michiel): Only for writing
} FileOpenType;
typedef struct ApiFile
{
    b32 noErrors;
    u64 fileSize;
    String filename;
    void *platform;
} ApiFile;

typedef struct ApiFileGroup
{
    MemoryAllocator *allocator;
    u32 fileCount;
    void *platform;
} ApiFileGroup;

typedef enum FileDirKind
{
    FileDir_None,
    FileDir_File,
    FileDir_Directory,
} FileDirKind;
typedef struct ApiFileDir
{
    FileDirKind kind;
    String name;
} ApiFileDir;

typedef struct ApiFileDirGroup
{
    String basePath;
    u32 fileDirCount;
    ApiFileDir *fileDirs;
} ApiFileDirGroup;

typedef enum FileCursorReference
{
    FileCursor_StartOfFile,
    FileCursor_CurrentPos,
    FileCursor_EndOfFile,
} FileCursorReference;

typedef struct FileStream
{
    u32 verbose;
    u32 indent;
    ApiFile file;
} FileStream;
// TODO(michiel): This can be cleaned up if we have some file print version
#define stdfile2stream(f)  ((FileStream){0, 0, {0, 0, {}, f}})
#define stream2stdfile(fs) ((FILE *)fs.file.platform)

#define FILE_ERROR(name) void name(ApiFile *apiFile, String message)
typedef FILE_ERROR(FileError);

#define GET_FILE_SIZE(name) umm name(ApiFile *apiFile)
typedef GET_FILE_SIZE(GetFileSize_); // NOTE(michiel): MSVC conflict

#define GET_FILE_POSITION(name) u64 name(ApiFile *apiFile)
typedef GET_FILE_POSITION(GetFilePosition);

#define SET_FILE_POSITION(name) void name(ApiFile *apiFile, u64 offset, \
FileCursorReference type)
typedef SET_FILE_POSITION(SetFilePosition);

// NOTE(michiel): All in one operation. Read the whole file and allocate the buffer
#define READ_ENTIRE_FILE(name) Buffer name(MemoryAllocator *allocator, String filename)
typedef READ_ENTIRE_FILE(ReadEntireFile);

// NOTE(michiel): All in one operation. Write the whole file.
#define WRITE_ENTIRE_FILE(name) b32 name(String filename, Buffer buffer)
typedef WRITE_ENTIRE_FILE(WriteEntireFile);

// NOTE(michiel): Open all of type
#define GET_ALL_FILE_OF_TYPE_BEGIN(name) ApiFileGroup name(MemoryAllocator *allocator, String directory, String matchPattern)
typedef GET_ALL_FILE_OF_TYPE_BEGIN(GetAllFileOfTypeBegin);

#define GET_ALL_FILE_OF_TYPE_END(name) void name(ApiFileGroup *fileGroup)
typedef GET_ALL_FILE_OF_TYPE_END(GetAllFileOfTypeEnd);

#define OPEN_NEXT_FILE(name) ApiFile name(ApiFileGroup *fileGroup)
typedef OPEN_NEXT_FILE(OpenNextFile);

// NOTE(michiel): Directory listing
#define GET_ALL_IN_DIR(name) ApiFileDirGroup *name(MemoryAllocator *allocator, String directory)
typedef GET_ALL_IN_DIR(GetAllInDir);

// NOTE(michiel): Open single file
#define OPEN_FILE(name) ApiFile name(String filename, u32 flags)
typedef OPEN_FILE(OpenFile_); // NOTE(NAME): MSVC conflict

#define READ_FROM_FILE(name) umm name(ApiFile *apiFile, umm size, void *buffer)
typedef READ_FROM_FILE(ReadFromFile);

#define READ_FROM_FILE_OFFSET(name) umm name(ApiFile *apiFile, umm offset, \
umm size, void *buffer)
typedef READ_FROM_FILE_OFFSET(ReadFromFileOffset);

#define WRITE_TO_FILE(name) void name(ApiFile *apiFile, umm size, void *data)
typedef WRITE_TO_FILE(WriteToFile);

#define WRITE_FMT_TO_FILE(name) void name(ApiFile *apiFile, char *fmt, ...)
typedef WRITE_FMT_TO_FILE(WriteFmtToFile);

#define WRITE_VFMT_TO_FILE(name) void name(ApiFile *apiFile, \
char *fmt, va_list args)
typedef WRITE_VFMT_TO_FILE(WriteVFmtToFile);

#define CLOSE_FILE(name) void name(ApiFile *apiFile)
typedef CLOSE_FILE(CloseFile);

#define no_file_errors(handle) ((handle)->noErrors)

typedef struct FileAPI
{
    char *pathSep;

    FileError *file_error;

    ReadEntireFile *read_entire_file;
    WriteEntireFile *write_entire_file;

    GetAllFileOfTypeBegin *get_all_files_of_type_begin;
    GetAllFileOfTypeEnd *get_all_files_of_type_end;
    OpenNextFile *open_next_file;

    GetAllInDir *get_all_in_dir;

    OpenFile_ *open_file;
    CloseFile *close_file;

    GetFileSize_ *get_file_size;
    GetFilePosition *get_file_position;
    SetFilePosition *set_file_position;

    ReadFromFile *read_from_file;
    ReadFromFileOffset *read_from_file_offset;

    WriteToFile *write_to_file;
    WriteFmtToFile *write_fmt_to_file;
    WriteVFmtToFile *write_vfmt_to_file;
} FileAPI;

#define INIT_FILE_API(name) void name(FileAPI *fileApi)
typedef INIT_FILE_API(InitFileAPI);
