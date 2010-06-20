module references;

copy(int src, ref int dst) -> void
{
    dst = src;
}

copy(float src, ref float dst) -> void
{
    dst = src;
}

copy(bool src, ref bool dst) -> void
{
    dst = src;
}

getb() -> bool
{
    if (geti() == 0)
    {
        return false;
    }
    
    return true;
}

put(bool v) -> void
{
    if (v)
    {
        put(1);
    }
    else
    {
        put(0);
    }
}

main() -> void
{
    {
        new a = geti();
        new b = -a;
        copy(a, b);
        put(b);
    }
    {
        new a = getf();
        new b = -a;
        copy(a, b);
        put(b);
    }
    {
        new a = getb();
        new b = !a;
        copy(a, b);
        put(b);
    }
}