#include <sl/ast/Constant.hpp>

namespace sl
{
namespace ast
{

struct ConstantType : public boost::static_visitor<BasicType>
{
    BasicType operator()(const int& ) const
    {
        return int_;
    }

    BasicType operator()(const float& ) const
    {
        return float_;
    }

    BasicType operator()(const bool& ) const
    {
        return bool_;
    }
};

BasicType Constant::type() const
{
    ConstantType ct;
    return value_.apply_visitor(ct);
}


}
}
