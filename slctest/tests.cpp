#include <tut/tut.hpp> 
#include <string>
#include <fstream>
#include <cmath>
#include <limits>
#include <boost/foreach.hpp>
#include "../slrun/ast.hpp"
#include "../slrun/st.hpp"
#include "../slvm/vm.hpp"
#include "../slrun/parseAst.hpp"
#include "../slrun/parseSt.hpp"
#include "../slrun/codeGen.hpp"

namespace tut
{

struct Test_data { };

typedef test_group<Test_data> tg;
typedef tg::object object;
tg BPath_group("SL_TEST");

struct CompileError : public std::runtime_error
{
    CompileError() : std::runtime_error("compilation error") { }
};

sl::vm::Module compileFile(const std::string& fname)
{
    using namespace sl;

    std::ifstream fin(fname.c_str());

    assert(fin.is_open());

    sl::ErrorLogger errorLogger(fname);
    boost::optional<ast::Module> module = parseFile(fin, errorLogger);

    if (!module) throw CompileError();

    sl::st::Module parsed = parseModule(*module, errorLogger);

    sl::FunctionAddrMap fam;

    sl::vm::BytecodeBuffer bb = generateBytecode(parsed, fam);

    {
        std::ofstream fout((fname + "asm").c_str());

        exportToAsm(bb, fout);
    }

    sl::vm::Module bcModule(std::move(bb));

    BOOST_FOREACH(const sl::FunctionAddrMap::value_type& f, fam)
    {
        bcModule.registerFunction(f.second, f.first);
    }

    return bcModule;
}

bool validFile(const std::string& fname)
{
    using namespace sl;

    std::ifstream fin(fname.c_str());

    assert(fin.is_open());

    sl::ErrorLogger errorLogger(fname);

    boost::optional<ast::Module> module = parseFile(fin, errorLogger);

    if (!module) return false;

    parseModule(*module, errorLogger);

    return !errorLogger.hasErrors();
}

template <>
template <>
void object::test<1>()
{
    set_test_name("operators");

    sl::vm::Module module = compileFile("tests\\test1.sl");
    sl::vm::Environment env(1024);

    ensure("identity [int]", module.call<int>(sl::vm::FunctionCall("identity$i").p(12345), env) == 12345);
    ensure("plus test [int]", module.call<int>(sl::vm::FunctionCall("plus$i").p(6), env) == 6);
    ensure("neg test [int]", module.call<int>(sl::vm::FunctionCall("neg$i").p(6), env) == -6);
    ensure("add test [int]", module.call<int>(sl::vm::FunctionCall("add$ii").p(4).p(5), env) == 4 + 5);
    ensure("sub test [int]", module.call<int>(sl::vm::FunctionCall("sub$ii").p(4).p(5), env) == 4 - 5);
    ensure("mul test [int]", module.call<int>(sl::vm::FunctionCall("mul$ii").p(4).p(5), env) == 4 * 5);
    ensure("div test [int]", module.call<int>(sl::vm::FunctionCall("div$ii").p(14).p(6), env) == 14 / 6);
    ensure("mod test [int]", module.call<int>(sl::vm::FunctionCall("mod$ii").p(14).p(6), env) == 14 % 6);
    ensure("identity [float]", module.call<float>(sl::vm::FunctionCall("identity$f").p(12345.0f), env) == 12345.0f);
    ensure("plus test [float]", module.call<float>(sl::vm::FunctionCall("plus$f").p(6.0f), env) == 6.0f);
    ensure("neg test [float]", module.call<float>(sl::vm::FunctionCall("neg$f").p(6.0f), env) == -6.0f);
    ensure("add test [float]", module.call<float>(sl::vm::FunctionCall("add$ff").p(4.0f).p(5.0f), env) == 4.0f + 5.0f);
    ensure("sub test [float]", module.call<float>(sl::vm::FunctionCall("sub$ff").p(4.0f).p(5.0f), env) == 4.0f - 5.0f);
    ensure("mul test [float]", module.call<float>(sl::vm::FunctionCall("mul$ff").p(4.0f).p(5.0f), env) == 4.0f * 5.0f);
    ensure("div test [float]", module.call<float>(sl::vm::FunctionCall("div$ff").p(14.0f).p(6.0f), env) == 14.0f / 6.0f);
    ensure_distance("mod test [float]", module.call<float>(sl::vm::FunctionCall("mod$ff").p(14.0f).p(6.0f), env), std::fmod(14.0f, 6.0f), std::numeric_limits<float>::epsilon() * 14.0f);
    ensure("land test [bool]", module.call<int>(sl::vm::FunctionCall("land$bb").p(0).p(0), env) == 0);
    ensure("land test [bool]", module.call<int>(sl::vm::FunctionCall("land$bb").p(0).p(1), env) == 0);
    ensure("land test [bool]", module.call<int>(sl::vm::FunctionCall("land$bb").p(1).p(0), env) == 0);
    ensure("land test [bool]", module.call<int>(sl::vm::FunctionCall("land$bb").p(1).p(1), env) == 1);
    ensure("lor test [bool]", module.call<int>(sl::vm::FunctionCall("lor$bb").p(0).p(0), env) == 0);
    ensure("lor test [bool]", module.call<int>(sl::vm::FunctionCall("lor$bb").p(0).p(1), env) == 1);
    ensure("lor test [bool]", module.call<int>(sl::vm::FunctionCall("lor$bb").p(1).p(0), env) == 1);
    ensure("lor test [bool]", module.call<int>(sl::vm::FunctionCall("lor$bb").p(1).p(1), env) == 1);
    ensure("lnot test [bool]", module.call<int>(sl::vm::FunctionCall("lnot$b").p(0), env) == 1);
    ensure("lnot test [bool]", module.call<int>(sl::vm::FunctionCall("lnot$b").p(1), env) == 0);
}

template <>
template <>
void object::test<2>()
{
    set_test_name("variables");
    ensure("repeated function parameters", !validFile("tests\\bad1.sl"));
    ensure("repeated local variables", !validFile("tests\\bad3.sl"));
    ensure("new and delete", validFile("tests\\test2.sl"));
}

template <>
template <>
void object::test<3>()
{
    ensure("repeated functions", !validFile("tests\\bad2.sl"));
}

template <>
template <>
void object::test<4>()
{
    set_test_name("swap");

    sl::vm::Module module = compileFile("tests\\test4.sl");
    sl::vm::Environment env(1024);

    ensure("swap [int]", module.call<int>(sl::vm::FunctionCall("test_swap_int$"), env) == 1);
    ensure("swap [float]", module.call<int>(sl::vm::FunctionCall("test_swap_float$"), env) == 1);
    ensure("swap [bool]", module.call<int>(sl::vm::FunctionCall("test_swap_bool$"), env) == 1);
}

template <>
template <>
void object::test<5>()
{
}


}
