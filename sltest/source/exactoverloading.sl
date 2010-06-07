module exactoverloading;

f(int a) -> void
{
    put(1);
}

f(float a) -> void
{
    put(2);
}

f(int a, int b) -> void
{
    put(3);
}

f(float a, int b) -> void
{
    put(4);
}

f(int a, float b) -> void
{
    put(5);
}

f(float a, float b) -> void
{
    put(6);
}

main() -> void
{
    f(10);
    f(10.0);
    f(1, 2);
    f(1.0, 2);
    f(1, 2.0);
    f(1.0, 2.0);
}