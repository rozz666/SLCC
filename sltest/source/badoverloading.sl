module badoverloading;

f(int a) -> void
{
}

f(int a, int b) -> void
{
}

f(int a, float b) -> void
{
}

main() -> void
{
    f(true);
    f(2.0, 3.0);
    f(1.0, 5);
    f(false, 5);
    f(false, 5.0);
    f(10, true);
}