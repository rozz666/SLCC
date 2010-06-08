module overloading;

g(int a) -> void
{
    put(7);
}

g(int a, int b) -> void
{
    put(8);
}

f(bool a, int b) -> void
{
    put(9);
}

main() -> void
{
    g(10);
    g(10.0);
    g(1, 2);
    g(1.0, 2);
    g(1, 2.0);
    g(1.0, 2.0);
    f(true, 1);
    f(true, 1.0);
}