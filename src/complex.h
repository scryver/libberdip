
struct Complex
{
    f32 real;
    f32 imag;
};

internal Complex
complex(f32 real, f32 imag)
{
    Complex result;
    result.real = real;
    result.imag = imag;
    return result;
}

internal Complex
operator +(Complex a, Complex b)
{
    Complex result;
    result.real = a.real + b.real;
    result.imag = a.imag + b.imag;
    return result;
}

internal Complex &
operator +=(Complex &a, Complex b)
{
    a = a + b;
    return a;
}

internal Complex
operator -(Complex a, Complex b)
{
    Complex result;
    result.real = a.real - b.real;
    result.imag = a.imag - b.imag;
    return result;
}

internal Complex &
operator -=(Complex &a, Complex b)
{
    a = a - b;
    return a;
}

internal Complex
operator *(Complex a, Complex b)
{
    Complex result;
    result.real = a.real * b.real - a.imag * b.imag;
    result.imag = a.real * b.imag + a.imag * b.real;
    return result;
}

internal Complex &
operator *=(Complex &a, Complex b)
{
    a = a * b;
    return a;
}

internal Complex
operator *(Complex a, f32 scalar)
{
    Complex result;
    result.real = a.real * scalar;
    result.imag = a.imag * scalar;
    return result;
}

internal Complex
operator *(f32 scalar, Complex a)
{
    return a * scalar;
}

internal Complex &
operator *=(Complex &a, f32 b)
{
    a = a * b;
    return a;
}

internal Complex
operator /(Complex a, Complex b)
{
    Complex result;
    f32 divisor = 1.0f / (square(b.real) + square(b.imag));
    result.real = (a.real * b.real + a.imag * b.imag) * divisor;
    result.imag = (a.imag * b.real - a.real * b.imag) * divisor;
    return result;
}

internal Complex &
operator /=(Complex &a, Complex b)
{
    a = a / b;
    return a;
}

internal Complex
operator /(Complex a, f32 b)
{
    Complex result;
    f32 divisor = 1.0f / b;
    result = a * divisor;
    return result;
}

internal Complex &
operator /=(Complex &a, f32 b)
{
    a = a / b;
    return a;
}

internal Complex
square(Complex c)
{
    return c * c;
}

internal Complex
euler_power(f32 imagPower)
{
    Complex result;
    result.real = cos(imagPower);
    result.imag = sin(imagPower);
    return result;
}

internal f32
absolute(Complex c)
{
    return square_root(square(c.real) + square(c.imag));
}

internal f32
inner(Complex a, Complex b)
{
    return a.real * b.real + a.imag * b.imag;
}

