#include "builtin.hpp"

namespace sl
{

namespace builtin
{

const char *operatorName(ast::UnOp s)
{
    switch (s)
    {
        case ast::plus_: return "operator+";
        case ast::minus_: return "operator-";
        case ast::lnot_: return "operator!";
    }

    assert(!"Not all operator handled");

    return "";
}

const char *operatorName(ast::MulOp o)
{
    switch (o)
    {
        case ast::mul_: return "operator*";
        case ast::div_: return "operator/";
        case ast::mod_: return "operator%";
    }

    assert(!"Not all operator handled");

    return "";
}

const char *operatorName(ast::RelOp o)
{
    switch (o)
    {
        case ast::less_: return "operator<";
        case ast::lessEqual_: return "operator<=";
        case ast::greater_: return "operator>";
        case ast::greaterEqual_: return "operator>=";
    }

    assert(!"Not all operator handled");

    return "";
}

const char *operatorName(ast::EqOp o)
{
    switch (o)
    {
        case ast::equal_: return "operator==";
        case ast::notEqual_: return "operator!=";
    }

    assert(!"Not all operator handled");

    return "";
}

const char *operatorName(ast::LAndOp)
{
    return "operator&&";
}

const char *operatorName(ast::LOrOp)
{
    return "operator||";
}

const st::BuiltinFunction operator_plus_i(operatorName(ast::plus_), st::int_, st::int_);
const st::BuiltinFunction operator_plus_f(operatorName(ast::plus_), st::float_, st::float_);
const st::BuiltinFunction operator_minus_i(operatorName(ast::minus_), st::int_, st::int_);
const st::BuiltinFunction operator_minus_f(operatorName(ast::minus_), st::float_, st::float_);
const st::BuiltinFunction operator_lnot_b(operatorName(ast::lnot_), st::bool_, st::bool_);

const st::BuiltinFunction operator_plus_ii(operatorName(ast::plus_), st::int_, st::int_, st::int_);
const st::BuiltinFunction operator_plus_fi(operatorName(ast::plus_), st::float_, st::int_, st::float_);
const st::BuiltinFunction operator_plus_if(operatorName(ast::plus_), st::int_, st::float_, st::float_);
const st::BuiltinFunction operator_plus_ff(operatorName(ast::plus_), st::float_, st::float_, st::float_);

const st::BuiltinFunction operator_minus_ii(operatorName(ast::minus_), st::int_, st::int_, st::int_);
const st::BuiltinFunction operator_minus_fi(operatorName(ast::minus_), st::float_, st::int_, st::float_);
const st::BuiltinFunction operator_minus_if(operatorName(ast::minus_), st::int_, st::float_, st::float_);
const st::BuiltinFunction operator_minus_ff(operatorName(ast::minus_), st::float_, st::float_, st::float_);

const st::BuiltinFunction operator_mul_ii(operatorName(ast::mul_), st::int_, st::int_, st::int_);
const st::BuiltinFunction operator_mul_fi(operatorName(ast::mul_), st::float_, st::int_, st::float_);
const st::BuiltinFunction operator_mul_if(operatorName(ast::mul_), st::int_, st::float_, st::float_);
const st::BuiltinFunction operator_mul_ff(operatorName(ast::mul_), st::float_, st::float_, st::float_);

const st::BuiltinFunction operator_div_ii(operatorName(ast::div_), st::int_, st::int_, st::int_);
const st::BuiltinFunction operator_div_fi(operatorName(ast::div_), st::float_, st::int_, st::float_);
const st::BuiltinFunction operator_div_if(operatorName(ast::div_), st::int_, st::float_, st::float_);
const st::BuiltinFunction operator_div_ff(operatorName(ast::div_), st::float_, st::float_, st::float_);

const st::BuiltinFunction operator_mod_ii(operatorName(ast::mod_), st::int_, st::int_, st::int_);
const st::BuiltinFunction operator_mod_fi(operatorName(ast::mod_), st::float_, st::int_, st::float_);
const st::BuiltinFunction operator_mod_if(operatorName(ast::mod_), st::int_, st::float_, st::float_);
const st::BuiltinFunction operator_mod_ff(operatorName(ast::mod_), st::float_, st::float_, st::float_);

const st::BuiltinFunction operator_lt_ii(operatorName(ast::less_), st::int_, st::int_, st::bool_);
const st::BuiltinFunction operator_lt_fi(operatorName(ast::less_), st::float_, st::int_, st::bool_);
const st::BuiltinFunction operator_lt_if(operatorName(ast::less_), st::int_, st::float_, st::bool_);
const st::BuiltinFunction operator_lt_ff(operatorName(ast::less_), st::float_, st::float_, st::bool_);

const st::BuiltinFunction operator_le_ii(operatorName(ast::lessEqual_), st::int_, st::int_, st::bool_);
const st::BuiltinFunction operator_le_fi(operatorName(ast::lessEqual_), st::float_, st::int_, st::bool_);
const st::BuiltinFunction operator_le_if(operatorName(ast::lessEqual_), st::int_, st::float_, st::bool_);
const st::BuiltinFunction operator_le_ff(operatorName(ast::lessEqual_), st::float_, st::float_, st::bool_);

const st::BuiltinFunction operator_gt_ii(operatorName(ast::greater_), st::int_, st::int_, st::bool_);
const st::BuiltinFunction operator_gt_fi(operatorName(ast::greater_), st::float_, st::int_, st::bool_);
const st::BuiltinFunction operator_gt_if(operatorName(ast::greater_), st::int_, st::float_, st::bool_);
const st::BuiltinFunction operator_gt_ff(operatorName(ast::greater_), st::float_, st::float_, st::bool_);

const st::BuiltinFunction operator_ge_ii(operatorName(ast::greaterEqual_), st::int_, st::int_, st::bool_);
const st::BuiltinFunction operator_ge_fi(operatorName(ast::greaterEqual_), st::float_, st::int_, st::bool_);
const st::BuiltinFunction operator_ge_if(operatorName(ast::greaterEqual_), st::int_, st::float_, st::bool_);
const st::BuiltinFunction operator_ge_ff(operatorName(ast::greaterEqual_), st::float_, st::float_, st::bool_);

const st::BuiltinFunction operator_eq_ii(operatorName(ast::equal_), st::int_, st::int_, st::bool_);
const st::BuiltinFunction operator_eq_fi(operatorName(ast::equal_), st::float_, st::int_, st::bool_);
const st::BuiltinFunction operator_eq_if(operatorName(ast::equal_), st::int_, st::float_, st::bool_);
const st::BuiltinFunction operator_eq_ff(operatorName(ast::equal_), st::float_, st::float_, st::bool_);
const st::BuiltinFunction operator_eq_bb(operatorName(ast::equal_), st::bool_, st::bool_, st::bool_);

const st::BuiltinFunction operator_neq_ii(operatorName(ast::notEqual_), st::int_, st::int_, st::bool_);
const st::BuiltinFunction operator_neq_fi(operatorName(ast::notEqual_), st::float_, st::int_, st::bool_);
const st::BuiltinFunction operator_neq_if(operatorName(ast::notEqual_), st::int_, st::float_, st::bool_);
const st::BuiltinFunction operator_neq_ff(operatorName(ast::notEqual_), st::float_, st::float_, st::bool_);
const st::BuiltinFunction operator_neq_bb(operatorName(ast::notEqual_), st::bool_, st::bool_, st::bool_);

const st::BuiltinFunction operator_land_bb("operator&&", st::bool_, st::bool_, st::bool_);
const st::BuiltinFunction operator_lor_bb("operator||", st::bool_, st::bool_, st::bool_);

const st::BuiltinFunction function_swap_ii("swap", st::int_, st::int_, st::void_);
const st::BuiltinFunction function_swap_ff("swap", st::float_, st::float_, st::void_);
const st::BuiltinFunction function_swap_bb("swap", st::bool_, st::bool_, st::void_);

}
}