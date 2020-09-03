
//#include <fcntl.h>
//#include <dirent.h>

#ifndef LIBBERDIP_FILE_DEBUG
#define LIBBERDIP_FILE_DEBUG 0
#endif // LIBBERDIP_FILE_DEBUG

typedef struct LinuxFindFile
{
    DIR *dir;
    struct dirent *fileData;
} LinuxFindFile;

typedef struct LinuxFileGroup
{
    String wildcard;
    b32 fileAvailable;
    LinuxFindFile findData;
    s32 lastFileHandle;
} LinuxFileGroup;

internal b32
linux_find_file_in_folder(String wildcard, LinuxFindFile *finder)
{
    b32 result = false;
    if (finder->dir)
    {
        struct dirent *fileData = readdir(finder->dir);
        while (fileData)
        {
            if (match_pattern(wildcard, string(fileData->d_name)))
            {
                result = true;
                finder->fileData = fileData;
                break;
            }
            fileData = readdir(finder->dir);
        }
        if (!result)
        {
            finder->fileData = 0;
        }
    }
    return result;
}

internal
FILE_ERROR(linux_file_error)
{
    // TODO(michiel): Better define name
#if LIBBERDIP_FILE_DEBUG
    fprintf(stderr, "LINUX FILE ERROR: %.*s\n", STR_FMT(message));
#endif
    apiFile->noErrors = false;
}

internal inline s64
linux_file_size(s32 fileHandle)
{
    s64 size = 0;
    if (fileHandle >= 0)
    {
        off_t origPos = lseek(fileHandle, 0, SEEK_CUR);
        off_t fileSize = lseek(fileHandle, 0, SEEK_END);
        lseek(fileHandle, origPos, SEEK_SET);
        size = fileSize;
    }
    return size;
}

internal
GET_FILE_SIZE(linux_get_file_size)
{
    s32 handle = (s32)apiFile->platform;
    umm result = 0;
    if (handle)
    {
        s64 size = linux_file_size(handle);
        if (size > 0) {
            result = (umm)size;
        }
    }
    return result;
}

internal
GET_FILE_POSITION(linux_get_file_position)
{
    linuxHandle = (s32)apiFile->platform;
    off_t position = 0;
    if ((linuxHandle >= 0) && no_file_errors(apiFile))
    {
        position = lseek(linuxHandle, 0, SEEK_CUR);
    }
    return (u64)position;
}

internal
SET_FILE_POSITION(linux_set_file_position)
{
    s32 linuxHandle = (s32)apiFile->platform;
    if((linuxHandle >= 0) && no_file_errors(apiFile))
    {
        switch (type)
        {
            case FileCursor_StartOfFile: {
                lseek(linuxHandle, offset, SEEK_SET);
            } break;
            case FileCursor_CurrentPos: {
                lseek(linuxHandle, offset, SEEK_CUR);
            } break;
            case FileCursor_EndOfFile: {
                lseek(linuxHandle, offset, SEEK_END);
            } break;
            INVALID_DEFAULT_CASE;
        }
    }
}

internal
READ_ENTIRE_FILE(linux_read_entire_file)
{
    Buffer result = {};
    s32 fileHandle = open(to_cstring(filename), O_RDONLY);

    if (fileHandle >= 0)
    {
        s64 size = linux_file_size(fileHandle);
        if (size > 0)
        {
            Buffer allocated = allocate_size(allocator, size, Alloc_NoClear);
            result.data = allocated.data;
            if (result.data)
            {
                result.size = (umm)size;
                s64 bytesRead = read(fileHandle, result.data, result.size);
                if (bytesRead != result.size)
                {
                    deallocate(allocator, allocated);
                    result.size = 0;
                }
            }
        }
        close(fileHandle);
    }

    return result;
}

internal
WRITE_ENTIRE_FILE(linux_write_entire_file)
{
    b32 result = false;
    s32 fd = open(to_cstring(filename), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd >= 0)
    {
        ssize_t fileBytesWritten = write(fd, buffer.data, buffer.size);
        if (fsync(fd) >= 0)
        {
            result = (fileBytesWritten == (ssize_t)buffer.size);
        }
        else
        {
            result = false;
        }
        close(fd);
    }
    return result;
}

internal
GET_ALL_FILE_OF_TYPE_BEGIN(linux_get_all_files_of_type_begin)
{
    ApiFileGroup result = {};

    void *allocated = allocate_size(allocator, sizeof(LinuxFileGroup), 0);
    LinuxFileGroup *linuxFileGroup = (LinuxFileGroup *)allocated;
    result.platform = linuxFileGroup;
    result.fileCount = 0;
    linuxFileGroup->wildcard = matchPattern;

    LinuxFindFile findData;
    findData.dir = opendir(to_cstring(directory));
    for (b32 found = linux_find_file_in_folder(linuxFileGroup->wildcard, &findData);
         found;
         found = linux_find_file_in_folder(linuxFileGroup->wildcard, &findData))
    {
        ++result.fileCount;
    }
    closedir(findData.dir);

    linuxFileGroup->findData.dir = opendir(to_cstring(directory));
    linuxFileGroup->fileAvailable = linux_find_file_in_folder(linuxFileGroup->wildcard, &linuxFileGroup->findData);

    return result;
}

internal
GET_ALL_FILE_OF_TYPE_END(linux_get_all_files_of_type_end)
{
    LinuxFileGroup *linuxFileGroup = (LinuxFileGroup *)fileGroup->platform;
    if (linuxFileGroup)
    {
        if (linuxFileGroup->lastFileHandle >= 0)
        {
            close(linuxFileGroup->lastFileHandle);
            linuxFileGroup->lastFileHandle = -1;
        }
        if (linuxFileGroup->findData.dir)
        {
            closedir(linuxFileGroup->findData.dir);
            linuxFileGroup->findData.dir = 0;
        }

        linuxFileGroup = deallocate(fileGroup->allocator, linuxFileGroup);
    }
}

internal
OPEN_NEXT_FILE(linux_open_next_file)
{
    ApiFile result = {};

    LinuxFileGroup *linuxFileGroup = (LinuxFileGroup *)fileGroup->platform;

    if (linuxFileGroup->lastFileHandle >= 0)
    {
        close(linuxFileGroup->lastFileHandle);
        linuxFileGroup->lastFileHandle = -1;
    }

    if (linuxFileGroup->findData.dir)
    {
        s32 dirFd = dirfd(linuxFileGroup->findData.dir);

        char pathBuffer[1024];
        char *currentPath = getcwd(pathBuffer, sizeof(pathBuffer));

        fchdir(dirFd);
        if (linuxFileGroup->fileAvailable)
        {
            result.filename = string(linuxFileGroup->findData.fileData->d_name);
            s32 linuxHandle = open(to_cstring(result.filename), O_RDONLY);
            result.platform = (void *)linuxHandle;
            result.fileSize = safe_truncate_to_u32(linux_file_size(linuxHandle));
            result.noErrors = (linuxHandle >= 0);
            linuxFileGroup->lastFileHandle = linuxHandle;

            if (!linux_find_file_in_folder(linuxFileGroup->wildcard, &linuxFileGroup->findData))
            {
                linuxFileGroup->fileAvailable = false;
            }
        }
        chdir(currentPath);
    }

    return result;
}

internal
OPEN_FILE(linux_open_file)
{
    // NOTE(michiel): Make it save to call to_cstring()
    i_expect(filename.size < 1024);
    u8 filenameBuf[1024];
    copy(filename.size, filename.data, filenameBuf);
    filenameBuf[filename.size] = 0;
    filename.data = filenameBuf;

    ApiFile result = {};
    result.filename = filename;

    s32 linuxHandle = -1;
    if (flags == (FileOpen_Read | FileOpen_Write))
    {
        linuxHandle = open(to_cstring(filename), O_RDWR);
        result.fileSize = safe_truncate_to_u32(linux_file_size(handle->linuxHandle));
    }
    else if (flags == FileOpen_Read)
    {
        linuxHandle = open(to_cstring(filename), O_RDONLY);
        result.fileSize = safe_truncate_to_u32(linux_file_size(handle->linuxHandle));
    }
    else if (flags == FileOpen_Write)
    {
        linuxHandle = open(to_cstring(filename), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    }
    else
    {
        i_expect(flags == FileOpen_Append);
        linuxHandle = open(to_cstring(filename), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    }
    result.noErrors = (linuxHandle >= 0);
    result.platform = (void *)linuxHandle;

    return result;
}

internal
CLOSE_FILE(linux_close_file)
{
    if (apiFile->platform >= 0)
    {
        s32 linuxHandle = (s32)apiFile->platform;
        close(linuxHandle);
    }
    apiFile->platform = -1;
}

internal
READ_FROM_FILE(linux_read_from_file)
{
    umm result = 0;
    s32 linuxHandle = (s32)apiFile->platform;
    if((linuxHandle >= 0) && no_file_errors(apiFile))
    {
        s64 bytesRead = read(linuxHandle, buffer, size);
        if ((s64)size == bytesRead)
        {
            // NOTE(michiel): Success!!
            result = size;
        }
        else
        {
            linux_file_error(apiFile, static_string("Read file failed."));
        }
    }
    return result;
}

internal
READ_FROM_FILE_OFFSET(linux_read_from_file_offset)
{
    umm result = 0;
    s32 linuxHandle = (s32)apiFile->platform;
    if((linuxHandle >= 0) && no_file_errors(apiFile))
    {
#if 0
        linux_set_file_position(apiFile, 0, FileCursor_StartOfFile);
        linux_set_file_position(apiFile, offset, FileCursor_CurrentPos);
        s64 bytesRead = read(linuxHandle, buffer, size);
#else
        s64 bytesRead = pread(linuxHandle, buffer, size, offset);
#endif
        if ((s64)size == bytesRead)
        {
            // NOTE(michiel): Success!!
            result = size;
        }
        else
        {
            linux_file_error(apiFile, static_string("Read file with offset failed."));
        }
    }
    return result;
}

internal
WRITE_TO_FILE(linux_write_to_file)
{
    s32 linuxHandle = (s32)apiFile->platform;
    if((linuxHandle >= 0) && no_file_errors(apiFile))
    {
        ssize_t fileBytesWritten = write(linuxHandle, data, size);
        if (fileBytesWritten == (ssize_t)size)
        {
            // NOTE(michiel): Succes
        }
        else
        {
            linux_file_error(apiFile, static_string("Writing file failed."));
        }
    }
}

internal
WRITE_VFMT_TO_FILE(linux_write_vfmt_to_file)
{
    s32 linuxHandle = (s32)apiFile->platform;
    if((linuxHandle >= 0) && no_file_errors(apiFile))
    {
        char buffer[4096];
        // TODO(michiel): Remove snprintf
        vsnprintf(buffer, sizeof(buffer), fmt, args);

        ssize_t bufSize = string_length(buffer);
        ssize_t fileBytesWritten = write(linuxHandle, buffer, bufSize);
        if (fileBytesWritten == (ssize_t)bufSize)
        {
            // NOTE(michiel): Succes
        }
        else
        {
            linux_file_error(apiFile, static_string("Writing vfmt file failed."));
        }
    }
}

internal
WRITE_FMT_TO_FILE(linux_write_fmt_to_file)
{
    va_list args;
    va_start(args, fmt);
    linux_write_vfmt_to_file(apiFile, fmt, args);
    va_end(args);
}

internal INIT_FILE_API(linux_file_api)
{
    fileApi->file_error = linux_file_error;
    fileApi->read_entire_file = linux_read_entire_file;
    fileApi->write_entire_file = linux_write_entire_file;
    fileApi->get_all_files_of_type_begin = linux_get_all_files_of_type_begin;
    fileApi->get_all_files_of_type_end = linux_get_all_files_of_type_end;
    fileApi->open_next_file = linux_open_next_file;
    fileApi->open_file = linux_open_file;
    fileApi->close_file = linux_close_file;
    fileApi->get_file_size = linux_get_file_size;
    fileApi->get_file_position = linux_get_file_position;
    fileApi->set_file_position = linux_set_file_position;
    fileApi->read_from_file = linux_read_from_file;
    fileApi->read_from_file_offset = linux_read_from_file_offset;
    fileApi->write_to_file = linux_write_to_file;
    fileApi->write_fmt_to_file = linux_write_fmt_to_file;
    fileApi->write_vfmt_to_file = linux_write_vfmt_to_file;
}
