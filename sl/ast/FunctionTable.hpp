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
#include <sl/ast/isConvertible.hpp>

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
        std::vector<sl::BasicType> srcArgs(boost::begin(args), boost::end(args));
        unsigned rank = 0;
        
        BOOST_FOREACH(C::const_reference f, r)
        {
            unsigned r = rankMatch(srcArgs, functionArgTypes(f.second));

            if (r > rank)
            {
                found.clear();
                rank = r;
            }

            if (r == rank && rank > 0) found.push_back(f.second);
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

    static unsigned rankMatch(sl::BasicType src, sl::BasicType dest)
    {
        if (src == dest) return 2;
        else if (isConvertible(src, dest)) return 1;
        else return 0;
    }

    static unsigned rankMatch(const std::vector<sl::BasicType>& src, const std::vector<sl::BasicType>& dest)
    {
        if (src.size() != dest.size()) return 0;

        unsigned rank = 2;

        for (std::vector<sl::BasicType>::size_type i = 0; i != src.size(); ++i)
        {
            unsigned r = rankMatch(src[i], dest[i]);

            if (r < rank) rank = r;
        }

        return rank;
    }
};

}
}
#endif /* SL_AST_FUNCTIONTABLE_HPP */
