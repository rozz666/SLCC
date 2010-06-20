#ifndef SL_TEST_CONSOLEREPORTER_HPP
#define SL_TEST_CONSOLEREPORTER_HPP

#include <iostream>
#include <boost/algorithm/string/replace.hpp>
#include <sl/test/Reporter.hpp>
#include <sl/test/Console.hpp>

namespace sl
{
namespace test
{

class ConsoleReporter : public Reporter
{
public:

    virtual void run()
    {
        okCount_ = 0;
        failedCount_ = 0;
        errorCount_ = 0;
    }

    virtual void finish()
    {
        std::cout << "\nsummary:";

        if (okCount_ > 0) std::cout << " ok: " << green << okCount_ << rc;
        if (failedCount_ > 0) std::cout << " failed: " << red << failedCount_ << rc;
        if (errorCount_ > 0) std::cout << " errors: " << red << errorCount_ << rc;

        std::cout << std::endl;
    }

    virtual void runTestSuite(unsigned, const std::string& name)
    {
        std::cout << name << ":" << std::flush;
    }

    virtual void finishTestSuite()
    {
        std::cout << std::endl;

        for (auto it = errors_.begin(); it != errors_.end(); ++it)
        {
            std::cout << it->first << ":" << std::endl;
            std::cout << it->second.str() << std::endl;
        }

        errors_.clear();
    }

    virtual void runTest(const std::string& name)
    {
        result_ = PASSED;
        testName_ = name;
    }

    virtual void finishTest()
    {
        switch (result_)
        {
            case PASSED:
                ++okCount_;
                std::cout << green << ".";
                break;
            case FAILED:
                ++failedCount_;
                std::cout << red << "F";
                break;
            case ERROR:
                ++errorCount_;
                std::cout << red << "E";
                break;
        }
        std::cout << rc << std::flush;
    }

    virtual void outputMismatch(const std::string& actual, const std::string& expected)
    {
        result_ = FAILED;
        errors_[testName_] << " output mismatch:\n      expected: " << boost::replace_all_copy(expected, "\n", " ") << " actual: " << boost::replace_all_copy(actual, "\n", " ") << std::endl;
    }

    virtual void compilationError(const std::string& msg)
    {
        result_ = FAILED;
        std::string m = msg;
        m.erase(m.length() - 1);
        boost::replace_all(m, "\n", "\n      ");
        errors_[testName_] << " compilation error(s):\n      " << m << std::endl;
    }

    virtual void errorMissing(const std::string& expected)
    {
        result_ = FAILED;
        errors_[testName_] << " error missing: " << expected << std::endl;
    }

    virtual void unexpectedError(const std::string& msg)
    {
        result_ = FAILED;
        errors_[testName_] << " unexpected error: " << msg << std::endl;
    }

    virtual void fileNotFound(const std::string& name)
    {
        result_ = ERROR;
        errors_[testName_] << " file not found: " << name << std::endl;
    }

private:

    enum TestResult
    {
        PASSED,
        FAILED,
        ERROR
    };

    TestResult result_;
    std::string testName_;
    std::map<std::string, std::ostringstream> errors_;
    unsigned okCount_;
    unsigned failedCount_;
    unsigned errorCount_;
};

}
}

#endif /* SL_TEST_CONSOLEREPORTER_HPP */