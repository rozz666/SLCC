module newdeletebad;

f() -> void
{
    new a = 1;
    
    if (a < 0)
    {
        delete a;
    }
}

g() -> void
{
    new a = 1;
    
    while (a < 0)
    {
        delete a;
    }
}

h() -> void
{
    new a = 10;
    delete a;
    a = 5;
}

main() -> void { }