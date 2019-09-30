// NOTE(michiel): Uses <fcntl.h> and <dirent.h>, and <mmap.h> but that should go away

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
#if LIBBERDIP_EXPECT
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
    s64 size = linux_file_size(handle->linuxHandle);
    if (size > 0) {
        result = (umm)size;
    }
    return result;
}

internal
GET_FILE_POSITION(linux_get_file_position)
{
    LinuxFileHandle *linuxHandle = (LinuxFileHandle *)apiFile->platform;
    off_t position = lseek(linuxHandle->linuxHandle, 0, SEEK_CUR);
    return (u64)position;
}

internal
SET_FILE_POSITION(linux_set_file_position)
{
    if(no_file_errors(apiFile))
    {
        LinuxFileHandle *linuxHandle = (LinuxFileHandle *)apiFile->platform;

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
    s32 fd = open(to_cstring(filename), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
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

    // TODO(michiel): Fix memory
    LinuxFileGroup *linuxFileGroup = (LinuxFileGroup *)
        mmap(0, sizeof(LinuxFileGroup),
             PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
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
        if (linuxFileGroup->findData.dir)
        {
            closedir(linuxFileGroup->findData.dir);
            linuxFileGroup->findData.dir = 0;
        }

        // TODO(michiel): Fix memory
        munmap(linuxFileGroup, sizeof(LinuxFileGroup));
        linuxFileGroup = 0;
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
        fchdir(dirFd);
        if (linuxFileGroup->fileAvailable)
        {
            LinuxFileHandle *handle = (LinuxFileHandle *)
                mmap(0, sizeof(LinuxFileHandle),
                     PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            result.platform = handle;

            if (handle)
            {
                result.filename = string(linuxFileGroup->findData.fileData->d_name);
                handle->linuxHandle = open(to_cstring(result.filename), O_RDONLY);
                result.fileSize = safe_truncate_to_u32(linux_file_size(handle->linuxHandle));
                result.noErrors = (handle->linuxHandle >= 0);
                linuxFileGroup->lastFileHandle = handle->linuxHandle;
            }

            if (!linux_find_file_in_folder(linuxFileGroup->wildcard, &linuxFileGroup->findData))
            {
                linuxFileGroup->fileAvailable = false;
            }
        }
        chdir("-");
    }

    return result;
}

internal
OPEN_FILE(linux_open_file)
{
    ApiFile result = {};

    LinuxFileHandle *handle = (LinuxFileHandle *)
        mmap(0, sizeof(LinuxFileHandle),
             PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
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
            handle->linuxHandle = open(to_cstring(filename), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        }
        else
        {
            i_expect(flags == FileOpen_Append);
            handle->linuxHandle = open(to_cstring(filename), O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
        }
        result.noErrors = (handle->linuxHandle >= 0);
    }

    return result;
}

internal
CLOSE_FILE(linux_close_file)
{
    LinuxFileHandle *linuxHandle = (LinuxFileHandle *)apiFile->platform;
    close(linuxHandle->linuxHandle);
    munmap(linuxHandle, sizeof(LinuxFileHandle));
    apiFile->platform = 0;
}

internal
READ_FROM_FILE(linux_read_from_file)
{
    umm result = 0;
    if(no_file_errors(apiFile))
    {
        LinuxFileHandle *linuxHandle = (LinuxFileHandle *)apiFile->platform;

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
    if(no_file_errors(apiFile))
    {
        LinuxFileHandle *linuxHandle = (LinuxFileHandle *)apiFile->platform;

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
    if(no_file_errors(apiFile))
    {
        LinuxFileHandle *linuxHandle = (LinuxFileHandle *)apiFile->platform;

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
    if(no_file_errors(apiFile))
    {
        LinuxFileHandle *linuxHandle = (LinuxFileHandle *)apiFile->platform;

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
