#ifndef SL_TEST_TESTOPS_HPP
#define SL_TEST_TESTOPS_HPP

#include <fstream>
#include <string>
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

class TestOps
{
public:

    TestOps(const std::string& file, Reporter& reporter)
        : logger_(file), reporter_(&reporter), fileOk_(compile(file))    { }

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

        return MatchOps(bytecode_, *reporter_).match(input, output);
    }        

    void expect(const sl::err::Message& msg)
    { 
        if (!fileOk_) return;

        if (logger_.errors().size() == 1)
        {
            const sl::err::Message& actual = logger_.errors().front();
            
            if (actual.id == msg.id && actual.pos == msg.pos && actual.text == msg.text) return;
        }

        std::ostringstream os;
        logger_.print(os);
        reporter_->compilationError(os.str());
    }

private:

    ErrorLogger logger_;
    Reporter *reporter_;
    sl::vm::BytecodeBuffer bytecode_;
    bool fileOk_;

    bool compile(const std::string& file)
    {
        std::ifstream fin(file.c_str());

        if (!fin.is_open())
        {
            reporter_->fileNotFound(file);
            return false;
        }

        if (boost::optional<cst::Module> module = parseFile(fin, logger_))
        {
            ast::Module parsed = parseModule(*module, logger_);

            FunctionAddrMap fam;

            generateBytecode(parsed, fam).swap(bytecode_);
        }

        return true;
    }
};

}
}

#endif /* SL_TEST_TESTOPS_HPP */