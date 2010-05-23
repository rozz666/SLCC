module test5;


inc(ref int a) -> void
{
    a = a + 1;
}

pass_by_ref_int() -> bool
{
    new a = 1;
    inc(a);
    return a == 2;
}

inc(ref float a) -> void
{
    a = a + 1;
}

pass_by_ref_float() -> bool
{
    new a = 1.0;
    inc(a);
    return a == 2.0;
}

inc(ref bool a) -> void
{
    a = !a;
}

pass_by_ref_bool() -> bool
{
    new a = true;
    inc(a);
    return a == false;
}

main() ->int { }