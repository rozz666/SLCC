#ifndef SL_TEST_RUNNER_HPP
#define SL_TEST_RUNNER_HPP

#include <map>
#include <sl/test/Reporter.hpp>
#include <sl/test/TestSuiteBase.hpp>

namespace sl
{
namespace test
{

class Runner
{
public:

    static Runner& get()
    {
        static Runner runner;
        return runner;
    }

    void run(Reporter& reporter)
    {
        reporter.run();

        for (auto it = suites_.begin(); it != suites_.end(); ++it)
        {
            reporter.runTestSuite(it->second->number(), it->second->name());
            it->second->runTests(reporter);
            reporter.finishTestSuite();
        }

        reporter.finish();
    }

    void registerTestSuite(TestSuiteBase& suite)
    {
        suites_.insert(Suites::value_type(suite.number(), &suite));
    }

private:

    typedef std::map<unsigned, TestSuiteBase *> Suites;
    Suites suites_;
};

}
}

#endif /* SL_TEST_RUNNER_HPP */