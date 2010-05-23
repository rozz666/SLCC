module test6;

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

main() ->int { }