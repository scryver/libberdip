//
// NOTE(michiel): Standard library global memory
//

// #include <stdlib.h>

struct StdMemoryBlock
{
    // TODO(michiel): Maybe move this to the end, so it is easier for usage code to get from memory to the platform block
    PlatformMemoryBlock block;
    StdMemoryBlock *prev;
    StdMemoryBlock *next;
    u64 padding[2];
};
compile_expect(sizeof(StdMemoryBlock) == MEMORY_PLATFORM_REAL_SIZE);

struct StdGlobalMemory
{
    TicketMutex mutex;
    StdMemoryBlock sentinel;
};

global StdGlobalMemory gStdMemory;
