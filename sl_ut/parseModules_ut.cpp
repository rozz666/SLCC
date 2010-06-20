#include <tut/tut.hpp>
#include <fstream>
#include <sl/parseAst.hpp>
#include <sl/parseCst.hpp>

namespace tut
{

struct LoadModuleBase
{
    std::map<std::string, std::string> modules;

    boost::optional<sl::cst::Module> operator()(const std::string& name, sl::ErrorLogger& errorLogger)
    {
        std::istringstream is(modules[name]);
        return sl::parseFile(name, is, errorLogger);
    }
};

struct LoadModuleCycle : LoadModuleBase
{
    LoadModuleCycle()
    {
        modules["cycle1"] = "module cycle1;\nimport cycle2;";
        modules["cycle2"] = "module cycle2;\nimport cycle1;";
    }
};

struct LoadModuleTree : LoadModuleBase
{
    LoadModuleTree()
    {
        modules["tree1"] = "module tree1;\nimport tree2;\nimport tree3;";
        modules["tree2"] = "module tree2;\nimport tree4;\nimport tree5;";
        modules["tree3"] = "module tree3;";
        modules["tree4"] = "module tree4;";
        modules["tree5"] = "module tree5;";
    }
};

struct parseModules_Test_data
{
    sl::ErrorLogger errorLogger;
    sl::ast::ModuleMap mm;    
};

typedef test_group<parseModules_Test_data> tg;
typedef tg::object object;
tg CC_group("parseModules");

template <>
template <>
void object::test<1>()
{
    using namespace sl;

    set_test_name("module cycle");

    ensure("parse modules", parseModules("cycle1", mm, LoadModuleCycle(), errorLogger));
    ensure_equals("size", mm.size(), 2u);
    ensure("cycle1", mm.contains("cycle1"));
    ensure("cycle2", mm.contains("cycle2"));
}

template <>
template <>
void object::test<2>()
{
    using namespace sl;

    set_test_name("module tree");

    ensure("parse modules", parseModules("tree1", mm, LoadModuleTree(), errorLogger));
    ensure_equals("size", mm.size(), 5u);
    ensure("tree1", mm.contains("tree1"));
    ensure("tree2", mm.contains("tree2"));
    ensure("tree3", mm.contains("tree3"));
    ensure("tree4", mm.contains("tree4"));
    ensure("tree5", mm.contains("tree5"));
}

}
