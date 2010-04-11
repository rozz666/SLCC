module cycle1;

import cycle2;

f() -> int
{
    return 5;
}

main() -> int
{
    return f2(); 
}