#include <tut/tut.hpp> 
#include <string>
#include <fstream>
#include <cmath>
#include <limits>
#include <boost/foreach.hpp>
#include <sl/cst.hpp>
#include <sl/ast.hpp>
#include <sl/vm.hpp>
#include <sl/parseCst.hpp>
#include <sl/parseAst.hpp>
#include <sl/codeGen.hpp>

namespace tut
{

struct Test_data { };

typedef test_group<Test_data> tg;
typedef tg::object object;
tg BPath_group("SL_FUNC_TEST");

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
    boost::optional<cst::Module> module = parseFile(fin, errorLogger);

    if (!module) throw CompileError();

    sl::ast::Module parsed = parseModule(*module, errorLogger);

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

    boost::optional<cst::Module> module = parseFile(fin, errorLogger);

    if (!module) return false;

    parseModule(*module, errorLogger);

    if (!errorLogger.errors().empty())
    {
        std::ofstream lf(fname + "log");
        errorLogger.print(lf);
    }

    return errorLogger.errors().empty();
}

struct LoadModule
{
    std::string basePath;

    LoadModule(const std::string& basePath) : basePath(basePath) { }

    boost::optional<sl::cst::Module> operator()(const std::string& name, sl::ErrorLogger& errorLogger)
    {
        std::ifstream fin(basePath + name + ".sl");

        if (!fin.is_open())
        {
            // error
            return boost::none;
        }

        return sl::parseFile(fin, errorLogger);
    }
};

template <>
template <>
void object::test<1>()
{
    set_test_name("operators");

    sl::vm::Module module = compileFile("tests\\test1.sl");
    sl::vm::Environment env(1024);

    using sl::int_t;
    using sl::float_t;

    ensure("identity [int]", module.call<int_t>(sl::vm::FunctionCall("identity$i").p(12345), env) == 12345);
    ensure("plus test [int]", module.call<int_t>(sl::vm::FunctionCall("plus$i").p(6), env) == 6);
    ensure("neg test [int]", module.call<int_t>(sl::vm::FunctionCall("neg$i").p(6), env) == -6);
    ensure("add test [int]", module.call<int_t>(sl::vm::FunctionCall("add$ii").p(4).p(5), env) == 4 + 5);
    ensure("sub test [int]", module.call<int_t>(sl::vm::FunctionCall("sub$ii").p(4).p(5), env) == 4 - 5);
    ensure("mul test [int]", module.call<int_t>(sl::vm::FunctionCall("mul$ii").p(4).p(5), env) == 4 * 5);
    ensure("div test [int]", module.call<int_t>(sl::vm::FunctionCall("div$ii").p(14).p(6), env) == 14 / 6);
    ensure("mod test [int]", module.call<int_t>(sl::vm::FunctionCall("mod$ii").p(14).p(6), env) == 14 % 6);
    ensure("identity [float]", module.call<float_t>(sl::vm::FunctionCall("identity$f").p(12345.0f), env) == 12345.0f);
    ensure("plus test [float]", module.call<float_t>(sl::vm::FunctionCall("plus$f").p(6.0f), env) == 6.0f);
    ensure("neg test [float]", module.call<float_t>(sl::vm::FunctionCall("neg$f").p(6.0f), env) == -6.0f);
    ensure("add test [float]", module.call<float_t>(sl::vm::FunctionCall("add$ff").p(4.0f).p(5.0f), env) == 4.0f + 5.0f);
    ensure("sub test [float]", module.call<float_t>(sl::vm::FunctionCall("sub$ff").p(4.0f).p(5.0f), env) == 4.0f - 5.0f);
    ensure("mul test [float]", module.call<float_t>(sl::vm::FunctionCall("mul$ff").p(4.0f).p(5.0f), env) == 4.0f * 5.0f);
    ensure("div test [float]", module.call<float_t>(sl::vm::FunctionCall("div$ff").p(14.0f).p(6.0f), env) == 14.0f / 6.0f);
    ensure_distance("mod test [float]", module.call<float_t>(sl::vm::FunctionCall("mod$ff").p(14.0f).p(6.0f), env), std::fmod(14.0f, 6.0f), std::numeric_limits<float>::epsilon() * 14.0f);
    ensure("land test [bool]", module.call<int_t>(sl::vm::FunctionCall("land$bb").p(0).p(0), env) == 0);
    ensure("land test [bool]", module.call<int_t>(sl::vm::FunctionCall("land$bb").p(0).p(1), env) == 0);
    ensure("land test [bool]", module.call<int_t>(sl::vm::FunctionCall("land$bb").p(1).p(0), env) == 0);
    ensure("land test [bool]", module.call<int_t>(sl::vm::FunctionCall("land$bb").p(1).p(1), env) == 1);
    ensure("lor test [bool]", module.call<int_t>(sl::vm::FunctionCall("lor$bb").p(0).p(0), env) == 0);
    ensure("lor test [bool]", module.call<int_t>(sl::vm::FunctionCall("lor$bb").p(0).p(1), env) == 1);
    ensure("lor test [bool]", module.call<int_t>(sl::vm::FunctionCall("lor$bb").p(1).p(0), env) == 1);
    ensure("lor test [bool]", module.call<int_t>(sl::vm::FunctionCall("lor$bb").p(1).p(1), env) == 1);
    ensure("lnot test [bool]", module.call<int_t>(sl::vm::FunctionCall("lnot$b").p(0), env) == 1);
    ensure("lnot test [bool]", module.call<int_t>(sl::vm::FunctionCall("lnot$b").p(1), env) == 0);
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

    using sl::int_t;
    using sl::float_t;

    ensure("swap [int]", module.call<int_t>(sl::vm::FunctionCall("test_swap_int$"), env) == 1);
    ensure("swap [float]", module.call<int_t>(sl::vm::FunctionCall("test_swap_float$"), env) == 1);
    ensure("swap [bool]", module.call<int_t>(sl::vm::FunctionCall("test_swap_bool$"), env) == 1);
}

template <>
template <>
void object::test<5>()
{
    set_test_name("passing by reference");

    sl::vm::Module module = compileFile("tests\\test5.sl");
    sl::vm::Environment env(1024);

    using sl::int_t;
    using sl::float_t;

    ensure("pass by ref [int]", module.call<int_t>(sl::vm::FunctionCall("pass_by_ref_int$"), env) == 1);
    ensure("pass by ref [float]", module.call<int_t>(sl::vm::FunctionCall("pass_by_ref_float$"), env) == 1);
    ensure("pass by ref [bool]", module.call<int_t>(sl::vm::FunctionCall("pass_by_ref_bool$"), env) == 1);
}

template <>
template <>
void object::test<6>()
{
    set_test_name("if");

    sl::vm::Module module = compileFile("tests\\test6.sl");
    sl::vm::Environment env(1024);
    sl::vm::DataAddr sp = env.sp();

    using sl::int_t;
    using sl::float_t;

    ensure("max1", module.call<int_t>(sl::vm::FunctionCall("max1$ii").p(2).p(1), env) == 2);
    ensure_equals("stack1", env.sp(), sp);
    ensure("max1", module.call<int_t>(sl::vm::FunctionCall("max1$ii").p(1).p(2), env) == 2);
    ensure_equals("stack2", env.sp(), sp);
    ensure("max2", module.call<int_t>(sl::vm::FunctionCall("max2$ii").p(2).p(1), env) == 2);
    ensure_equals("stack3", env.sp(), sp);
    ensure("max2", module.call<int_t>(sl::vm::FunctionCall("max2$ii").p(1).p(2), env) == 2);
    ensure_equals("stack4", env.sp(), sp);
    ensure("max3", module.call<int_t>(sl::vm::FunctionCall("max3$ii").p(2).p(1), env) == 2);
    ensure_equals("stack5", env.sp(), sp);
    ensure("max3", module.call<int_t>(sl::vm::FunctionCall("max3$ii").p(1).p(2), env) == 2);
    ensure_equals("stack6", env.sp(), sp);
    ensure("max4", module.call<int_t>(sl::vm::FunctionCall("max4$ii").p(2).p(1), env) == 2);
    ensure_equals("stack7", env.sp(), sp);
    ensure("max4", module.call<int_t>(sl::vm::FunctionCall("max4$ii").p(1).p(2), env) == 2);
    ensure_equals("stack8", env.sp(), sp);
}

template <>
template <>
void object::test<7>()
{
    ensure("delete in if statement", !validFile("tests\\bad4.sl"));
    ensure("delete in while loop", !validFile("tests\\bad5.sl"));
}

template <>
template <>
void object::test<8>()
{
    set_test_name("while loop");

    sl::vm::Module module = compileFile("tests\\test7.sl");
    sl::vm::Environment env(1024);
    sl::vm::DataAddr sp = env.sp();

    using sl::int_t;
    using sl::float_t;

    ensure_equals("while test1", module.call<int_t>(sl::vm::FunctionCall("while_test$i").p(0), env), 0);
    ensure_equals("stack1", env.sp(), sp);
    ensure_equals("while test2", module.call<int_t>(sl::vm::FunctionCall("while_test$i").p(-10), env), 0);
    ensure_equals("stack2", env.sp(), sp);
    ensure_equals("while test3", module.call<int_t>(sl::vm::FunctionCall("while_test$i").p(10), env), 10);
    ensure_equals("stack3", env.sp(), sp);
}

template <>
template <>
void object::test<9>()
{
    using namespace sl;

    set_test_name("module cycle");

    ErrorLogger errorLogger("???");
    ast::ModuleMap mm;
    LoadModule lm("tests\\modules\\");

    ensure("parse modules", parseModules("cycle1", mm, lm, errorLogger));
    ensure_equals("size", mm.size(), 2u);
    ensure("cycle1", mm.contains("cycle1"));
    ensure("cycle2", mm.contains("cycle2"));
}

template <>
template <>
void object::test<10>()
{
    using namespace sl;

    set_test_name("module tree");

    ErrorLogger errorLogger("???");
    ast::ModuleMap mm;
    LoadModule lm("tests\\modules\\");

    ensure("parse modules", parseModules("tree1", mm, lm, errorLogger));
    ensure_equals("size", mm.size(), 5u);
    ensure("tree1", mm.contains("tree1"));
    ensure("tree2", mm.contains("tree2"));
    ensure("tree3", mm.contains("tree3"));
    ensure("tree4", mm.contains("tree4"));
    ensure("tree5", mm.contains("tree5"));
}

}
