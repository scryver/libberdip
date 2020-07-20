struct TicketMutex
{
    u64 volatile ticket;
    u64 volatile serving;
};

#if COMPILER_MSVC
#include <intrin.h>

#define COMPLETE_PREVIOUS_READS_BEFORE_FUTURE_READS     _ReadBarrier()
#define COMPLETE_PREVIOUS_WRITES_BEFORE_FUTURE_WRITES   _WriteBarrier()

internal inline u32 atomic_compare_exchange_u32(u32 volatile *value, u32 new_value, u32 expected)
{
    u32 result = _InterlockedCompareExchange((long volatile *)value, new_value, expected);
    return (result);
}

internal inline u64 atomic_compare_exchange_u64(u64 volatile *value, u64 new_value, u64 expected)
{
    u64 result = _InterlockedCompareExchange64((__int64  volatile *)value, new_value, expected);
    return (result);
}

internal inline u64 atomic_exchange_u64(u64 volatile *value, u64 new_value)
{
    u64 result = _InterlockedExchange64((__int64 volatile *)value, new_value);
    return result;
}

internal inline u64 atomic_add_u64(u64 volatile *value, u64 addend)
{
    // NOTE(michiel): Returns the original value _prior_ to adding
    u64 result = _InterlockedExchangeAdd64((__int64 volatile *)value, addend);
    return result;
}

internal inline u32 atomic_add_u32(u32 volatile *value, u32 addend)
{
    // NOTE(michiel): Returns the original value _prior_ to adding
    u32 result = _InterlockedExchangeAdd((long *)value, addend);
    return result;
}

internal inline u32 get_thread_id(void)
{
    u8 *thread_local_storage = (u8 *)__readgsqword(0x30);
    u32 thread_id = *(u32 *)(thread_local_storage + 0x48);
    return thread_id;
}

#elif COMPILER_LLVM || COMPILER_GCC || COMPILER_TCC
#include <x86intrin.h>

#define COMPLETE_PREVIOUS_READS_BEFORE_FUTURE_READS     asm volatile("" ::: "memory")
#define COMPLETE_PREVIOUS_WRITES_BEFORE_FUTURE_WRITES   asm volatile("" ::: "memory")

internal inline u32 atomic_compare_exchange_u32(u32 volatile *value, u32 new_value, u32 expected)
{
    u32 result = __sync_val_compare_and_swap(value, expected, new_value);
    return (result);
}

internal inline u64 atomic_compare_exchange_u64(u64 volatile *value, u64 new_value, u64 expected)
{
    u64 result = __sync_val_compare_and_swap(value, expected, new_value);
    return (result);
}

internal inline u64 atomic_exchange_u64(u64 volatile *value, u64 new_value)
{
    u64 result = __sync_lock_test_and_set(value, new_value);
    return result;
}

internal inline u64 atomic_add_u64(u64 volatile *value, u64 addend)
{
    // NOTE(michiel): Returns the original value _prior_ to adding
    u64 result = __sync_fetch_and_add(value, addend);
    return result;
}

internal inline u32 atomic_add_u32(u32 volatile *value, u32 addend)
{
    // NOTE(michiel): Returns the original value _prior_ to adding
    u32 result = __sync_fetch_and_add(value, addend);
    return result;
}

internal inline u32 get_thread_id(void)
{
    u32 thread_id;
#if defined(__APPLE__) && defined(__x86_64__)
    asm("mov %%gs:0x00,%0" : "=r"(thread_id));
#elif defined(__i386__)
    asm("mov %%gs:0x08,%0" : "=r"(thread_id));
#elif defined(__x86_64__)
    asm("mov %%fs:0x10,%0" : "=r"(thread_id));
#else
#error Unsupported architecture
#endif
    return (thread_id);
}
#else
#error SSE/NEON optimizations are not available for this compiler yet!!!!
#endif

internal void
atomic_lock(u32 volatile *lock)
{
    while (atomic_compare_exchange_u32(lock, 1, 0) != 0) {
        _mm_pause();
    }
}

internal void
atomic_unlock(u32 volatile *lock)
{
    COMPLETE_PREVIOUS_WRITES_BEFORE_FUTURE_WRITES;
    *lock = 0;
}

internal void
begin_ticket_mutex(TicketMutex *mutex)
{
    u64 ticket = atomic_add_u64(&mutex->ticket, 1);
    while (ticket != mutex->serving) {
        _mm_pause();
    }
}

internal void
end_ticket_mutex(TicketMutex *mutex)
{
    atomic_add_u64(&mutex->serving, 1);
}
