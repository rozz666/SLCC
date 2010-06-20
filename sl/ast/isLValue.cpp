#include <boost/variant/static_visitor.hpp>
#include <sl/ast/isLValue.hpp>

namespace sl
{
namespace ast
{

struct IsLValue : public boost::static_visitor<bool>
{
    bool operator()(const Variable *) const { return true; }

    template <typename T>
    bool operator()(const T& ) const { return false; }
};

bool isLValue(const Expression& e)
{
    IsLValue v;
    return e.apply_visitor(v);
}

}
}
