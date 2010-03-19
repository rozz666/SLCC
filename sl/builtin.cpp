#include <sl/builtin.hpp>

namespace sl
{

namespace builtin
{

const char *operatorName(cst::UnOp s)
{
    switch (s)
    {
        case cst::plus_: return "operator+";
        case cst::minus_: return "operator-";
        case cst::lnot_: return "operator!";
    }

    assert(!"Not all operator handled");

    return "";
}

const char *operatorName(cst::MulOp o)
{
    switch (o)
    {
        case cst::mul_: return "operator*";
        case cst::div_: return "operator/";
        case cst::mod_: return "operator%";
    }

    assert(!"Not all operator handled");

    return "";
}

const char *operatorName(cst::RelOp o)
{
    switch (o)
    {
        case cst::less_: return "operator<";
        case cst::lessEqual_: return "operator<=";
        case cst::greater_: return "operator>";
        case cst::greaterEqual_: return "operator>=";
    }

    assert(!"Not all operator handled");

    return "";
}

const char *operatorName(cst::EqOp o)
{
    switch (o)
    {
        case cst::equal_: return "operator==";
        case cst::notEqual_: return "operator!=";
    }

    assert(!"Not all operator handled");

    return "";
}

const char *operatorName(cst::LAndOp)
{
    return "operator&&";
}

const char *operatorName(cst::LOrOp)
{
    return "operator||";
}

const ast::BuiltinFunction operator_plus_i(operatorName(cst::plus_), ast::int_, ast::int_);
const ast::BuiltinFunction operator_plus_f(operatorName(cst::plus_), ast::float_, ast::float_);
const ast::BuiltinFunction operator_minus_i(operatorName(cst::minus_), ast::int_, ast::int_);
const ast::BuiltinFunction operator_minus_f(operatorName(cst::minus_), ast::float_, ast::float_);
const ast::BuiltinFunction operator_lnot_b(operatorName(cst::lnot_), ast::bool_, ast::bool_);

const ast::BuiltinFunction operator_plus_ii(operatorName(cst::plus_), ast::int_, ast::int_, ast::int_);
const ast::BuiltinFunction operator_plus_fi(operatorName(cst::plus_), ast::float_, ast::int_, ast::float_);
const ast::BuiltinFunction operator_plus_if(operatorName(cst::plus_), ast::int_, ast::float_, ast::float_);
const ast::BuiltinFunction operator_plus_ff(operatorName(cst::plus_), ast::float_, ast::float_, ast::float_);

const ast::BuiltinFunction operator_minus_ii(operatorName(cst::minus_), ast::int_, ast::int_, ast::int_);
const ast::BuiltinFunction operator_minus_fi(operatorName(cst::minus_), ast::float_, ast::int_, ast::float_);
const ast::BuiltinFunction operator_minus_if(operatorName(cst::minus_), ast::int_, ast::float_, ast::float_);
const ast::BuiltinFunction operator_minus_ff(operatorName(cst::minus_), ast::float_, ast::float_, ast::float_);

const ast::BuiltinFunction operator_mul_ii(operatorName(cst::mul_), ast::int_, ast::int_, ast::int_);
const ast::BuiltinFunction operator_mul_fi(operatorName(cst::mul_), ast::float_, ast::int_, ast::float_);
const ast::BuiltinFunction operator_mul_if(operatorName(cst::mul_), ast::int_, ast::float_, ast::float_);
const ast::BuiltinFunction operator_mul_ff(operatorName(cst::mul_), ast::float_, ast::float_, ast::float_);

const ast::BuiltinFunction operator_div_ii(operatorName(cst::div_), ast::int_, ast::int_, ast::int_);
const ast::BuiltinFunction operator_div_fi(operatorName(cst::div_), ast::float_, ast::int_, ast::float_);
const ast::BuiltinFunction operator_div_if(operatorName(cst::div_), ast::int_, ast::float_, ast::float_);
const ast::BuiltinFunction operator_div_ff(operatorName(cst::div_), ast::float_, ast::float_, ast::float_);

const ast::BuiltinFunction operator_mod_ii(operatorName(cst::mod_), ast::int_, ast::int_, ast::int_);
const ast::BuiltinFunction operator_mod_fi(operatorName(cst::mod_), ast::float_, ast::int_, ast::float_);
const ast::BuiltinFunction operator_mod_if(operatorName(cst::mod_), ast::int_, ast::float_, ast::float_);
const ast::BuiltinFunction operator_mod_ff(operatorName(cst::mod_), ast::float_, ast::float_, ast::float_);

const ast::BuiltinFunction operator_lt_ii(operatorName(cst::less_), ast::int_, ast::int_, ast::bool_);
const ast::BuiltinFunction operator_lt_fi(operatorName(cst::less_), ast::float_, ast::int_, ast::bool_);
const ast::BuiltinFunction operator_lt_if(operatorName(cst::less_), ast::int_, ast::float_, ast::bool_);
const ast::BuiltinFunction operator_lt_ff(operatorName(cst::less_), ast::float_, ast::float_, ast::bool_);

const ast::BuiltinFunction operator_le_ii(operatorName(cst::lessEqual_), ast::int_, ast::int_, ast::bool_);
const ast::BuiltinFunction operator_le_fi(operatorName(cst::lessEqual_), ast::float_, ast::int_, ast::bool_);
const ast::BuiltinFunction operator_le_if(operatorName(cst::lessEqual_), ast::int_, ast::float_, ast::bool_);
const ast::BuiltinFunction operator_le_ff(operatorName(cst::lessEqual_), ast::float_, ast::float_, ast::bool_);

const ast::BuiltinFunction operator_gt_ii(operatorName(cst::greater_), ast::int_, ast::int_, ast::bool_);
const ast::BuiltinFunction operator_gt_fi(operatorName(cst::greater_), ast::float_, ast::int_, ast::bool_);
const ast::BuiltinFunction operator_gt_if(operatorName(cst::greater_), ast::int_, ast::float_, ast::bool_);
const ast::BuiltinFunction operator_gt_ff(operatorName(cst::greater_), ast::float_, ast::float_, ast::bool_);

const ast::BuiltinFunction operator_ge_ii(operatorName(cst::greaterEqual_), ast::int_, ast::int_, ast::bool_);
const ast::BuiltinFunction operator_ge_fi(operatorName(cst::greaterEqual_), ast::float_, ast::int_, ast::bool_);
const ast::BuiltinFunction operator_ge_if(operatorName(cst::greaterEqual_), ast::int_, ast::float_, ast::bool_);
const ast::BuiltinFunction operator_ge_ff(operatorName(cst::greaterEqual_), ast::float_, ast::float_, ast::bool_);

const ast::BuiltinFunction operator_eq_ii(operatorName(cst::equal_), ast::int_, ast::int_, ast::bool_);
const ast::BuiltinFunction operator_eq_fi(operatorName(cst::equal_), ast::float_, ast::int_, ast::bool_);
const ast::BuiltinFunction operator_eq_if(operatorName(cst::equal_), ast::int_, ast::float_, ast::bool_);
const ast::BuiltinFunction operator_eq_ff(operatorName(cst::equal_), ast::float_, ast::float_, ast::bool_);
const ast::BuiltinFunction operator_eq_bb(operatorName(cst::equal_), ast::bool_, ast::bool_, ast::bool_);

const ast::BuiltinFunction operator_neq_ii(operatorName(cst::notEqual_), ast::int_, ast::int_, ast::bool_);
const ast::BuiltinFunction operator_neq_fi(operatorName(cst::notEqual_), ast::float_, ast::int_, ast::bool_);
const ast::BuiltinFunction operator_neq_if(operatorName(cst::notEqual_), ast::int_, ast::float_, ast::bool_);
const ast::BuiltinFunction operator_neq_ff(operatorName(cst::notEqual_), ast::float_, ast::float_, ast::bool_);
const ast::BuiltinFunction operator_neq_bb(operatorName(cst::notEqual_), ast::bool_, ast::bool_, ast::bool_);

const ast::BuiltinFunction operator_land_bb("operator&&", ast::bool_, ast::bool_, ast::bool_);
const ast::BuiltinFunction operator_lor_bb("operator||", ast::bool_, ast::bool_, ast::bool_);

const ast::BuiltinFunction function_swap_ii("swap", ast::int_, ast::int_, ast::void_);
const ast::BuiltinFunction function_swap_ff("swap", ast::float_, ast::float_, ast::void_);
const ast::BuiltinFunction function_swap_bb("swap", ast::bool_, ast::bool_, ast::void_);

const ast::BuiltinFunction function_geti("geti", ast::int_);
const ast::BuiltinFunction function_getf("getf", ast::float_);
const ast::BuiltinFunction function_put_i("put", ast::int_, ast::void_);
const ast::BuiltinFunction function_put_f("put", ast::float_, ast::void_);

}
}