#ifndef SL_TEST_TESTOPS_HPP
#define SL_TEST_TESTOPS_HPP

#include <fstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/dynamic_bitset.hpp>
#include <sl/vm/BytecodeBuffer.hpp>
#include <sl/vm/Environment.hpp>
#include <sl/err/messages.hpp>
#include <sl/test/Reporter.hpp>
#include <sl/ErrorLogger.hpp>
#include <sl/parseCst.hpp>
#include <sl/parseAst.hpp>
#include <sl/codeGen.hpp>

namespace sl
{
namespace test
{

class MatchOps
{
public:

    MatchOps() : ignore_(true) { }

    MatchOps(const sl::vm::BytecodeBuffer& bytecode, Reporter& reporter)
        : ignore_(false), bytecode_(&bytecode), reporter_(&reporter) { }

    MatchOps& match(const std::string& input, const std::string& expectedOutput)
    {
        if (ignore_) return *this;

        std::istringstream is(input);
        std::ostringstream os;

        sl::vm::Environment env(1024, is, os);

        env.execute(&bytecode_->front(), 0);

        std::string output = os.str();

        if (output != expectedOutput)
        {
            reporter_->outputMismatch(output, expectedOutput);
        }

        return *this;
    }

private:

    bool ignore_;
    const sl::vm::BytecodeBuffer *bytecode_;
    Reporter *reporter_;
};

class ExpectOps
{
public:

    ExpectOps() : ignore_(true) { }
    ExpectOps(const ErrorLogger& errorLogger, boost::dynamic_bitset<>& expected, Reporter& reporter)
        : ignore_(false), last_(true), logger_(&errorLogger), expected_(&expected), reporter_(&reporter) { }

    ExpectOps(const ExpectOps& right) : ignore_(right.ignore_), last_(right.last_), logger_(right.logger_), expected_(right.expected_), reporter_(right.reporter_)
    {
        right.last_ = false;
    }

    ExpectOps expect(const err::Message& msg)
    {
        if (ignore_) return *this;

        boost::dynamic_bitset<>& expected = *expected_;

        for (ErrorLogger::Errors::size_type i = 0; i != logger_->errors().size(); ++i)
        {
            const err::Message& m = logger_->errors()[i];

            if (!expected.test(i) && m.id == msg.id && m.pos == msg.pos && m.text == msg.text)
            {
                expected.set(i);

                return *this;
            }
        }

        reporter_->errorMissing(boost::lexical_cast<std::string>(msg));

        return *this;
    }

    ~ExpectOps()
    {
        if (!last_) return;

        for (ErrorLogger::Errors::size_type i = 0; i != logger_->errors().size(); ++i)
        {
            if (!expected_->test(i)) reporter_->unexpectedError(boost::lexical_cast<std::string>(logger_->errors()[i]));
        }
    }

private:
    bool ignore_;
    mutable bool last_;
    const ErrorLogger *logger_;
    boost::dynamic_bitset<> *expected_;
    Reporter *reporter_;
};

class TestOps : boost::noncopyable
{
public:

    TestOps(const std::string& file, Reporter& reporter)
        : reporter_(&reporter), fileOk_(compile(file)), expected_(logger_.errors().size()) { }

    TestOps(TestOps&& right) : logger_(right.logger_), reporter_(right.reporter_)
    {
        right.reporter_ = nullptr;
    }

    ~TestOps()
    {
        if (reporter_) reporter_->finishTest();
    }

    MatchOps match(const std::string& input, const std::string& output)
    {
        if (!fileOk_) return MatchOps();

        if (!logger_.errors().empty())
        {
            std::ostringstream os;
            logger_.print(os);
            reporter_->compilationError(os.str());
            return MatchOps();
        }

        if (bytecode_.empty())
        {
            reporter_->compilationError("No bytecode\n");
            return MatchOps();
        }

        return MatchOps(bytecode_, *reporter_).match(input, output);
    }        

    ExpectOps expect(const sl::err::Message& msg)
    { 
        if (!fileOk_) return ExpectOps();
     
        return ExpectOps(logger_, expected_, *reporter_).expect(msg);
    }

private:

    ErrorLogger logger_;
    Reporter *reporter_;
    sl::vm::BytecodeBuffer bytecode_;
    bool fileOk_;
    boost::dynamic_bitset<> expected_;

    bool compile(const std::string& file)
    {
        std::ifstream fin(file.c_str());

        if (!fin.is_open())
        {
            reporter_->fileNotFound(file);
            return false;
        }

        if (boost::optional<cst::Module> module = parseFile(file, fin, logger_))
        {
            ast::Module parsed = parseModule(*module, logger_);

            if (!logger_.errors().empty()) return true;

            FunctionAddrMap fam;

            generateBytecode(parsed, fam).swap(bytecode_);
        }

        return true;
    }
};

}
}

#endif /* SL_TEST_TESTOPS_HPP */