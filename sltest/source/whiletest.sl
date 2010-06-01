module whiletest;

main() -> void
{
    new n = geti();
    new i = 0;
    
    while (n > 0)
    {
        i = i + 1;
        n = n - 1;
    }

    put(i);
}