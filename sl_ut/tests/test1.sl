module test1;

identity(int a) -> int
{
    return a;
}

plus(int a) -> int
{
    return +a;
}

neg(int a) -> int
{
    return -a;
}

add(int a, int b) -> int
{
    return a + b;
}

sub(int a, int b) -> int
{
    return a - b;
}

mul(int a, int b) -> int
{
    return a * b;
}

div(int a, int b) -> int
{
    return a / b;
}

mod(int a, int b) -> int
{
    return a % b;
}

identity(float a) -> float
{
    return a;
}

plus(float a) -> float
{
    return +a;
}

neg(float a) -> float
{
    return -a;
}

add(float a, float b) -> float
{
    return a + b;
}

sub(float a, float b) -> float
{
    return a - b;
}

mul(float a, float b) -> float
{
    return a * b;
}

div(float a, float b) -> float
{
    return a / b;
}

mod(float a, float b) -> float
{
    return a % b;
}

lnot(bool a) -> bool
{
    return !a;
}

land(bool a, bool b) -> bool
{
    return a && b;
}

lor(bool a, bool b) -> bool
{
    return a || b;
}

main() -> int { }