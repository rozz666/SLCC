module bad5;

f() -> void
{
    new a = 1;
    
    while (a < 0)
    {
        delete a;
    }
}


main() ->int { }