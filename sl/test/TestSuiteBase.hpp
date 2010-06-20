#ifndef SL_TEST_TESTSUITEBASE_HPP
#define SL_TEST_TESTSUITEBASE_HPP

#include <string>
#include <sl/test/Reporter.hpp>
#include <sl/test/TestOps.hpp>

namespace sl
{

namespace test
{

class TestSuiteBase
{
public:

    TestSuiteBase(unsigned N, const char *name) : number_(N), name_(name) { }

    unsigned number() const { return number_; }
    const std::string& name() const { return name_; }

    virtual void runTests(Reporter& reporter) = 0;

    TestOps test(const char *name, const char *file)
    {
        reporter_->runTest(name);
        return TestOps(file, *reporter_);
    }

protected:

    ~TestSuiteBase() { }

    void setReporter(Reporter& reporter) { reporter_ = &reporter; }

private:

    unsigned number_;
    std::string name_;
    Reporter *reporter_;
};

}
}

#endif /* SL_TEST_TESTSUITEBASE_HPP */
