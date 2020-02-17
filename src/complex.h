
struct Complex32
{
    f32 real;
    f32 imag;
};

internal Complex32
complex(f32 real, f32 imag)
{
    Complex32 result;
    result.real = real;
    result.imag = imag;
    return result;
}

internal Complex32
operator +(Complex32 a, Complex32 b)
{
    Complex32 result;
    result.real = a.real + b.real;
    result.imag = a.imag + b.imag;
    return result;
}

internal Complex32 &
operator +=(Complex32 &a, Complex32 b)
{
    a = a + b;
    return a;
}

internal Complex32
operator -(Complex32 a, Complex32 b)
{
    Complex32 result;
    result.real = a.real - b.real;
    result.imag = a.imag - b.imag;
    return result;
}

internal Complex32 &
operator -=(Complex32 &a, Complex32 b)
{
    a = a - b;
    return a;
}

internal Complex32
operator *(Complex32 a, Complex32 b)
{
    Complex32 result;
    result.real = a.real * b.real - a.imag * b.imag;
    result.imag = a.real * b.imag + a.imag * b.real;
    return result;
}

internal Complex32 &
operator *=(Complex32 &a, Complex32 b)
{
    a = a * b;
    return a;
}

internal Complex32
operator *(Complex32 a, f32 scalar)
{
    Complex32 result;
    result.real = a.real * scalar;
    result.imag = a.imag * scalar;
    return result;
}

internal Complex32
operator *(f32 scalar, Complex32 a)
{
    return a * scalar;
}

internal Complex32 &
operator *=(Complex32 &a, f32 b)
{
    a = a * b;
    return a;
}

internal Complex32
operator /(Complex32 a, Complex32 b)
{
    Complex32 result;
    f32 divisor = 1.0f / (square(b.real) + square(b.imag));
    result.real = (a.real * b.real + a.imag * b.imag) * divisor;
    result.imag = (a.imag * b.real - a.real * b.imag) * divisor;
    return result;
}

internal Complex32 &
operator /=(Complex32 &a, Complex32 b)
{
    a = a / b;
    return a;
}

internal Complex32
operator /(Complex32 a, f32 b)
{
    Complex32 result;
    f32 divisor = 1.0f / b;
    result = a * divisor;
    return result;
}

internal Complex32 &
operator /=(Complex32 &a, f32 b)
{
    a = a / b;
    return a;
}

internal Complex32
square(Complex32 c)
{
    return c * c;
}

internal Complex32
conjugate(Complex32 a)
{
    Complex32 result;
    result.real = a.real;
    result.imag = -a.imag;
    return result;
}

internal Complex32
euler_power(f32 imagPower)
{
    Complex32 result;
    result.real = cos(imagPower);
    result.imag = sin(imagPower);
    return result;
}

internal f32
absolute(Complex32 c)
{
    return square_root(square(c.real) + square(c.imag));
}

internal f32
inner(Complex32 a, Complex32 b)
{
    return a.real * b.real + a.imag * b.imag;
}

internal v2
magnitude_angle(Complex32 c, f32 epsilon = 0.00001f)
{
    v2 result;
    result.x = absolute(c);
    if ((c.real > -epsilon) && (c.real < epsilon) &&
        (c.imag > -epsilon) && (c.imag < epsilon))
    {
        result.y = 0.0f;
    }
    else
    {
        result.y = atan2(c.imag, c.real);
    }
    return result;
}

