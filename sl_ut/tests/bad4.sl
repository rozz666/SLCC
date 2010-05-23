module bad4;

f() -> void
{
    new a = 1;
    
    if (a < 0)
    {
        delete a;
    }
}


main() ->int { }