module swaptest;

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
        new left = geti();
        new right = geti();
        swap(left, right);
        put(left);
        put(right);
    }
    {
        new left = getf();
        new right = getf();
        swap(left, right);
        put(left);
        put(right);
    }
    {
        new left = getb();
        new right = getb();
        swap(left, right);
        put(left);
        put(right);
    }
}