module cycle2;

import cycle1;

f2() -> int
{
    return f();
}