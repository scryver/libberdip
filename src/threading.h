//
// NOTE(michiel): Threading
//

#ifndef MAX_WORK_QUEUE_ENTRIES
#define MAX_WORK_QUEUE_ENTRIES 256
#endif

struct PlatformWorkQueue;
#define PLATFORM_WORK_QUEUE_CALLBACK(name)      void name(PlatformWorkQueue *queue, void *data)
typedef PLATFORM_WORK_QUEUE_CALLBACK(PlatformWorkQueueCallback);

#define PLATFORM_CREATE_WORK_QUEUE(name)        void name(PlatformWorkQueue *queue, u32 threadCount)
typedef PLATFORM_CREATE_WORK_QUEUE(PlatformCreateWorkQueue);

#define PLATFORM_ADD_WORK_ENTRY(name)           void name(PlatformWorkQueue *queue, PlatformWorkQueueCallback *callback, void *data)
typedef PLATFORM_ADD_WORK_ENTRY(PlatformAddWorkEntry);

#define PLATFORM_COMPLETE_ALL_WORK(name)        void name(PlatformWorkQueue *queue)
typedef PLATFORM_COMPLETE_ALL_WORK(PlatformCompleteAllWork);

typedef struct PlatformWorkQueueEntry
{
    PlatformWorkQueueCallback *callback;
    void *data;
} PlatformWorkQueueEntry;

typedef struct ThreadingAPI
{
    PlatformCreateWorkQueue *create_work_queue;
    PlatformAddWorkEntry    *add_work_entry;
    PlatformCompleteAllWork *complete_all_work;
} ThreadingAPI;

#define INIT_THREADING_API(name)  void name(ThreadingAPI *threadingApi)
typedef INIT_THREADING_API(InitThreadingAPI);
