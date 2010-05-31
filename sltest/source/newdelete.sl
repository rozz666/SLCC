module newdelete;

print(int i) -> void
{
    put(1);
    put(i);
}

print(float f) -> void
{
    put(2);
    put(f);
}

print(bool b) -> void
{
    put(3);
    if (b)
    {
		put(1);
	}
    else
    {
        put(0);
    }
}

main() -> void
{
    new a = 5;
    print(a);
    delete a;
    new a = 5.0;
    print(a);
    delete a;
    new a = true;
    print(a);
    delete a;
    new a = 6;
    print(a);
}