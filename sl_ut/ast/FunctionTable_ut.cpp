#include <boost/range/algorithm.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/assign/list_of.hpp>
#include <sl/ast/FunctionTable.hpp>
#include <sl/ast/BuiltinFunction.hpp>
#include <sl/ast/functionArgTypes.hpp>
#include <tut/tut.hpp> 

namespace tut
{

struct FunctionTable_Test_data
{
    sl::ast::FunctionTable ft;

    void ensureExactFound(const std::string& msg, const sl::ast::BuiltinFunction& f)
    {
        std::vector<sl::BasicType> argTypes = sl::ast::functionArgTypes(f);

        boost::optional<sl::ast::FunctionRef> ref = ft.findExact(f.name(), argTypes);
        ensure(msg + " found", ref);
        ensure(msg + " same", *ref == sl::ast::FunctionRef(&f));
    }

    void ensureExactNotFound(const std::string& msg, const sl::ast::BuiltinFunction& f)
    {
        std::vector<sl::BasicType> argTypes = sl::ast::functionArgTypes(f);

        boost::optional<sl::ast::FunctionRef> ref = ft.findExact(f.name(), argTypes);
        ensure_not(msg + " not found", ref);
    }

    void ensureFound(const std::string& msg, const sl::ast::BuiltinFunction& f, std::vector<sl::BasicType> argTypes)
    {
        std::vector<sl::ast::FunctionRef> found = ft.find(f.name(), argTypes);
        ensure_equals(msg + " found", found.size(), 1u);
        ensure(msg + " same", found[0] == sl::ast::FunctionRef(&f));
    }

    void ensureNotFound(const std::string& msg, const std::string& name, std::vector<sl::BasicType> argTypes)
    {
        std::vector<sl::ast::FunctionRef> found = ft.find(name, argTypes);
        ensure_equals(msg + " not found", found.size(), 0u);
    }

    void ensureAmbiguous(const std::string& msg, const std::string& name, std::vector<sl::BasicType> argTypes)
    {
        std::vector<sl::ast::FunctionRef> found = ft.find(name, argTypes);
        ensure(msg + " ambiguous", found.size() > 1);
    }
};

typedef test_group<FunctionTable_Test_data> tg;
typedef tg::object object;
tg FunctionTable_group("ast.FunctionTable");

template <>
template <>
void object::test<1>()
{
    set_test_name("findExact");

    sl::ast::BuiltinFunction f1("f", sl::void_);
    sl::ast::BuiltinFunction f2("f", sl::int_, sl::void_);
    sl::ast::BuiltinFunction f3("f", sl::float_, sl::void_);
    sl::ast::BuiltinFunction f4("f", sl::int_, sl::int_, sl::void_);
    sl::ast::BuiltinFunction f5("f", sl::int_, sl::float_, sl::void_);
    sl::ast::BuiltinFunction f6("f", sl::float_, sl::float_, sl::void_);

    ensure("insert f1", ft.insert(&f1));
    ensure("insert f2", ft.insert(&f2));
    ensure("insert f3", ft.insert(&f3));
    ensure("insert f4", ft.insert(&f4));
    ensure("insert f5", ft.insert(&f5));

    ensureExactFound("f1", f1);
    ensureExactFound("f2", f2);
    ensureExactFound("f3", f3);
    ensureExactFound("f4", f4);
    ensureExactFound("f5", f5);
    ensureExactNotFound("f6", f6);
}

template <>
template <>
void object::test<2>()
{
    set_test_name("repeated functions");

    sl::ast::BuiltinFunction f1("f", sl::void_);
    sl::ast::BuiltinFunction f2("f", sl::int_, sl::void_);
    sl::ast::BuiltinFunction f3("f", sl::int_, sl::float_, sl::void_);
    sl::ast::BuiltinFunction f1b("f", sl::void_);
    sl::ast::BuiltinFunction f2b("f", sl::int_, sl::void_);
    sl::ast::BuiltinFunction f3b("f", sl::int_, sl::float_, sl::void_);

    ensure("insert 1", ft.insert(&f1));
    ensure("insert 2", ft.insert(&f2));
    ensure("insert 3", ft.insert(&f3));

    ensure_not("insert 1b", ft.insert(&f1b));
    ensure_not("insert 2b", ft.insert(&f2b));
    ensure_not("insert 3b", ft.insert(&f3b));

    ensureExactFound("f1", f1);
    ensureExactFound("f2", f2);
    ensureExactFound("f3", f3);
}

template <>
template <>
void object::test<3>()
{
    set_test_name("find with no conversions");

    sl::ast::BuiltinFunction f1("f1", sl::int_, sl::void_);
    sl::ast::BuiltinFunction f2a("f2", sl::int_, sl::int_, sl::void_);
    sl::ast::BuiltinFunction f2b("f2", sl::int_, sl::float_, sl::void_);
    sl::ast::BuiltinFunction f3("f3", sl::float_, sl::float_, sl::void_);
    sl::ast::BuiltinFunction f4("f4", sl::bool_, sl::void_);

    ensure("insert 1", ft.insert(&f1));
    ensure("insert 2a", ft.insert(&f2a));
    ensure("insert 2b", ft.insert(&f2b));
    ensure("insert 3", ft.insert(&f3));

    ensureFound("f1", f1, boost::assign::list_of(sl::int_));
    ensureFound("f2a", f2a, boost::assign::list_of(sl::int_)(sl::int_));
    ensureFound("f2b", f2b, boost::assign::list_of(sl::int_)(sl::float_));
    ensureFound("f3", f3, boost::assign::list_of(sl::float_)(sl::float_));
    ensureNotFound("f4 1", "f4", boost::assign::list_of(sl::int_));
    ensureNotFound("f4 2", "f4", boost::assign::list_of(sl::float_));
}

template <>
template <>
void object::test<4>()
{
    set_test_name("find with conversions");

    sl::ast::BuiltinFunction f1("f1", sl::int_, sl::void_);
    sl::ast::BuiltinFunction f2a("f2", sl::int_, sl::int_, sl::void_);
    sl::ast::BuiltinFunction f2b("f2", sl::int_, sl::float_, sl::void_);
    sl::ast::BuiltinFunction f3("f3", sl::float_, sl::float_, sl::void_);

    ensure("insert 1", ft.insert(&f1));
    ensure("insert 2a", ft.insert(&f2a));
    ensure("insert 2b", ft.insert(&f2b));
    ensure("insert 3", ft.insert(&f3));

    ensureFound("f1", f1, boost::assign::list_of(sl::float_));
    ensureAmbiguous("f2a or f2b 1", "f2", boost::assign::list_of(sl::float_)(sl::int_));
    ensureAmbiguous("f2a or f2b 2", "f2", boost::assign::list_of(sl::float_)(sl::float_));
    ensureFound("f3", f3, boost::assign::list_of(sl::int_)(sl::float_));
    ensureFound("f3", f3, boost::assign::list_of(sl::int_)(sl::int_));
    ensureFound("f3", f3, boost::assign::list_of(sl::float_)(sl::int_));
}

}
