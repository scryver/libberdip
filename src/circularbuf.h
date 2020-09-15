struct CircularBuffer
{
    u32 mask;      // NOTE(michiel): total size is pow(2), mask = total size - 1
    u32 readIndex;
    u32 writeIndex;
    u8  buffer[8]; // NOTE(michiel): At _least_ 8
};

internal CircularBuffer *alloc_circular_buffer(MemoryAllocator *allocator, u32 size);
// NOTE(michiel): returns number of bytes written
internal u32 circular_add(CircularBuffer *buffer, u32 sourceCount, u8 *source);
// NOTE(michiel): returns number of bytes read
internal u32 circular_get(CircularBuffer *buffer, u32 maxDataCount, u8 *dest);

internal u32
get_size(CircularBuffer *buffer)
{
    u32 result = (buffer->writeIndex - buffer->readIndex) & buffer->mask;
    return result;
}

internal u32
get_available_size(CircularBuffer *buffer)
{
    return buffer->mask - get_size(buffer);
}

internal void
circular_add_byte(CircularBuffer *buffer, u8 byte)
{
    i_expect(get_available_size(buffer));
    buffer->buffer[buffer->writeIndex] = byte;
    buffer->writeIndex = (buffer->writeIndex + 1) & buffer->mask;
}

internal u8
circular_get_byte(CircularBuffer *buffer)
{
    i_expect(get_size(buffer));
    u8 result = buffer->buffer[buffer->readIndex];
    buffer->readIndex = (buffer->readIndex + 1) & buffer->mask;
    return result;
}

internal u8
circular_unadd_byte(CircularBuffer *buffer)
{
    i_expect(get_size(buffer));
    buffer->writeIndex = (buffer->writeIndex - 1) & buffer->mask;
    u8 result = buffer->buffer[buffer->writeIndex];
    return result;
}

internal void
circular_unget_byte(CircularBuffer *buffer, u8 byte)
{
    i_expect(get_available_size(buffer));
    buffer->readIndex = (buffer->readIndex - 1) & buffer->mask;
    buffer->buffer[buffer->readIndex] = byte;
}
