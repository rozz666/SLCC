#ifndef SL_AST_FUNCTIONTABLE_HPP
#define SL_AST_FUNCTIONTABLE_HPP

#include <string>
#include <unordered_map>
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <sl/ast/FunctionRef.hpp>
#include <sl/ast/functionMangledName.hpp>
#include <sl/ast/functionName.hpp>
#include <sl/ast/functionArgTypes.hpp>

namespace sl
{
namespace ast
{

class FunctionTable
{
    typedef std::unordered_multimap<std::string, FunctionRef> C;
public:

    typedef std::pair<C::const_iterator, C::const_iterator> Range;

    FunctionTable() { }

    bool insert(const FunctionRef& f)
    {
        std::string name = functionName(f);
        if (findExact(name, functionArgTypes(f))) return false;

        functions_.insert(std::make_pair(name, f));

        return true;
    }

    template <typename TypeRange>
    std::vector<FunctionRef> find(const std::string& name, const TypeRange& args) const
    {
        Range r = functions_.equal_range(name);
        std::string mangledName = functionMangledName(name, functionSuffix(args));
        std::vector<FunctionRef> found;
        
        BOOST_FOREACH(C::const_reference f, r)
        {
            if (functionMangledName(f.second) == mangledName)
                found.push_back(f.second);
        }

        return found;
    }

    template <typename TypeRange>
    boost::optional<FunctionRef> findExact(std::string name, const TypeRange& args) const
    {
        Range r = functions_.equal_range(name);
        std::string mangledName = functionMangledName(name, functionSuffix(args));

        C::const_iterator it = boost::find_if(r, [&mangledName](const C::value_type& f) { return sl::ast::functionMangledName(f.second) == mangledName; });

        if (it == boost::end(r)) return boost::none;

        return it->second;
    }

private:

    C functions_;
};

}
}
#endif /* SL_AST_FUNCTIONTABLE_HPP */
