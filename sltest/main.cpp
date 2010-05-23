#include <iostream>
#include <sl/test.hpp>

int main()
{
    try
    {
        sl::test::ConsoleReporter reporter;
        sl::test::Runner& runner = sl::test::Runner::get();
        runner.run(reporter);    
    }
    catch (const std::exception& ex)
    {
        std::cerr << "SL.Test raised exception: " << ex.what() << std::endl;
        return 1;
    }
}