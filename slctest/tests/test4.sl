module test4;


test_swap_int() -> bool
{
    new a = 1;
    new b = 2;
    swap(a, b);
    return a == 2 && b == 1;
}

test_swap_float() -> bool
{
    new a = 1.0;
    new b = 2.0;
    swap(a, b);
    return a == 2.0 && b == 1.0;
}

test_swap_bool() -> bool
{
    new a = true;
    new b = false;
    swap(a, b);
    return a == false && b == true;
}

main() ->int { }