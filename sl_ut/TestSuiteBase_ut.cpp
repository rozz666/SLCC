#include <tut/tut.hpp>
#include <sl/test/TestSuiteBase.hpp>

namespace tut
{

struct TestSuiteBase_Test_data
{
    class TestTestSuite : public sl::test::TestSuiteBase
    {
    public:

        TestTestSuite(unsigned N, const char *name) : sl::test::TestSuiteBase(N, name) { }

        virtual void runTests(sl::test::Reporter& reporter)
        {
            setReporter(reporter);
        }

        using sl::test::TestSuiteBase::setReporter;
    };

    class ReporterStub : public sl::test::Reporter
    {
    public:

        std::vector<std::string> runTestCalls;

        virtual void run() { }
        virtual void finish() { }
        virtual void runTestSuite(unsigned id, const std::string& name) { }
        virtual void finishTestSuite() { }
        virtual void runTest(const std::string& name) { runTestCalls.push_back(name); }
        virtual void finishTest() { }
        virtual void outputMismatch(const std::string& actual, const std::string& expected) { }
        virtual void compilationError(const std::string& msg) { }
        virtual void errorMissing(const std::string& expected) { }
        virtual void unexpectedError(const std::string& expected) { }
        virtual void fileNotFound(const std::string& name) { }
    };
};

typedef test_group<TestSuiteBase_Test_data> tg;
typedef tg::object object;
tg TestSuiteBase_group("TestSuiteBase");

template <>
template <>
void object::test<1>()
{
    TestTestSuite tts(1u, "SuiteName");
    ReporterStub reporter;

    ensure_equals("number", tts.number(), 1u);
    ensure_equals("name", tts.name(), "SuiteName");

    tts.setReporter(reporter);

    tts.test("test", "test.sl");

    ensure_equals("call", reporter.runTestCalls.size(), 1u);
    ensure_equals("name", reporter.runTestCalls[0], "test");
}

template <>
template <>
void object::test<2>()
{
}

template <>
template <>
void object::test<3>()
{
}

}
