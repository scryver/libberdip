//
// NOTE(michiel): Standard library global memory
//

// #include <stdlib.h>

struct StdMemoryBlock
{
    PlatformMemoryBlock block;
    StdMemoryBlock *prev;
    StdMemoryBlock *next;
    u64 padding[2];
};
compile_expect(sizeof(StdMemoryBlock) == 64);

struct StdGlobalMemory
{
    TicketMutex mutex;
    StdMemoryBlock sentinel;
};

global StdGlobalMemory gStdMemory;
