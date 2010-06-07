module overloading;

g(int a) -> void
{
    put(7);
}

g(int a, int b) -> void
{
    put(8);
}

main() -> void
{
    g(10);
    g(10.0);
    g(1, 2);
    g(1.0, 2);
    g(1, 2.0);
    g(1.0, 2.0);
}