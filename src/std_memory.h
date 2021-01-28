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
#if COMPILER_MSVC_X86
    u64 padding[5];
#else
    u64 padding[2];
#endif
};
compile_expect(sizeof(StdMemoryBlock) == MEMORY_PLATFORM_REAL_SIZE);

struct StdGlobalMemory
{
    TicketMutex mutex;
    StdMemoryBlock sentinel;
};

global StdGlobalMemory gStdMemory;
