#ifndef SL_CST_OPERATORS_HPP
#define SL_CST_OPERATORS_HPP

namespace sl
{
namespace cst
{

enum UnOp
{
    plus_,
    minus_,
    lnot_
};

enum MulOp
{
    mul_,
    div_,
    mod_
};

enum RelOp
{
    less_,
    lessEqual_,
    greater_,
    greaterEqual_
};

enum EqOp
{
    equal_,
    notEqual_
};

enum LAndOp
{
    and_
};

enum LOrOp
{
    or_
};

}
}
#endif /* SL_CST_OPERATORS_HPP */
