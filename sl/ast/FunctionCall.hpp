#ifndef SL_AST_FUNCTIONCALL_HPP
#define SL_AST_FUNCTIONCALL_HPP

#include <vector>
#include <sl/FilePosition.hpp>
#include <sl/ast/FunctionRef.hpp>
#include <sl/ast/Expression.hpp>

namespace sl
{
namespace ast
{

class FunctionCall
{
public:

    typedef std::vector<Expression> ParamContainer;

    template <typename It>    
    FunctionCall(const FilePosition& pos, FunctionRef f, It firstParam, It lastParam)
        : pos_(pos), f_(f), params_(firstParam, lastParam) { }

    FilePosition pos() const { return pos_; }
    const FunctionRef& f() const { return f_; }
    const ParamContainer& params() const { return params_; }

private:

    FilePosition pos_;
    FunctionRef f_;
    ParamContainer params_;
};

}
}
#endif /* SL_AST_FUNCTIONCALL_HPP */
