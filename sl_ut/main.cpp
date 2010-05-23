#include <tut/tut.hpp>
#include <iostream>
#include "tut_color_console_reporter.hpp"

namespace tut
{
    
test_runner_singleton runner;

}

int main()
{
    tut::color_console_reporter reporter;
    tut::runner.get().set_callback(&reporter);

    try
    {
        tut::runner.get().run_tests();
    }
    catch (const std::exception& ex)
    {
		std::cerr << "tut raised ex: " << ex.what() << std::endl;
        return 1;
    }
} 