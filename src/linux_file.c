// NOTE(michiel): Uses <fcntl.h> and <dirent.h>, and <sys/mman.h> but that should go away

#ifndef LIBBERDIP_FILE_DEBUG
#define LIBBERDIP_FILE_DEBUG 0
#endif // LIBBERDIP_FILE_DEBUG

typedef struct LinuxFindFile
{
    DIR *dir;
    struct dirent *fileData;
} LinuxFindFile;

typedef struct LinuxFileHandle
{
    s32 linuxHandle;
} LinuxFileHandle;

typedef struct LinuxFileGroup
{
    String wildcard;
    b32 fileAvailable;
    LinuxFindFile findData;
    LinuxFileHandle *lastFileHandle;
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
    LinuxFileHandle *handle = (LinuxFileHandle *)apiFile->platform;
    umm result = 0;
    if (handle)
    {
        s64 size = linux_file_size(handle->linuxHandle);
        if (size > 0) {
            result = (umm)size;
        }
    }
    return result;
}

internal
GET_FILE_POSITION(linux_get_file_position)
{
    LinuxFileHandle *linuxHandle = (LinuxFileHandle *)apiFile->platform;
    off_t position = 0;
    if (linuxHandle && no_file_errors(apiFile))
    {
        i_expect(linuxHandle->linuxHandle >= 0);
        position = lseek(linuxHandle->linuxHandle, 0, SEEK_CUR);
    }
    return (u64)position;
}

internal
SET_FILE_POSITION(linux_set_file_position)
{
    LinuxFileHandle *linuxHandle = (LinuxFileHandle *)apiFile->platform;
    if(linuxHandle && no_file_errors(apiFile))
    {
        i_expect(linuxHandle->linuxHandle >= 0);
        switch (type)
        {
            case FileCursor_StartOfFile: {
                lseek(linuxHandle->linuxHandle, offset, SEEK_SET);
            } break;
            case FileCursor_CurrentPos: {
                lseek(linuxHandle->linuxHandle, offset, SEEK_CUR);
            } break;
            case FileCursor_EndOfFile: {
                lseek(linuxHandle->linuxHandle, offset, SEEK_END);
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
            // TODO(michiel): Api allocate
            Buffer allocated = linux_allocate_size(size, Alloc_NoClear);
            result.data = allocated.data;
            if (result.data)
            {
                result.size = (umm)size;
                s64 bytesRead = read(fileHandle, result.data, result.size);
                if (bytesRead != result.size)
                {
                    linux_deallocate_size(allocated);
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

    Buffer allocated = linux_allocate_size(sizeof(LinuxFileGroup), 0);
    LinuxFileGroup *linuxFileGroup = (LinuxFileGroup *)allocated.data;
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
        if (linuxFileGroup->lastFileHandle)
        {
            close(linuxFileGroup->lastFileHandle->linuxHandle);
            Buffer deallocateMem = {sizeof(LinuxFileHandle), (u8 *)linuxFileGroup->lastFileHandle};
            linux_deallocate_size(deallocateMem);
        }
        if (linuxFileGroup->findData.dir)
        {
            closedir(linuxFileGroup->findData.dir);
            linuxFileGroup->findData.dir = 0;
        }

        Buffer deallocateMem = {sizeof(LinuxFileGroup), (u8 *)linuxFileGroup};
        linux_deallocate_size(deallocateMem);
        linuxFileGroup = 0;
    }
}


internal
OPEN_NEXT_FILE(linux_open_next_file)
{
    ApiFile result = {};

    LinuxFileGroup *linuxFileGroup = (LinuxFileGroup *)fileGroup->platform;

    LinuxFileHandle *useHandle = 0;
    if (linuxFileGroup->lastFileHandle)
    {
        close(linuxFileGroup->lastFileHandle->linuxHandle);
        useHandle = linuxFileGroup->lastFileHandle;
        linuxFileGroup->lastFileHandle = 0;
    }

    if (linuxFileGroup->findData.dir)
    {
        s32 dirFd = dirfd(linuxFileGroup->findData.dir);

        char pathBuffer[1024];
        char *currentPath = getcwd(pathBuffer, sizeof(pathBuffer));

        fchdir(dirFd);
        if (linuxFileGroup->fileAvailable)
        {
            if (!useHandle)
            {
                Buffer allocated = linux_allocate_size(sizeof(LinuxFileHandle), 0);
                useHandle = (LinuxFileHandle *)allocated.data;
            }
            result.platform = useHandle;

            if (useHandle)
            {
                result.filename = string(linuxFileGroup->findData.fileData->d_name);
                useHandle->linuxHandle = open(to_cstring(result.filename), O_RDONLY);
                result.fileSize = safe_truncate_to_u32(linux_file_size(useHandle->linuxHandle));
                result.noErrors = (useHandle->linuxHandle >= 0);
                linuxFileGroup->lastFileHandle = useHandle;
            }

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

    Buffer allocated = linux_allocate_size(sizeof(LinuxFileHandle), 0);
    LinuxFileHandle *handle = (LinuxFileHandle *)allocated.data;
    result.platform = handle;
    result.filename = filename;

    if (handle)
    {
        if (flags == (FileOpen_Read | FileOpen_Write))
        {
            handle->linuxHandle = open(to_cstring(filename), O_RDWR);
            result.fileSize = safe_truncate_to_u32(linux_file_size(handle->linuxHandle));
        }
        else if (flags == FileOpen_Read)
        {
            handle->linuxHandle = open(to_cstring(filename), O_RDONLY);
            result.fileSize = safe_truncate_to_u32(linux_file_size(handle->linuxHandle));
        }
        else if (flags == FileOpen_Write)
        {
            handle->linuxHandle = open(to_cstring(filename), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        }
        else
        {
            i_expect(flags == FileOpen_Append);
            handle->linuxHandle = open(to_cstring(filename), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
        }
        result.noErrors = (handle->linuxHandle >= 0);
    }

    return result;
}

internal
CLOSE_FILE(linux_close_file)
{
    if (apiFile->platform)
    {
        LinuxFileHandle *linuxHandle = (LinuxFileHandle *)apiFile->platform;
        close(linuxHandle->linuxHandle);
        Buffer deallocateMem = {sizeof(LinuxFileHandle), (u8 *)apiFile->platform};
        linux_deallocate_size(deallocateMem);
    }
    apiFile->platform = 0;
}

internal
READ_FROM_FILE(linux_read_from_file)
{
    umm result = 0;
    LinuxFileHandle *linuxHandle = (LinuxFileHandle *)apiFile->platform;
    if(linuxHandle && no_file_errors(apiFile))
    {
        i_expect(linuxHandle->linuxHandle >= 0);
        s64 bytesRead = read(linuxHandle->linuxHandle, buffer, size);
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
    LinuxFileHandle *linuxHandle = (LinuxFileHandle *)apiFile->platform;
    if(linuxHandle && no_file_errors(apiFile))
    {
        i_expect(linuxHandle->linuxHandle >= 0);
#if 0
        linux_set_file_position(apiFile, 0, FileCursor_StartOfFile);
        linux_set_file_position(apiFile, offset, FileCursor_CurrentPos);
        s64 bytesRead = read(linuxHandle->linuxHandle, buffer, size);
#else
        s64 bytesRead = pread(linuxHandle->linuxHandle, buffer, size, offset);
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
    LinuxFileHandle *linuxHandle = (LinuxFileHandle *)apiFile->platform;
    if(linuxHandle && no_file_errors(apiFile))
    {
        i_expect(linuxHandle->linuxHandle >= 0);

        ssize_t fileBytesWritten = write(linuxHandle->linuxHandle, data, size);
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
    LinuxFileHandle *linuxHandle = (LinuxFileHandle *)apiFile->platform;
    if(linuxHandle && no_file_errors(apiFile))
    {
        i_expect(linuxHandle->linuxHandle >= 0);

        char buffer[4096];
        // TODO(michiel): Remove snprintf
        vsnprintf(buffer, sizeof(buffer), fmt, args);

        ssize_t bufSize = string_length(buffer);
        ssize_t fileBytesWritten = write(linuxHandle->linuxHandle, buffer, bufSize);
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
