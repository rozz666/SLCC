#include <tut/tut.hpp>
#include <tut/tut_reporter.hpp>
#include <iostream>

namespace tut
{
    
test_runner_singleton runner;

}

int main()
{
    tut::reporter reporter;
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