internal inline u16 safe_truncate_to_u16(u32 value) { i_expect(value <= U16_MAX); return (u16)(value & U16_MAX); }
internal inline u8  safe_truncate_to_u8(u32 value)  { i_expect(value <= U8_MAX);  return (u8)(value & U8_MAX); }
internal inline u8  safe_truncate_to_u8(u16 value)  { i_expect(value <= U8_MAX);  return (u8)(value & U8_MAX); }
internal inline s16 safe_truncate_to_s16(s32 value) { i_expect(value <= (s32)S16_MAX); i_expect(value >= (s32)S16_MIN); return (s16)value; }
internal inline s8  safe_truncate_to_s8(s32 value)  { i_expect(value <= (s32)(s16)S8_MAX); i_expect(value >= (s32)(s16)S8_MIN); return (s8)value; }
internal inline s8  safe_truncate_to_s8(s16 value)  { i_expect(value <= (s16)S8_MAX); i_expect(value >= (s16)S8_MIN); return (s8)value; }

internal inline b32 is_infinite(f32 f)
{
    u32 uf = *(u32 *)&f;
    return (uf == F32_EXP_MASK) || (uf == (F32_EXP_MASK | F32_SIGN_MASK));
}
internal inline b32 is_NaN(f32 f)
{
    u32 uf = *(u32 *)&f;
    return ((uf & F32_EXP_MASK) == F32_EXP_MASK) && ((uf & F32_FRAC_MASK) != 0);
}

internal inline b32 is_infinite(f64 f)
{
    u64 uf = *(u64 *)&f;
    return (uf == F64_EXP_MASK) || (uf == (F64_EXP_MASK | F64_SIGN_MASK));
}
internal inline b32 is_NaN(f64 f)
{
    u64 uf = *(u64 *)&f;
    return ((uf & F64_EXP_MASK) == F64_EXP_MASK) && ((uf & F64_FRAC_MASK) != 0);
}

internal inline void *allocate_size(u32 size) { return allocate_size(size, 0); }