struct PlatformWorkQueue
{
    u32 volatile completionGoal;
    u32 volatile completionCount;

    u32 volatile nextEntryToWrite;
    u32 volatile nextEntryToRead;
    sem_t        semaphoreHandle;

    PlatformWorkQueueEntry entries[MAX_WORK_QUEUE_ENTRIES];
};

internal PLATFORM_ADD_WORK_ENTRY(linux_add_work_entry)
{
    u32 newNextEntryTowrite = (queue->nextEntryToWrite + 1) % array_count(queue->entries);

    i_expect(newNextEntryTowrite != queue->nextEntryToRead);
    PlatformWorkQueueEntry *entry = queue->entries + queue->nextEntryToWrite;
    entry->callback = callback;
    entry->data = data;
    ++queue->completionGoal;

    asm volatile("" ::: "memory"); // NOTE(michiel): Complete previous writes

    queue->nextEntryToWrite = newNextEntryTowrite;
    sem_post(&queue->semaphoreHandle);
}

internal b32
linux_do_next_work_queue_entry(PlatformWorkQueue *queue)
{
    b32 weShouldSleep = false;

    u32 originalNextEntryToRead = queue->nextEntryToRead;
    u32 newNextEntryToRead = (queue->nextEntryToRead + 1) % array_count(queue->entries);
    if (originalNextEntryToRead != queue->nextEntryToWrite)
    {
        u32 index = __sync_val_compare_and_swap(&queue->nextEntryToRead,
                                                originalNextEntryToRead,
                                                newNextEntryToRead);
        if (index == originalNextEntryToRead)
        {
            PlatformWorkQueueEntry entry = queue->entries[index];
            entry.callback(queue, entry.data);
            __sync_fetch_and_add(&queue->completionCount, 1);
        }
    }
    else
    {
        weShouldSleep = true;
    }

    return weShouldSleep;
}

internal PLATFORM_COMPLETE_ALL_WORK(linux_complete_all_work)
{
    while (queue->completionGoal != queue->completionCount)
    {
        linux_do_next_work_queue_entry(queue);
    }
    queue->completionGoal = 0;
    queue->completionCount = 0;
}

internal void *
linux_thread_proc(void *parameter)
{
    PlatformWorkQueue *queue = (PlatformWorkQueue *)parameter;

    for (;;)
    {
        if (linux_do_next_work_queue_entry(queue))
        {
            sem_wait(&queue->semaphoreHandle);
        }
    }

    return 0;
}

internal PLATFORM_CREATE_WORK_QUEUE(linux_create_work_queue)
{
    queue->completionGoal = 0;
    queue->completionCount = 0;

    queue->nextEntryToWrite = 0;
    queue->nextEntryToRead = 0;

    u32 initialCount = 0;
    sem_init(&queue->semaphoreHandle, 0, initialCount);

    for (u32 threadIndex = 0; threadIndex < threadCount; ++threadIndex)
    {
        pthread_attr_t attr;
        pthread_t tid;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        int result = pthread_create(&tid, &attr, linux_thread_proc, queue);
        unused(result); // TODO(michiel): Check for errors
        pthread_attr_destroy(&attr);
    }
}

internal INIT_THREADING_API(linux_threading_api)
{
    threadingApi->create_work_queue = linux_create_work_queue;
    threadingApi->add_work_entry = linux_add_work_entry;
    threadingApi->complete_all_work = linux_complete_all_work;
}
