module test7;

while_test(int a) -> int
{
    new i = 0;
    
    while (a > 0)
    {
        i = i + 1;
        a = a - 1;
    }

    return i;
}

main() ->int { }