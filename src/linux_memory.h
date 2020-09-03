//
// NOTE(michiel): Linux memory
//

// #include <unistd.h>
// #include <sys/mman.h>

struct LinuxMemoryBlock
{
    PlatformMemoryBlock block;
    LinuxMemoryBlock *prev;
    LinuxMemoryBlock *next;
    u64 padding[2];
};
compile_expect(sizeof(LinuxMemoryBlock) == 64);

struct LinuxGlobalMemory
{
    TicketMutex mutex;
    LinuxMemoryBlock sentinel;
};

global LinuxGlobalMemory gLinuxMemory;
