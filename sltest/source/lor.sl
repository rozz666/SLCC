module add;

getBool() -> bool
{
    if (geti() == 0)
    {
        return false;
    }
    
    return true;
}

putBool(bool v) -> void
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
    new left = getBool();
    new right = getBool();
    
    putBool(left || right);

}