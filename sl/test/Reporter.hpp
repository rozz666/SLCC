#ifndef SL_TEST_REPORTER_HPP
#define SL_TEST_REPORTER_HPP

#include <string>

namespace sl
{
namespace test
{

class Reporter
{
public:

    virtual void run() = 0;
    virtual void finish() = 0;
    virtual void runTestSuite(unsigned id, const std::string& name) = 0;
    virtual void finishTestSuite() = 0;
    virtual void runTest(const std::string& name) = 0;
    virtual void finishTest() = 0;
    virtual void outputMismatch(const std::string& actual, const std::string& expected) = 0;
    virtual void compilationError(const std::string& msg) = 0;
    virtual void errorMissing(const std::string& expected) = 0;
    virtual void unexpectedError(const std::string& expected) = 0;
    virtual void fileNotFound(const std::string& name) = 0;

protected:

    ~Reporter() { }
};

}
}

#endif /* SL_TEST_REPORTER_HPP */