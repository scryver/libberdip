internal u16 safe_truncate_to_u16(u32 value) { i_expect(value <= U16_MAX); return (u16)(value & U16_MAX); }
internal u8  safe_truncate_to_u8(u32 value)  { i_expect(value <= U8_MAX);  return (u8)(value & U8_MAX); }
internal u8  safe_truncate_to_u8(u16 value)  { i_expect(value <= U8_MAX);  return (u8)(value & U8_MAX); }
internal s16 safe_truncate_to_s16(s32 value) { i_expect(value <= (s32)S16_MAX); i_expect(value >= (s32)S16_MIN); return (s16)value; }
internal s8  safe_truncate_to_s8(s32 value)  { i_expect(value <= (s32)(s16)S8_MAX); i_expect(value >= (s32)(s16)S8_MIN); return (s8)value; }
internal s8  safe_truncate_to_s8(s16 value)  { i_expect(value <= (s16)S8_MAX); i_expect(value >= (s16)S8_MIN); return (s8)value; }

internal b32 is_infinite(f32 f)
{
    u32 uf = *(u32 *)&f;
    return (uf & (F32_EXP_MASK | F32_FRAC_MASK)) == F32_EXP_MASK;
}
internal b32 is_pos_infinite(f32 f)
{
    u32 uf = *(u32 *)&f;
    return uf == F32_EXP_MASK;
}
internal b32 is_neg_infinite(f32 f)
{
    u32 uf = *(u32 *)&f;
    return uf == (F32_SIGN_MASK | F32_EXP_MASK);
}
internal b32 is_nan(f32 f)
{
    u32 uf = *(u32 *)&f;
    return ((uf & F32_EXP_MASK) == F32_EXP_MASK) && (uf & F32_FRAC_MASK);
}
internal b32 is_pos_nan(f32 f)
{
    u32 uf = *(u32 *)&f;
    return ((uf & F32_EXP_MASK) == F32_EXP_MASK) && (uf & F32_FRAC_MASK) && ((uf & F32_SIGN_MASK) == 0);
}
internal b32 is_neg_nan(f32 f)
{
    u32 uf = *(u32 *)&f;
    return ((uf & F32_EXP_MASK) == F32_EXP_MASK) && (uf & F32_FRAC_MASK) && (uf & F32_SIGN_MASK);
}

internal b32 is_infinite(f64 f)
{
    u64 uf = *(u64 *)&f;
    return (uf & (F64_EXP_MASK | F64_FRAC_MASK)) == F64_EXP_MASK;
}
internal b32 is_pos_infinite(f64 f)
{
    u64 uf = *(u64 *)&f;
    return uf == F64_EXP_MASK;
}
internal b32 is_neg_infinite(f64 f)
{
    u64 uf = *(u64 *)&f;
    return uf == (F64_SIGN_MASK | F64_EXP_MASK);
}
internal b32 is_nan(f64 f)
{
    u64 uf = *(u64 *)&f;
    return ((uf & F64_EXP_MASK) == F64_EXP_MASK) && (uf & F64_FRAC_MASK);
}
internal b32 is_pos_nan(f64 f)
{
    u64 uf = *(u64 *)&f;
    return ((uf & F64_EXP_MASK) == F64_EXP_MASK) && (uf & F64_FRAC_MASK) && ((uf & F64_SIGN_MASK) == 0);
}
internal b32 is_neg_nan(f64 f)
{
    u64 uf = *(u64 *)&f;
    return ((uf & F64_EXP_MASK) == F64_EXP_MASK) && (uf & F64_FRAC_MASK) && (uf & F64_SIGN_MASK);
}

internal f32 make_real(f32 f, f32 maxVal = F32_MAX)
{
    f32 result = f;
    if (is_neg_nan(f) || (f < -maxVal))
    {
        result = -maxVal;
    }
    else if (is_pos_nan(f) || (f > maxVal))
    {
        result = maxVal;
    }
    return result;
}
