//
// NOTE(michiel): Linux memory
//

// #include <unistd.h>
// #include <sys/mman.h>

struct LinuxMemoryBlock
{
    // TODO(michiel): Maybe move this to the end, so it is easier for usage code to get from memory to the platform block
    PlatformMemoryBlock block;
    LinuxMemoryBlock *prev;
    LinuxMemoryBlock *next;
    u64 padding[2];
};
compile_expect(sizeof(LinuxMemoryBlock) == MEMORY_PLATFORM_REAL_SIZE);

struct LinuxGlobalMemory
{
    TicketMutex mutex;
    LinuxMemoryBlock sentinel;
};

global LinuxGlobalMemory gLinuxMemory;
