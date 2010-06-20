#ifndef SL_AST_CAST_HPP
#define SL_AST_CAST_HPP

#include <sl/FilePosition.hpp>
#include <sl/basicTypes.hpp>
#include <sl/ast/Expression.hpp>

namespace sl
{
namespace ast
{

class Cast
{
public:

    Cast(const FilePosition& pos, const Expression& expr, BasicType type) : pos_(pos), expr_(expr), type_(type) { }

    FilePosition pos() const { return pos_; }
    const Expression& expr() const { return expr_; }
    BasicType type() const { return type_; }

private:

    FilePosition pos_;
    Expression expr_;
    BasicType type_;
};

}
}
#endif /* SL_AST_CAST_HPP */
