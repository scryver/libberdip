#ifndef MORE_FFT_SINES
#define MORE_FFT_SINES 1
#endif

//#define fft_cos(x)  cos_f32(x)
//#define fft_sin(x)  sin_f32(x)
//#define fft_cos(x)  __builtin_cosf(F32_TAU * x)
//#define fft_sin(x)  __builtin_sinf(F32_TAU * x)
#define fft_cos(x)  cos_pi(F32_TAU * x)
#define fft_sin(x)  sin_pi(F32_TAU * x)

internal void
fft(u32 dftCount, Complex32 *signal)
{
    i_expect(is_pow2(dftCount));
    i_expect(dftCount > 4);
    i_expect(!((umm)signal & 0xF)); // NOTE(michiel): Must be 16 bytes aligned

    u32 halfCount = dftCount / 2;
    BitScanResult highBit = find_most_significant_set_bit(dftCount);
    for (u32 index = 0; index < halfCount; index += 2)
    {
        u32 index0 = index + 0;
        u32 index1 = index + 1;

        u32 reversedIndex0 = reverse_bits(index0, highBit.index);
        u32 reversedIndex1 = reversedIndex0 ^ halfCount;

        if (reversedIndex0 > index0)
        {
            Complex32 temp = signal[index0];
            signal[index0] = signal[reversedIndex0];
            signal[reversedIndex0] = temp;
        }
        if (reversedIndex1 > index1)
        {
            Complex32 temp = signal[index1];
            signal[index1] = signal[reversedIndex1];
            signal[reversedIndex1] = temp;
        }

        u32 index4 = index0 + halfCount;
        u32 index5 = index1 + halfCount;
        u32 reversedIndex4 = reversedIndex0 + 1;
        u32 reversedIndex5 = reversedIndex1 + 1;

        if (reversedIndex4 > index4)
        {
            Complex32 temp = signal[index4];
            signal[index4] = signal[reversedIndex4];
            signal[reversedIndex4] = temp;
        }
        if (reversedIndex5 > index5)
        {
            Complex32 temp = signal[index5];
            signal[index5] = signal[reversedIndex5];
            signal[reversedIndex5] = temp;
        }
    }

    {
        SinCos_4x sinCos = sincos_f32_4x(F32_4x(0.0f, -0.125f, -0.25f, -0.375f));
        f32_4x W4_reals;
        f32_4x W4_imags;
        W4_reals.mi = _mm_shuffle_epi32(sinCos.cos.mi, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
        W4_imags.mi = _mm_shuffle_epi32(sinCos.sin.mi, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));

        f32_4x W8_reals = sinCos.cos;
        f32_4x W8_imags = sinCos.sin;
        for (u32 k = 0; k < dftCount; k += 8)
        {
            f32 *kGrab = (f32 *)(signal + k);
            f32 *kPut  = (f32 *)(signal + k);

            f32_4x EO2_0 = F32_4x(kGrab);
            kGrab += 4;
            f32_4x EO2_1 = F32_4x(kGrab);
            kGrab += 4;
            f32_4x EO2_2 = F32_4x(kGrab);
            kGrab += 4;
            f32_4x EO2_3 = F32_4x(kGrab);

            f32_4x E2_02;
            f32_4x O2_02;
            f32_4x E2_13;
            f32_4x O2_13;
            E2_02.m = _mm_shuffle_ps(EO2_0.m, EO2_2.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            O2_02.m = _mm_shuffle_ps(EO2_0.m, EO2_2.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
            E2_13.m = _mm_shuffle_ps(EO2_1.m, EO2_3.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            O2_13.m = _mm_shuffle_ps(EO2_1.m, EO2_3.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));

            f32_4x E4_02 = E2_02 + O2_02;
            f32_4x ac_4x = E2_13 + O2_13;
            f32_4x E4_13 = E2_02 - O2_02;
            f32_4x bd_4x = E2_13 - O2_13;

            f32_4x X_reals;
            X_reals.m = _mm_shuffle_ps(ac_4x.m, bd_4x.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
            X_reals.mi = _mm_shuffle_epi32(X_reals.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
            f32_4x X_imags;
            X_imags.m = _mm_shuffle_ps(ac_4x.m, bd_4x.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));
            X_imags.mi = _mm_shuffle_epi32(X_imags.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));

            f32_4x O4_reals = (W4_reals * X_reals) - (W4_imags * X_imags);
            f32_4x O4_imags = (W4_reals * X_imags) + (W4_imags * X_reals);

            f32_4x E4_0_4x;
            f32_4x E4_1_4x;
            f32_4x O4_0_4x;
            f32_4x O4_1_4x;
            E4_0_4x.m = _mm_shuffle_ps(E4_02.m, E4_13.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            E4_1_4x.m = _mm_shuffle_ps(E4_02.m, E4_13.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
            O4_0_4x.m = _mm_shuffle_ps(O4_reals.m, O4_imags.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            O4_0_4x.mi = _mm_shuffle_epi32(O4_0_4x.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
            O4_1_4x.m = _mm_shuffle_ps(O4_reals.m, O4_imags.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
            O4_1_4x.mi = _mm_shuffle_epi32(O4_1_4x.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));

            f32_4x ef_4x = E4_1_4x + O4_1_4x;
            f32_4x gh_4x = E4_1_4x - O4_1_4x;

            f32_4x Y_reals;
            f32_4x Y_imags;
            Y_reals.m = _mm_shuffle_ps(ef_4x.m, gh_4x.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
            Y_imags.m = _mm_shuffle_ps(ef_4x.m, gh_4x.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));

            f32_4x O8_reals = (W8_reals * Y_reals) - (W8_imags * Y_imags);
            f32_4x O8_imags = (W8_reals * Y_imags) + (W8_imags * Y_reals);

            f32_4x E8_0_4x = E4_0_4x + O4_0_4x;
            f32_4x E8_1_4x = E4_0_4x - O4_0_4x;
            f32_4x O8_0_4x;
            f32_4x O8_1_4x;
            O8_0_4x.m = _mm_shuffle_ps(O8_reals.m, O8_imags.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            O8_0_4x.mi = _mm_shuffle_epi32(O8_0_4x.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
            O8_1_4x.m = _mm_shuffle_ps(O8_reals.m, O8_imags.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
            O8_1_4x.mi = _mm_shuffle_epi32(O8_1_4x.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));

            _mm_store_ps(kPut, (E8_0_4x + O8_0_4x).m);
            kPut += 4;
            _mm_store_ps(kPut, (E8_1_4x + O8_1_4x).m);
            kPut += 4;
            _mm_store_ps(kPut, (E8_0_4x - O8_0_4x).m);
            kPut += 4;
            _mm_store_ps(kPut, (E8_1_4x - O8_1_4x).m);
        }
    }

    u32 halfM = 8;
    u32 m = 16;

    {
        Complex32 Wm;
        Complex32 Wm2;
        Complex32 Wm3;
        Complex32 Wm4;

        // NOTE(michiel): Prepare values to be copied
        f32 oneOverMPre = 1.0f / (f32)m;
#if 0
        Wm.real = fft_cos(-oneOverMPre * 2.0f);
        Wm.imag = fft_sin(-oneOverMPre * 2.0f);
        Wm2.real = fft_cos(-oneOverMPre * 4.0f);
        Wm2.imag = fft_sin(-oneOverMPre * 4.0f);
        Wm3.real = fft_cos(-oneOverMPre * 6.0f);
        Wm3.imag = fft_sin(-oneOverMPre * 6.0f);
        Wm4.real = fft_cos(-oneOverMPre * 8.0f);
        Wm4.imag = fft_sin(-oneOverMPre * 8.0f);
#else
        SinCos_4x sinCos = sincos_f32_4x(F32_4x(-oneOverMPre*2.0f, -oneOverMPre*4.0f, -oneOverMPre*6.0f, -oneOverMPre*8.0f));
        Wm.real = sinCos.cos.e[0];
        Wm.imag = sinCos.sin.e[0];
        Wm2.real = sinCos.cos.e[1];
        Wm2.imag = sinCos.sin.e[1];
        Wm3.real = sinCos.cos.e[2];
        Wm3.imag = sinCos.sin.e[2];
        Wm4.real = sinCos.cos.e[3];
        Wm4.imag = sinCos.sin.e[3];
#endif

        while (m <= dftCount)
        {
            f32 oneOverM = 1.0f / (f32)m;

#if MORE_FFT_SINES
            Complex32 Wm5;
            Complex32 Wm6;
            Complex32 Wm7;
            Complex32 Wm8;

            Wm8 = Wm4;
            Wm6 = Wm3;
#endif
            Wm4 = Wm2;
            Wm2 = Wm;

#if 0
            Wm.real = fft_cos(-oneOverM);
            Wm.imag = fft_sin(-oneOverM);
            Wm3.real = fft_cos(-oneOverM * 3.0f);
            Wm3.imag = fft_sin(-oneOverM * 3.0f);

#if MORE_FFT_SINES
            Wm5.real = fft_cos(-oneOverM * 5.0f);
            Wm5.imag = fft_sin(-oneOverM * 5.0f);
            Wm7.real = fft_cos(-oneOverM * 7.0f);
            Wm7.imag = fft_sin(-oneOverM * 7.0f);
#endif
#else
            sinCos = sincos_f32_4x(F32_4x(-oneOverM, -oneOverM*3.0f, -oneOverM*5.0f, -oneOverM*7.0f));
            Wm.real = sinCos.cos.e[0];
            Wm.imag = sinCos.sin.e[0];
            Wm3.real = sinCos.cos.e[1];
            Wm3.imag = sinCos.sin.e[1];
#if MORE_FFT_SINES
            Wm5.real = sinCos.cos.e[2];
            Wm5.imag = sinCos.sin.e[2];
            Wm7.real = sinCos.cos.e[3];
            Wm7.imag = sinCos.sin.e[3];
#endif
#endif

#if MORE_FFT_SINES
            f32_4x wStep_real_4x = F32_4x(Wm8.real);
            f32_4x wStep_imag_4x = F32_4x(Wm8.imag);
#else
            f32_4x wStep_real_4x = F32_4x(Wm4.real);
            f32_4x wStep_imag_4x = F32_4x(Wm4.imag);
#endif

            for (u32 k = 0; k < dftCount; k += m)
            {
                Complex32 *src0 = signal + k;
                Complex32 *src1 = signal + k + halfM;

                f32_4x w0_real_4x = F32_4x(1.0f, Wm.real, Wm2.real, Wm3.real);
                f32_4x w0_imag_4x = F32_4x(0.0f, Wm.imag, Wm2.imag, Wm3.imag);
#if MORE_FFT_SINES
                f32_4x w1_real_4x = F32_4x(Wm4.real, Wm5.real, Wm6.real, Wm7.real);
                f32_4x w1_imag_4x = F32_4x(Wm4.imag, Wm5.imag, Wm6.imag, Wm7.imag);
#endif

                for (u32 j = 0; j < halfM; j += 8)
                {
                    f32 *EGrab = (f32 *)(src0 + j);
                    f32 *OGrab = (f32 *)(src1 + j);
                    f32 *EPut  = (f32 *)(src0 + j);
                    f32 *OPut  = (f32 *)(src1 + j);

                    f32_4x a01 = F32_4x(OGrab);
                    OGrab += 4;
                    f32_4x a23 = F32_4x(OGrab);
                    OGrab += 4;
                    f32_4x a45 = F32_4x(OGrab);
                    OGrab += 4;
                    f32_4x a67 = F32_4x(OGrab);

                    f32_4x a_real;
                    f32_4x a_imag;
                    a_real.m = _mm_shuffle_ps(a01.m, a23.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
                    a_imag.m = _mm_shuffle_ps(a01.m, a23.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));

                    f32_4x O0_real = (w0_real_4x * a_real) - (w0_imag_4x * a_imag);
                    f32_4x O0_imag = (w0_real_4x * a_imag) + (w0_imag_4x * a_real);

                    f32_4x temp_w = (w0_real_4x * wStep_real_4x) - (w0_imag_4x * wStep_imag_4x);
                    w0_imag_4x    = (w0_real_4x * wStep_imag_4x) + (w0_imag_4x * wStep_real_4x);
                    w0_real_4x    = temp_w;

                    f32_4x b_real;
                    f32_4x b_imag;
                    b_real.m = _mm_shuffle_ps(a45.m, a67.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
                    b_imag.m = _mm_shuffle_ps(a45.m, a67.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));

#if MORE_FFT_SINES
                    f32_4x O1_real = (w1_real_4x * b_real) - (w1_imag_4x * b_imag);
                    f32_4x O1_imag = (w1_real_4x * b_imag) + (w1_imag_4x * b_real);

                    f32_4x temp_w2 = (w1_real_4x * wStep_real_4x) - (w1_imag_4x * wStep_imag_4x);
                    w1_imag_4x     = (w1_real_4x * wStep_imag_4x) + (w1_imag_4x * wStep_real_4x);
                    w1_real_4x     = temp_w2;
#else
                    f32_4x O1_real = (w0_real_4x * b_real) - (w0_imag_4x * b_imag);
                    f32_4x O1_imag = (w0_real_4x * b_imag) + (w0_imag_4x * b_real);

                    f32_4x temp_w2 = (w0_real_4x * wStep_real_4x) - (w0_imag_4x * wStep_imag_4x);
                    w0_imag_4x     = (w0_real_4x * wStep_imag_4x) + (w0_imag_4x * wStep_real_4x);
                    w0_real_4x     = temp_w2;
#endif

                    f32_4x E01 = F32_4x(EGrab);
                    EGrab += 4;
                    f32_4x E23 = F32_4x(EGrab);
                    EGrab += 4;
                    f32_4x E45 = F32_4x(EGrab);
                    EGrab += 4;
                    f32_4x E67 = F32_4x(EGrab);

                    f32_4x O01;
                    f32_4x O23;
                    f32_4x O45;
                    f32_4x O67;
                    O01.m = _mm_shuffle_ps(O0_real.m, O0_imag.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
                    O01.mi = _mm_shuffle_epi32(O01.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
                    O23.m = _mm_shuffle_ps(O0_real.m, O0_imag.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
                    O23.mi = _mm_shuffle_epi32(O23.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
                    O45.m = _mm_shuffle_ps(O1_real.m, O1_imag.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
                    O45.mi = _mm_shuffle_epi32(O45.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
                    O67.m = _mm_shuffle_ps(O1_real.m, O1_imag.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
                    O67.mi = _mm_shuffle_epi32(O67.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));

                    f32_4x add01 = E01 + O01;
                    f32_4x add23 = E23 + O23;
                    f32_4x sub01 = E01 - O01;
                    f32_4x sub23 = E23 - O23;

                    f32_4x add45 = E45 + O45;
                    f32_4x add67 = E67 + O67;
                    f32_4x sub45 = E45 - O45;
                    f32_4x sub67 = E67 - O67;

                    _mm_store_ps(EPut, add01.m);
                    EPut += 4;
                    _mm_store_ps(OPut, sub01.m);
                    OPut += 4;
                    _mm_store_ps(EPut, add23.m);
                    EPut += 4;
                    _mm_store_ps(OPut, sub23.m);
                    OPut += 4;
                    _mm_store_ps(EPut, add45.m);
                    EPut += 4;
                    _mm_store_ps(OPut, sub45.m);
                    OPut += 4;
                    _mm_store_ps(EPut, add67.m);
                    _mm_store_ps(OPut, sub67.m);
                }
            }

            halfM = m;
            m <<= 1;
        }
    }
}

internal void
ifft(u32 dftCount, Complex32 *signal)
{
    i_expect(is_pow2(dftCount));
    i_expect(dftCount > 2);
    i_expect(!((umm)signal & 0xF)); // NOTE(michiel): Must be 16 bytes aligned

    u32 halfCount = dftCount / 2;
    BitScanResult highBit = find_most_significant_set_bit(dftCount);
    for (u32 index = 0; index < halfCount; index += 2)
    {
        u32 index0 = index + 0;
        u32 index1 = index + 1;

        u32 reversedIndex0 = reverse_bits(index0, highBit.index);
        u32 reversedIndex1 = reversedIndex0 ^ halfCount;

        if (reversedIndex0 > index0)
        {
            Complex32 temp = signal[index0];
            signal[index0] = signal[reversedIndex0];
            signal[reversedIndex0] = temp;
        }
        if (reversedIndex1 > index1)
        {
            Complex32 temp = signal[index1];
            signal[index1] = signal[reversedIndex1];
            signal[reversedIndex1] = temp;
        }

        u32 index4 = index0 + halfCount;
        u32 index5 = index1 + halfCount;
        u32 reversedIndex4 = reversedIndex0 + 1;
        u32 reversedIndex5 = reversedIndex1 + 1;

        if (reversedIndex4 > index4)
        {
            Complex32 temp = signal[index4];
            signal[index4] = signal[reversedIndex4];
            signal[reversedIndex4] = temp;
        }
        if (reversedIndex5 > index5)
        {
            Complex32 temp = signal[index5];
            signal[index5] = signal[reversedIndex5];
            signal[reversedIndex5] = temp;
        }
    }

    {
        Complex32 Wm4;
        Wm4.real = fft_cos(0.25f);
        Wm4.imag = fft_sin(0.25f);
        f32_4x W4_reals = F32_4x(1, Wm4.real, 1, Wm4.real);
        f32_4x W4_imags = F32_4x(0, Wm4.imag, 0, Wm4.imag);

        Complex32 Wm8_1;
        Wm8_1.real = fft_cos(0.125f);
        Wm8_1.imag = fft_sin(0.125f);
        Complex32 Wm8_3;
        Wm8_3.real = fft_cos(0.375f);
        Wm8_3.imag = fft_sin(0.375f);
        f32_4x W8_reals = F32_4x(1, Wm8_1.real, Wm4.real, Wm8_3.real);
        f32_4x W8_imags = F32_4x(0, Wm8_1.imag, Wm4.imag, Wm8_3.imag);

        f32_4x ampl2_4x = F32_4x(0.5f * 0.5f * (f32)dftCount);
        f32_4x ampl4_4x = F32_4x(0.5f * 0.5f * 0.25f * (f32)dftCount);
        f32_4x ampl8_4x = F32_4x(0.5f * 0.25f * 0.125f * (f32)dftCount);

        for (u32 k = 0; k < dftCount; k += 8)
        {
            f32 *kGrab = (f32 *)(signal + k);
            f32 *kPut  = (f32 *)(signal + k);

            f32_4x EO2_0 = F32_4x(kGrab);
            kGrab += 4;
            f32_4x EO2_1 = F32_4x(kGrab);
            kGrab += 4;
            f32_4x EO2_2 = F32_4x(kGrab);
            kGrab += 4;
            f32_4x EO2_3 = F32_4x(kGrab);

            f32_4x E2_02;
            f32_4x O2_02;
            f32_4x E2_13;
            f32_4x O2_13;
            E2_02.m = _mm_shuffle_ps(EO2_0.m, EO2_2.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            O2_02.m = _mm_shuffle_ps(EO2_0.m, EO2_2.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
            E2_13.m = _mm_shuffle_ps(EO2_1.m, EO2_3.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            O2_13.m = _mm_shuffle_ps(EO2_1.m, EO2_3.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));

            f32_4x E4_02 = (E2_02 + O2_02) * ampl2_4x;
            f32_4x ac_4x = (E2_13 + O2_13) * ampl2_4x;
            f32_4x E4_13 = (E2_02 - O2_02) * ampl2_4x;
            f32_4x bd_4x = (E2_13 - O2_13) * ampl2_4x;

            f32_4x X_reals;
            X_reals.m = _mm_shuffle_ps(ac_4x.m, bd_4x.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
            X_reals.mi = _mm_shuffle_epi32(X_reals.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
            f32_4x X_imags;
            X_imags.m = _mm_shuffle_ps(ac_4x.m, bd_4x.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));
            X_imags.mi = _mm_shuffle_epi32(X_imags.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));

            f32_4x mulX0 = W4_reals * X_reals;
            f32_4x mulX1 = W4_imags * X_imags;
            f32_4x mulX2 = W4_reals * X_imags;
            f32_4x mulX3 = W4_imags * X_reals;

            f32_4x O4_reals = mulX0 - mulX1;
            f32_4x O4_imags = mulX2 + mulX3;

            f32_4x E4_0_4x;
            f32_4x E4_1_4x;
            f32_4x O4_0_4x;
            f32_4x O4_1_4x;
            E4_0_4x.m = _mm_shuffle_ps(E4_02.m, E4_13.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            E4_1_4x.m = _mm_shuffle_ps(E4_02.m, E4_13.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
            O4_0_4x.m = _mm_shuffle_ps(O4_reals.m, O4_imags.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            O4_0_4x.mi = _mm_shuffle_epi32(O4_0_4x.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
            O4_1_4x.m = _mm_shuffle_ps(O4_reals.m, O4_imags.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
            O4_1_4x.mi = _mm_shuffle_epi32(O4_1_4x.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));

            f32_4x ef_4x = (E4_1_4x + O4_1_4x) * ampl4_4x;
            f32_4x gh_4x = (E4_1_4x - O4_1_4x) * ampl4_4x;

            f32_4x Y_reals;
            f32_4x Y_imags;
            Y_reals.m = _mm_shuffle_ps(ef_4x.m, gh_4x.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
            Y_imags.m = _mm_shuffle_ps(ef_4x.m, gh_4x.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));

            f32_4x mulY0 = W8_reals * Y_reals;
            f32_4x mulY1 = W8_imags * Y_imags;
            f32_4x mulY2 = W8_reals * Y_imags;
            f32_4x mulY3 = W8_imags * Y_reals;

            f32_4x O8_reals = mulY0 - mulY1;
            f32_4x O8_imags = mulY2 + mulY3;

            f32_4x E8_0_4x = (E4_0_4x + O4_0_4x) * ampl4_4x;
            f32_4x E8_1_4x = (E4_0_4x - O4_0_4x) * ampl4_4x;
            f32_4x O8_0_4x;
            f32_4x O8_1_4x;
            O8_0_4x.m = _mm_shuffle_ps(O8_reals.m, O8_imags.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            O8_0_4x.mi = _mm_shuffle_epi32(O8_0_4x.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
            O8_1_4x.m = _mm_shuffle_ps(O8_reals.m, O8_imags.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
            O8_1_4x.mi = _mm_shuffle_epi32(O8_1_4x.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));

            _mm_store_ps(kPut, ((E8_0_4x + O8_0_4x) * ampl8_4x).m);
            kPut += 4;
            _mm_store_ps(kPut, ((E8_1_4x + O8_1_4x) * ampl8_4x).m);
            kPut += 4;
            _mm_store_ps(kPut, ((E8_0_4x - O8_0_4x) * ampl8_4x).m);
            kPut += 4;
            _mm_store_ps(kPut, ((E8_1_4x - O8_1_4x) * ampl8_4x).m);
        }
    }

    u32 halfM = 8;
    u32 m = 16;
    u32 d = dftCount / m;

    {
        Complex32 Wm;
        Complex32 Wm2;
        Complex32 Wm3;
        Complex32 Wm4;

        // NOTE(michiel): Prepare values to be copied
        f32 oneOverMPre = 1.0f / (f32)m;
        Wm.real = fft_cos(oneOverMPre * 2.0f);
        Wm.imag = fft_sin(oneOverMPre * 2.0f);
        Wm2.real = fft_cos(oneOverMPre * 4.0f);
        Wm2.imag = fft_sin(oneOverMPre * 4.0f);
        Wm3.real = fft_cos(oneOverMPre * 6.0f);
        Wm3.imag = fft_sin(oneOverMPre * 6.0f);
        Wm4.real = fft_cos(oneOverMPre * 8.0f);
        Wm4.imag = fft_sin(oneOverMPre * 8.0f);

        while (m <= dftCount)
        {
            f32 oneOverM = 1.0f / (f32)m;

#if MORE_FFT_SINES
            Complex32 Wm5;
            Complex32 Wm6;
            Complex32 Wm7;
            Complex32 Wm8;

            Wm8 = Wm4;
            Wm6 = Wm3;
#endif
            Wm4 = Wm2;
            Wm2 = Wm;

            Wm.real = fft_cos(oneOverM);
            Wm.imag = fft_sin(oneOverM);
            Wm3.real = fft_cos(oneOverM * 3.0f);
            Wm3.imag = fft_sin(oneOverM * 3.0f);

#if MORE_FFT_SINES
            Wm5.real = fft_cos(oneOverM * 5.0f);
            Wm5.imag = fft_sin(oneOverM * 5.0f);
            Wm7.real = fft_cos(oneOverM * 7.0f);
            Wm7.imag = fft_sin(oneOverM * 7.0f);
#endif

#if MORE_FFT_SINES
            f32_4x wm8_real_4x = F32_4x(Wm8.real);
            f32_4x wm8_imag_4x = F32_4x(Wm8.imag);
#else
            f32_4x wm4_real_4x = F32_4x(Wm4.real);
            f32_4x wm4_imag_4x = F32_4x(Wm4.imag);
#endif

            f32_4x amplMod_4x = F32_4x((f32)d * oneOverM);

            for (u32 k = 0; k < dftCount; k += m)
            {
                Complex32 *src0 = signal + k;
                Complex32 *src1 = signal + k + halfM;

#if MORE_FFT_SINES
                f32_4x w0_real_4x = F32_4x(1.0f, Wm.real, Wm2.real, Wm3.real);
                f32_4x w0_imag_4x = F32_4x(0.0f, Wm.imag, Wm2.imag, Wm3.imag);
                f32_4x w1_real_4x = F32_4x(Wm4.real, Wm5.real, Wm6.real, Wm7.real);
                f32_4x w1_imag_4x = F32_4x(Wm4.imag, Wm5.imag, Wm6.imag, Wm7.imag);
#else
                f32_4x w_real_4x = F32_4x(1.0f, Wm.real, Wm2.real, Wm3.real);
                f32_4x w_imag_4x = F32_4x(0.0f, Wm.imag, Wm2.imag, Wm3.imag);
#endif

                for (u32 j = 0; j < halfM; j += 8)
                {
                    f32 *EGrab = (f32 *)(src0 + j);
                    f32 *OGrab = (f32 *)(src1 + j);
                    f32 *EPut  = (f32 *)(src0 + j);
                    f32 *OPut  = (f32 *)(src1 + j);

                    f32_4x a01 = F32_4x(OGrab);
                    OGrab += 4;
                    f32_4x a23 = F32_4x(OGrab);
                    OGrab += 4;
                    f32_4x a45 = F32_4x(OGrab);
                    OGrab += 4;
                    f32_4x a67 = F32_4x(OGrab);

                    f32_4x a_real;
                    f32_4x a_imag;
                    a_real.m = _mm_shuffle_ps(a01.m, a23.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
                    a_imag.m = _mm_shuffle_ps(a01.m, a23.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));

#if MORE_FFT_SINES
                    f32_4x mulX0 = w0_real_4x * a_real;
                    f32_4x mulX1 = w0_imag_4x * a_imag;
                    f32_4x mulX2 = w0_real_4x * a_imag;
                    f32_4x mulX3 = w0_imag_4x * a_real;

                    f32_4x temp_w = (w0_real_4x * wm8_real_4x) - (w0_imag_4x * wm8_imag_4x);
                    w0_imag_4x = (w0_real_4x * wm8_imag_4x) + (w0_imag_4x * wm8_real_4x);
                    w0_real_4x = temp_w;
#else
                    f32_4x mulX0 = w_real_4x * a_real;
                    f32_4x mulX1 = w_imag_4x * a_imag;
                    f32_4x mulX2 = w_real_4x * a_imag;
                    f32_4x mulX3 = w_imag_4x * a_real;

                    f32_4x temp_w = (w_real_4x * wm4_real_4x) - (w_imag_4x * wm4_imag_4x);
                    w_imag_4x = (w_real_4x * wm4_imag_4x) + (w_imag_4x * wm4_real_4x);
                    w_real_4x = temp_w;
#endif
                    f32_4x O0_real = mulX0 - mulX1;
                    f32_4x O0_imag = mulX2 + mulX3;

                    f32_4x b_real;
                    f32_4x b_imag;
                    b_real.m = _mm_shuffle_ps(a45.m, a67.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
                    b_imag.m = _mm_shuffle_ps(a45.m, a67.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));

#if MORE_FFT_SINES
                    f32_4x mulY0 = w1_real_4x * b_real;
                    f32_4x mulY1 = w1_imag_4x * b_imag;
                    f32_4x mulY2 = w1_real_4x * b_imag;
                    f32_4x mulY3 = w1_imag_4x * b_real;

                    f32_4x temp_w2 = (w1_real_4x * wm8_real_4x) - (w1_imag_4x * wm8_imag_4x);
                    w1_imag_4x = (w1_real_4x * wm8_imag_4x) + (w1_imag_4x * wm8_real_4x);
                    w1_real_4x = temp_w2;
#else
                    f32_4x mulY0 = w_real_4x * b_real;
                    f32_4x mulY1 = w_imag_4x * b_imag;
                    f32_4x mulY2 = w_real_4x * b_imag;
                    f32_4x mulY3 = w_imag_4x * b_real;

                    f32_4x temp_w2 = (w_real_4x * wm4_real_4x) - (w_imag_4x * wm4_imag_4x);
                    w_imag_4x = (w_real_4x * wm4_imag_4x) + (w_imag_4x * wm4_real_4x);
                    w_real_4x = temp_w2;
#endif
                    f32_4x O1_real = mulY0 - mulY1;
                    f32_4x O1_imag = mulY2 + mulY3;

                    f32_4x E01 = F32_4x(EGrab);
                    EGrab += 4;
                    f32_4x E23 = F32_4x(EGrab);
                    EGrab += 4;
                    f32_4x E45 = F32_4x(EGrab);
                    EGrab += 4;
                    f32_4x E67 = F32_4x(EGrab);

                    f32_4x O01;
                    f32_4x O23;
                    f32_4x O45;
                    f32_4x O67;
                    O01.m = _mm_shuffle_ps(O0_real.m, O0_imag.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
                    O01.mi = _mm_shuffle_epi32(O01.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
                    O23.m = _mm_shuffle_ps(O0_real.m, O0_imag.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
                    O23.mi = _mm_shuffle_epi32(O23.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
                    O45.m = _mm_shuffle_ps(O1_real.m, O1_imag.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
                    O45.mi = _mm_shuffle_epi32(O45.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
                    O67.m = _mm_shuffle_ps(O1_real.m, O1_imag.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
                    O67.mi = _mm_shuffle_epi32(O67.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));

                    f32_4x add01 = E01 + O01;
                    f32_4x add23 = E23 + O23;
                    f32_4x sub01 = E01 - O01;
                    f32_4x sub23 = E23 - O23;

                    add01 = add01 * amplMod_4x;
                    add23 = add23 * amplMod_4x;
                    sub01 = sub01 * amplMod_4x;
                    sub23 = sub23 * amplMod_4x;

                    f32_4x add45 = E45 + O45;
                    f32_4x add67 = E67 + O67;
                    f32_4x sub45 = E45 - O45;
                    f32_4x sub67 = E67 - O67;

                    add45 = add45 * amplMod_4x;
                    add67 = add67 * amplMod_4x;
                    sub45 = sub45 * amplMod_4x;
                    sub67 = sub67 * amplMod_4x;

                    _mm_store_ps(EPut, add01.m);
                    EPut += 4;
                    _mm_store_ps(OPut, sub01.m);
                    OPut += 4;
                    _mm_store_ps(EPut, add23.m);
                    EPut += 4;
                    _mm_store_ps(OPut, sub23.m);
                    OPut += 4;
                    _mm_store_ps(EPut, add45.m);
                    EPut += 4;
                    _mm_store_ps(OPut, sub45.m);
                    OPut += 4;
                    _mm_store_ps(EPut, add67.m);
                    _mm_store_ps(OPut, sub67.m);
                }
            }

            halfM = m;
            m <<= 1;
            d >>= 1;
        }
    }
}

internal void
fft_exact(u32 dftCount, Complex32 *signal)
{
    // NOTE(michiel): About 3 times slower, but always calls sin/cos instead of adding angles.
    // This will introduce less artifacts.
    i_expect(is_pow2(dftCount));
    i_expect(dftCount > 4);
    i_expect(!((umm)signal & 0xF)); // NOTE(michiel): Must be 16 bytes aligned

    u32 halfCount = dftCount / 2;
    BitScanResult highBit = find_most_significant_set_bit(dftCount);
    for (u32 index = 0; index < halfCount; index += 2)
    {
        u32 index0 = index + 0;
        u32 index1 = index + 1;

        u32 reversedIndex0 = reverse_bits(index0, highBit.index);
        u32 reversedIndex1 = reversedIndex0 ^ halfCount;

        if (reversedIndex0 > index0)
        {
            Complex32 temp = signal[index0];
            signal[index0] = signal[reversedIndex0];
            signal[reversedIndex0] = temp;
        }
        if (reversedIndex1 > index1)
        {
            Complex32 temp = signal[index1];
            signal[index1] = signal[reversedIndex1];
            signal[reversedIndex1] = temp;
        }

        u32 index4 = index0 + halfCount;
        u32 index5 = index1 + halfCount;
        u32 reversedIndex4 = reversedIndex0 + 1;
        u32 reversedIndex5 = reversedIndex1 + 1;

        if (reversedIndex4 > index4)
        {
            Complex32 temp = signal[index4];
            signal[index4] = signal[reversedIndex4];
            signal[reversedIndex4] = temp;
        }
        if (reversedIndex5 > index5)
        {
            Complex32 temp = signal[index5];
            signal[index5] = signal[reversedIndex5];
            signal[reversedIndex5] = temp;
        }
    }

    {
        SinCos_4x sinCos = sincos_f32_4x(F32_4x(0.0f, -0.125f, -0.25f, -0.375f));

        f32_4x W4_reals;
        f32_4x W4_imags;
        W4_reals.mi = _mm_shuffle_epi32(sinCos.cos.mi, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
        W4_imags.mi = _mm_shuffle_epi32(sinCos.sin.mi, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));

        f32_4x W8_reals = sinCos.cos;
        f32_4x W8_imags = sinCos.sin;

        for (u32 k = 0; k < dftCount; k += 8)
        {
            f32 *kGrab = (f32 *)(signal + k);
            f32 *kPut  = (f32 *)(signal + k);

            f32_4x EO2_0 = F32_4x(kGrab);
            kGrab += 4;
            f32_4x EO2_1 = F32_4x(kGrab);
            kGrab += 4;
            f32_4x EO2_2 = F32_4x(kGrab);
            kGrab += 4;
            f32_4x EO2_3 = F32_4x(kGrab);

            f32_4x E2_02;
            f32_4x O2_02;
            f32_4x E2_13;
            f32_4x O2_13;
            E2_02.m = _mm_shuffle_ps(EO2_0.m, EO2_2.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            O2_02.m = _mm_shuffle_ps(EO2_0.m, EO2_2.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
            E2_13.m = _mm_shuffle_ps(EO2_1.m, EO2_3.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            O2_13.m = _mm_shuffle_ps(EO2_1.m, EO2_3.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));

            f32_4x E4_02 = E2_02 + O2_02;
            f32_4x ac_4x = E2_13 + O2_13;
            f32_4x E4_13 = E2_02 - O2_02;
            f32_4x bd_4x = E2_13 - O2_13;

            f32_4x X_reals;
            X_reals.m = _mm_shuffle_ps(ac_4x.m, bd_4x.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
            X_reals.mi = _mm_shuffle_epi32(X_reals.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
            f32_4x X_imags;
            X_imags.m = _mm_shuffle_ps(ac_4x.m, bd_4x.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));
            X_imags.mi = _mm_shuffle_epi32(X_imags.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));

            f32_4x O4_reals = (W4_reals * X_reals) - (W4_imags * X_imags);
            f32_4x O4_imags = (W4_reals * X_imags) + (W4_imags * X_reals);

            f32_4x E4_0_4x;
            f32_4x E4_1_4x;
            f32_4x O4_0_4x;
            f32_4x O4_1_4x;
            E4_0_4x.m = _mm_shuffle_ps(E4_02.m, E4_13.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            E4_1_4x.m = _mm_shuffle_ps(E4_02.m, E4_13.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
            O4_0_4x.m = _mm_shuffle_ps(O4_reals.m, O4_imags.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            O4_0_4x.mi = _mm_shuffle_epi32(O4_0_4x.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
            O4_1_4x.m = _mm_shuffle_ps(O4_reals.m, O4_imags.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
            O4_1_4x.mi = _mm_shuffle_epi32(O4_1_4x.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));

            f32_4x ef_4x = E4_1_4x + O4_1_4x;
            f32_4x gh_4x = E4_1_4x - O4_1_4x;

            f32_4x Y_reals;
            f32_4x Y_imags;
            Y_reals.m = _mm_shuffle_ps(ef_4x.m, gh_4x.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
            Y_imags.m = _mm_shuffle_ps(ef_4x.m, gh_4x.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));

            f32_4x O8_reals = (W8_reals * Y_reals) - (W8_imags * Y_imags);
            f32_4x O8_imags = (W8_reals * Y_imags) + (W8_imags * Y_reals);

            f32_4x E8_0_4x = E4_0_4x + O4_0_4x;
            f32_4x E8_1_4x = E4_0_4x - O4_0_4x;
            f32_4x O8_0_4x;
            f32_4x O8_1_4x;
            O8_0_4x.m = _mm_shuffle_ps(O8_reals.m, O8_imags.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            O8_0_4x.mi = _mm_shuffle_epi32(O8_0_4x.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
            O8_1_4x.m = _mm_shuffle_ps(O8_reals.m, O8_imags.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
            O8_1_4x.mi = _mm_shuffle_epi32(O8_1_4x.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));

            _mm_store_ps(kPut, (E8_0_4x + O8_0_4x).m);
            kPut += 4;
            _mm_store_ps(kPut, (E8_1_4x + O8_1_4x).m);
            kPut += 4;
            _mm_store_ps(kPut, (E8_0_4x - O8_0_4x).m);
            kPut += 4;
            _mm_store_ps(kPut, (E8_1_4x - O8_1_4x).m);
        }
    }

    u32 halfM = 8;
    u32 m = 16;

    {
        f32_4x angleMod0 = F32_4x(0.0f, 1.0f, 2.0f, 3.0f);

        while (m <= dftCount)
        {
            f32 oneOverM = 1.0f / (f32)m;
            f32_4x oneOverM_4x = F32_4x(oneOverM);
            f32_4x angleBase = -oneOverM_4x * angleMod0;
            f32_4x angleStep = F32_4x(-4.0f) * oneOverM_4x;

            for (u32 k = 0; k < dftCount; k += m)
            {
                Complex32 *src0 = signal + k;
                Complex32 *src1 = signal + k + halfM;

                f32_4x angles = angleBase;

                for (u32 j = 0; j < halfM; j += 8)
                {
                    f32 *EGrab = (f32 *)(src0 + j);
                    f32 *OGrab = (f32 *)(src1 + j);
                    f32 *EPut  = (f32 *)(src0 + j);
                    f32 *OPut  = (f32 *)(src1 + j);

                    f32_4x a01 = F32_4x(OGrab);
                    OGrab += 4;
                    f32_4x a23 = F32_4x(OGrab);
                    OGrab += 4;
                    f32_4x a45 = F32_4x(OGrab);
                    OGrab += 4;
                    f32_4x a67 = F32_4x(OGrab);

                    f32_4x a_real;
                    f32_4x a_imag;
                    a_real.m = _mm_shuffle_ps(a01.m, a23.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
                    a_imag.m = _mm_shuffle_ps(a01.m, a23.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));

                    SinCos_4x w0 = sincos_f32_4x(angles);
                    angles = angles + angleStep;

                    f32_4x O0_real = (w0.cos * a_real) - (w0.sin * a_imag);
                    f32_4x O0_imag = (w0.cos * a_imag) + (w0.sin * a_real);

                    f32_4x b_real;
                    f32_4x b_imag;
                    b_real.m = _mm_shuffle_ps(a45.m, a67.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
                    b_imag.m = _mm_shuffle_ps(a45.m, a67.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));

                    SinCos_4x w1 = sincos_f32_4x(angles);
                    angles = angles + angleStep;

                    f32_4x O1_real = (w1.cos * b_real) - (w1.sin * b_imag);
                    f32_4x O1_imag = (w1.cos * b_imag) + (w1.sin * b_real);

                    f32_4x E01 = F32_4x(EGrab);
                    EGrab += 4;
                    f32_4x E23 = F32_4x(EGrab);
                    EGrab += 4;
                    f32_4x E45 = F32_4x(EGrab);
                    EGrab += 4;
                    f32_4x E67 = F32_4x(EGrab);

                    f32_4x O01;
                    f32_4x O23;
                    f32_4x O45;
                    f32_4x O67;
                    O01.m = _mm_shuffle_ps(O0_real.m, O0_imag.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
                    O01.mi = _mm_shuffle_epi32(O01.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
                    O23.m = _mm_shuffle_ps(O0_real.m, O0_imag.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
                    O23.mi = _mm_shuffle_epi32(O23.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
                    O45.m = _mm_shuffle_ps(O1_real.m, O1_imag.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
                    O45.mi = _mm_shuffle_epi32(O45.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
                    O67.m = _mm_shuffle_ps(O1_real.m, O1_imag.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
                    O67.mi = _mm_shuffle_epi32(O67.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));

                    f32_4x add01 = E01 + O01;
                    f32_4x add23 = E23 + O23;
                    f32_4x sub01 = E01 - O01;
                    f32_4x sub23 = E23 - O23;

                    f32_4x add45 = E45 + O45;
                    f32_4x add67 = E67 + O67;
                    f32_4x sub45 = E45 - O45;
                    f32_4x sub67 = E67 - O67;

                    _mm_store_ps(EPut, add01.m);
                    EPut += 4;
                    _mm_store_ps(OPut, sub01.m);
                    OPut += 4;
                    _mm_store_ps(EPut, add23.m);
                    EPut += 4;
                    _mm_store_ps(OPut, sub23.m);
                    OPut += 4;
                    _mm_store_ps(EPut, add45.m);
                    EPut += 4;
                    _mm_store_ps(OPut, sub45.m);
                    OPut += 4;
                    _mm_store_ps(EPut, add67.m);
                    _mm_store_ps(OPut, sub67.m);
                }
            }

            halfM = m;
            m <<= 1;
        }
    }
}

internal void
ifft_exact(u32 dftCount, Complex32 *signal)
{
    i_expect(is_pow2(dftCount));
    i_expect(dftCount > 2);
    i_expect(!((umm)signal & 0xF)); // NOTE(michiel): Must be 16 bytes aligned

    u32 halfCount = dftCount / 2;
    BitScanResult highBit = find_most_significant_set_bit(dftCount);
    for (u32 index = 0; index < halfCount; index += 2)
    {
        u32 index0 = index + 0;
        u32 index1 = index + 1;

        u32 reversedIndex0 = reverse_bits(index0, highBit.index);
        u32 reversedIndex1 = reversedIndex0 ^ halfCount;

        if (reversedIndex0 > index0)
        {
            Complex32 temp = signal[index0];
            signal[index0] = signal[reversedIndex0];
            signal[reversedIndex0] = temp;
        }
        if (reversedIndex1 > index1)
        {
            Complex32 temp = signal[index1];
            signal[index1] = signal[reversedIndex1];
            signal[reversedIndex1] = temp;
        }

        u32 index4 = index0 + halfCount;
        u32 index5 = index1 + halfCount;
        u32 reversedIndex4 = reversedIndex0 + 1;
        u32 reversedIndex5 = reversedIndex1 + 1;

        if (reversedIndex4 > index4)
        {
            Complex32 temp = signal[index4];
            signal[index4] = signal[reversedIndex4];
            signal[reversedIndex4] = temp;
        }
        if (reversedIndex5 > index5)
        {
            Complex32 temp = signal[index5];
            signal[index5] = signal[reversedIndex5];
            signal[reversedIndex5] = temp;
        }
    }

    {
        SinCos_4x sinCos = sincos_f32_4x(F32_4x(0.0f, 0.125f, 0.25f, 0.375f));

        f32_4x W4_reals;
        f32_4x W4_imags;
        W4_reals.mi = _mm_shuffle_epi32(sinCos.cos.mi, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
        W4_imags.mi = _mm_shuffle_epi32(sinCos.sin.mi, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));

        f32_4x W8_reals = sinCos.cos;
        f32_4x W8_imags = sinCos.sin;

        f32_4x ampl2_4x = F32_4x(0.5f * 0.5f * (f32)dftCount);
        f32_4x ampl4_4x = F32_4x(0.5f * 0.5f * 0.25f * (f32)dftCount);
        f32_4x ampl8_4x = F32_4x(0.5f * 0.25f * 0.125f * (f32)dftCount);

        for (u32 k = 0; k < dftCount; k += 8)
        {
            f32 *kGrab = (f32 *)(signal + k);
            f32 *kPut  = (f32 *)(signal + k);

            f32_4x EO2_0 = F32_4x(kGrab);
            kGrab += 4;
            f32_4x EO2_1 = F32_4x(kGrab);
            kGrab += 4;
            f32_4x EO2_2 = F32_4x(kGrab);
            kGrab += 4;
            f32_4x EO2_3 = F32_4x(kGrab);

            f32_4x E2_02;
            f32_4x O2_02;
            f32_4x E2_13;
            f32_4x O2_13;
            E2_02.m = _mm_shuffle_ps(EO2_0.m, EO2_2.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            O2_02.m = _mm_shuffle_ps(EO2_0.m, EO2_2.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
            E2_13.m = _mm_shuffle_ps(EO2_1.m, EO2_3.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            O2_13.m = _mm_shuffle_ps(EO2_1.m, EO2_3.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));

            f32_4x E4_02 = (E2_02 + O2_02) * ampl2_4x;
            f32_4x ac_4x = (E2_13 + O2_13) * ampl2_4x;
            f32_4x E4_13 = (E2_02 - O2_02) * ampl2_4x;
            f32_4x bd_4x = (E2_13 - O2_13) * ampl2_4x;

            f32_4x X_reals;
            X_reals.m = _mm_shuffle_ps(ac_4x.m, bd_4x.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
            X_reals.mi = _mm_shuffle_epi32(X_reals.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
            f32_4x X_imags;
            X_imags.m = _mm_shuffle_ps(ac_4x.m, bd_4x.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));
            X_imags.mi = _mm_shuffle_epi32(X_imags.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));

            f32_4x mulX0 = W4_reals * X_reals;
            f32_4x mulX1 = W4_imags * X_imags;
            f32_4x mulX2 = W4_reals * X_imags;
            f32_4x mulX3 = W4_imags * X_reals;

            f32_4x O4_reals = mulX0 - mulX1;
            f32_4x O4_imags = mulX2 + mulX3;

            f32_4x E4_0_4x;
            f32_4x E4_1_4x;
            f32_4x O4_0_4x;
            f32_4x O4_1_4x;
            E4_0_4x.m = _mm_shuffle_ps(E4_02.m, E4_13.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            E4_1_4x.m = _mm_shuffle_ps(E4_02.m, E4_13.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
            O4_0_4x.m = _mm_shuffle_ps(O4_reals.m, O4_imags.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            O4_0_4x.mi = _mm_shuffle_epi32(O4_0_4x.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
            O4_1_4x.m = _mm_shuffle_ps(O4_reals.m, O4_imags.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
            O4_1_4x.mi = _mm_shuffle_epi32(O4_1_4x.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));

            f32_4x ef_4x = (E4_1_4x + O4_1_4x) * ampl4_4x;
            f32_4x gh_4x = (E4_1_4x - O4_1_4x) * ampl4_4x;

            f32_4x Y_reals;
            f32_4x Y_imags;
            Y_reals.m = _mm_shuffle_ps(ef_4x.m, gh_4x.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
            Y_imags.m = _mm_shuffle_ps(ef_4x.m, gh_4x.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));

            f32_4x mulY0 = W8_reals * Y_reals;
            f32_4x mulY1 = W8_imags * Y_imags;
            f32_4x mulY2 = W8_reals * Y_imags;
            f32_4x mulY3 = W8_imags * Y_reals;

            f32_4x O8_reals = mulY0 - mulY1;
            f32_4x O8_imags = mulY2 + mulY3;

            f32_4x E8_0_4x = (E4_0_4x + O4_0_4x) * ampl4_4x;
            f32_4x E8_1_4x = (E4_0_4x - O4_0_4x) * ampl4_4x;
            f32_4x O8_0_4x;
            f32_4x O8_1_4x;
            O8_0_4x.m = _mm_shuffle_ps(O8_reals.m, O8_imags.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
            O8_0_4x.mi = _mm_shuffle_epi32(O8_0_4x.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
            O8_1_4x.m = _mm_shuffle_ps(O8_reals.m, O8_imags.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
            O8_1_4x.mi = _mm_shuffle_epi32(O8_1_4x.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));

            _mm_store_ps(kPut, ((E8_0_4x + O8_0_4x) * ampl8_4x).m);
            kPut += 4;
            _mm_store_ps(kPut, ((E8_1_4x + O8_1_4x) * ampl8_4x).m);
            kPut += 4;
            _mm_store_ps(kPut, ((E8_0_4x - O8_0_4x) * ampl8_4x).m);
            kPut += 4;
            _mm_store_ps(kPut, ((E8_1_4x - O8_1_4x) * ampl8_4x).m);
        }
    }

    u32 halfM = 8;
    u32 m = 16;
    u32 d = dftCount / m;

    {
        f32_4x angleMod0 = F32_4x(0.0f, 1.0f, 2.0f, 3.0f);

        while (m <= dftCount)
        {
            f32 oneOverM = 1.0f / (f32)m;
            f32_4x oneOverM_4x = F32_4x(oneOverM);
            f32_4x angleBase = oneOverM_4x * angleMod0;
            f32_4x angleStep = F32_4x(4.0f) * oneOverM_4x;

            f32_4x amplMod_4x = F32_4x((f32)d * oneOverM);

            for (u32 k = 0; k < dftCount; k += m)
            {
                Complex32 *src0 = signal + k;
                Complex32 *src1 = signal + k + halfM;

                f32_4x angles = angleBase;

                for (u32 j = 0; j < halfM; j += 8)
                {
                    f32 *EGrab = (f32 *)(src0 + j);
                    f32 *OGrab = (f32 *)(src1 + j);
                    f32 *EPut  = (f32 *)(src0 + j);
                    f32 *OPut  = (f32 *)(src1 + j);

                    f32_4x a01 = F32_4x(OGrab);
                    OGrab += 4;
                    f32_4x a23 = F32_4x(OGrab);
                    OGrab += 4;
                    f32_4x a45 = F32_4x(OGrab);
                    OGrab += 4;
                    f32_4x a67 = F32_4x(OGrab);

                    f32_4x a_real;
                    f32_4x a_imag;
                    a_real.m = _mm_shuffle_ps(a01.m, a23.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
                    a_imag.m = _mm_shuffle_ps(a01.m, a23.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));

                    SinCos_4x w0 = sincos_f32_4x(angles);
                    angles = angles + angleStep;

                    f32_4x mulX0 = w0.cos * a_real;
                    f32_4x mulX1 = w0.sin * a_imag;
                    f32_4x mulX2 = w0.cos * a_imag;
                    f32_4x mulX3 = w0.sin * a_real;

                    f32_4x O0_real = mulX0 - mulX1;
                    f32_4x O0_imag = mulX2 + mulX3;

                    f32_4x b_real;
                    f32_4x b_imag;
                    b_real.m = _mm_shuffle_ps(a45.m, a67.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
                    b_imag.m = _mm_shuffle_ps(a45.m, a67.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));

                    SinCos_4x w1 = sincos_f32_4x(angles);
                    angles = angles + angleStep;

                    f32_4x mulY0 = w1.cos * b_real;
                    f32_4x mulY1 = w1.sin * b_imag;
                    f32_4x mulY2 = w1.cos * b_imag;
                    f32_4x mulY3 = w1.sin * b_real;

                    f32_4x O1_real = mulY0 - mulY1;
                    f32_4x O1_imag = mulY2 + mulY3;

                    f32_4x E01 = F32_4x(EGrab);
                    EGrab += 4;
                    f32_4x E23 = F32_4x(EGrab);
                    EGrab += 4;
                    f32_4x E45 = F32_4x(EGrab);
                    EGrab += 4;
                    f32_4x E67 = F32_4x(EGrab);

                    f32_4x O01;
                    f32_4x O23;
                    f32_4x O45;
                    f32_4x O67;
                    O01.m = _mm_shuffle_ps(O0_real.m, O0_imag.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
                    O01.mi = _mm_shuffle_epi32(O01.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
                    O23.m = _mm_shuffle_ps(O0_real.m, O0_imag.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
                    O23.mi = _mm_shuffle_epi32(O23.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
                    O45.m = _mm_shuffle_ps(O1_real.m, O1_imag.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
                    O45.mi = _mm_shuffle_epi32(O45.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
                    O67.m = _mm_shuffle_ps(O1_real.m, O1_imag.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
                    O67.mi = _mm_shuffle_epi32(O67.mi, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));

                    f32_4x add01 = E01 + O01;
                    f32_4x add23 = E23 + O23;
                    f32_4x sub01 = E01 - O01;
                    f32_4x sub23 = E23 - O23;

                    add01 = add01 * amplMod_4x;
                    add23 = add23 * amplMod_4x;
                    sub01 = sub01 * amplMod_4x;
                    sub23 = sub23 * amplMod_4x;

                    f32_4x add45 = E45 + O45;
                    f32_4x add67 = E67 + O67;
                    f32_4x sub45 = E45 - O45;
                    f32_4x sub67 = E67 - O67;

                    add45 = add45 * amplMod_4x;
                    add67 = add67 * amplMod_4x;
                    sub45 = sub45 * amplMod_4x;
                    sub67 = sub67 * amplMod_4x;

                    _mm_store_ps(EPut, add01.m);
                    EPut += 4;
                    _mm_store_ps(OPut, sub01.m);
                    OPut += 4;
                    _mm_store_ps(EPut, add23.m);
                    EPut += 4;
                    _mm_store_ps(OPut, sub23.m);
                    OPut += 4;
                    _mm_store_ps(EPut, add45.m);
                    EPut += 4;
                    _mm_store_ps(OPut, sub45.m);
                    OPut += 4;
                    _mm_store_ps(EPut, add67.m);
                    _mm_store_ps(OPut, sub67.m);
                }
            }

            halfM = m;
            m <<= 1;
            d >>= 1;
        }
    }
}
