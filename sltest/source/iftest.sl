module iftest;

max1(int a, int b) -> int
{
    if (a < b)
    {
        return b;
    }
    else
    {
        return a;
    }
}

max2(int a, int b) -> int
{
    if (a < b) { }
    else
    {
        return a;
    }

    return b;
}

max3(int a, int b) -> int
{
    if (a < b)
    {
        return b;
    }
    else { }

    return a;
}

max4(int a, int b) -> int
{
    if (a < b)
    {
        return b;
    }

    return a;
}

main() -> void
{
    new a = geti();
    new b = geti();
    put(max1(a, b));
    put(max2(a, b));
    put(max3(a, b));
    put(max4(a, b));
}