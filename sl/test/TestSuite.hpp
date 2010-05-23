#ifndef SL_TEST_TESTSUITE_HPP
#define SL_TEST_TESTSUITE_HPP

#include <sl/FilePosition.hpp>
#include <sl/test/TestSuiteBase.hpp>
#include <sl/test/Runner.hpp>

namespace sl
{

namespace test
{

template <unsigned N>
class TestSuite : public TestSuiteBase
{
public:

    TestSuite(const char *name) : TestSuiteBase(N, name)
    {
        Runner::get().registerTestSuite(*this);
    }

    virtual void runTests(Reporter& reporter)
    {
        setReporter(reporter);
        run();
    }

protected:

    typedef sl::FilePosition at;

    void run();
};

}
}

#endif /* SL_TEST_TESTSUITE_HPP */