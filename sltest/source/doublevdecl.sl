module doublevdecl;

f(int a, int a) -> int
{
    return a;
}

f() -> void
{
    new a = 1;
    new a = 2;
}

main() -> void { }