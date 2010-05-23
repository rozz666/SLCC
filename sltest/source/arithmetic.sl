module arithmetic;

main() -> int
{
    {
        new tmp = geti();
        put(tmp + geti());
    }
    {
        new tmp = geti();
        put(tmp - geti());
    }
    {
        new tmp = geti();
        put(tmp * geti());
    }
    {
        new tmp = geti();
        put(tmp / geti());
    }
}