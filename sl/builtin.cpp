#include <sl/basicTypes.hpp>
#include <sl/cst/operators.hpp>
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

const ast::BuiltinFunction operator_plus_i(operatorName(cst::plus_), int_, int_);
const ast::BuiltinFunction operator_plus_f(operatorName(cst::plus_), float_, float_);
const ast::BuiltinFunction operator_minus_i(operatorName(cst::minus_), int_, int_);
const ast::BuiltinFunction operator_minus_f(operatorName(cst::minus_), float_, float_);
const ast::BuiltinFunction operator_lnot_b(operatorName(cst::lnot_), bool_, bool_);

const ast::BuiltinFunction operator_plus_ii(operatorName(cst::plus_), int_, int_, int_);
const ast::BuiltinFunction operator_plus_fi(operatorName(cst::plus_), float_, int_, float_);
const ast::BuiltinFunction operator_plus_if(operatorName(cst::plus_), int_, float_, float_);
const ast::BuiltinFunction operator_plus_ff(operatorName(cst::plus_), float_, float_, float_);

const ast::BuiltinFunction operator_minus_ii(operatorName(cst::minus_), int_, int_, int_);
const ast::BuiltinFunction operator_minus_fi(operatorName(cst::minus_), float_, int_, float_);
const ast::BuiltinFunction operator_minus_if(operatorName(cst::minus_), int_, float_, float_);
const ast::BuiltinFunction operator_minus_ff(operatorName(cst::minus_), float_, float_, float_);

const ast::BuiltinFunction operator_mul_ii(operatorName(cst::mul_), int_, int_, int_);
const ast::BuiltinFunction operator_mul_fi(operatorName(cst::mul_), float_, int_, float_);
const ast::BuiltinFunction operator_mul_if(operatorName(cst::mul_), int_, float_, float_);
const ast::BuiltinFunction operator_mul_ff(operatorName(cst::mul_), float_, float_, float_);

const ast::BuiltinFunction operator_div_ii(operatorName(cst::div_), int_, int_, int_);
const ast::BuiltinFunction operator_div_fi(operatorName(cst::div_), float_, int_, float_);
const ast::BuiltinFunction operator_div_if(operatorName(cst::div_), int_, float_, float_);
const ast::BuiltinFunction operator_div_ff(operatorName(cst::div_), float_, float_, float_);

const ast::BuiltinFunction operator_mod_ii(operatorName(cst::mod_), int_, int_, int_);
const ast::BuiltinFunction operator_mod_fi(operatorName(cst::mod_), float_, int_, float_);
const ast::BuiltinFunction operator_mod_if(operatorName(cst::mod_), int_, float_, float_);
const ast::BuiltinFunction operator_mod_ff(operatorName(cst::mod_), float_, float_, float_);

const ast::BuiltinFunction operator_lt_ii(operatorName(cst::less_), int_, int_, bool_);
const ast::BuiltinFunction operator_lt_fi(operatorName(cst::less_), float_, int_, bool_);
const ast::BuiltinFunction operator_lt_if(operatorName(cst::less_), int_, float_, bool_);
const ast::BuiltinFunction operator_lt_ff(operatorName(cst::less_), float_, float_, bool_);

const ast::BuiltinFunction operator_le_ii(operatorName(cst::lessEqual_), int_, int_, bool_);
const ast::BuiltinFunction operator_le_fi(operatorName(cst::lessEqual_), float_, int_, bool_);
const ast::BuiltinFunction operator_le_if(operatorName(cst::lessEqual_), int_, float_, bool_);
const ast::BuiltinFunction operator_le_ff(operatorName(cst::lessEqual_), float_, float_, bool_);

const ast::BuiltinFunction operator_gt_ii(operatorName(cst::greater_), int_, int_, bool_);
const ast::BuiltinFunction operator_gt_fi(operatorName(cst::greater_), float_, int_, bool_);
const ast::BuiltinFunction operator_gt_if(operatorName(cst::greater_), int_, float_, bool_);
const ast::BuiltinFunction operator_gt_ff(operatorName(cst::greater_), float_, float_, bool_);

const ast::BuiltinFunction operator_ge_ii(operatorName(cst::greaterEqual_), int_, int_, bool_);
const ast::BuiltinFunction operator_ge_fi(operatorName(cst::greaterEqual_), float_, int_, bool_);
const ast::BuiltinFunction operator_ge_if(operatorName(cst::greaterEqual_), int_, float_, bool_);
const ast::BuiltinFunction operator_ge_ff(operatorName(cst::greaterEqual_), float_, float_, bool_);

const ast::BuiltinFunction operator_eq_ii(operatorName(cst::equal_), int_, int_, bool_);
const ast::BuiltinFunction operator_eq_fi(operatorName(cst::equal_), float_, int_, bool_);
const ast::BuiltinFunction operator_eq_if(operatorName(cst::equal_), int_, float_, bool_);
const ast::BuiltinFunction operator_eq_ff(operatorName(cst::equal_), float_, float_, bool_);
const ast::BuiltinFunction operator_eq_bb(operatorName(cst::equal_), bool_, bool_, bool_);

const ast::BuiltinFunction operator_neq_ii(operatorName(cst::notEqual_), int_, int_, bool_);
const ast::BuiltinFunction operator_neq_fi(operatorName(cst::notEqual_), float_, int_, bool_);
const ast::BuiltinFunction operator_neq_if(operatorName(cst::notEqual_), int_, float_, bool_);
const ast::BuiltinFunction operator_neq_ff(operatorName(cst::notEqual_), float_, float_, bool_);
const ast::BuiltinFunction operator_neq_bb(operatorName(cst::notEqual_), bool_, bool_, bool_);

const ast::BuiltinFunction operator_land_bb("operator&&", bool_, bool_, bool_);
const ast::BuiltinFunction operator_lor_bb("operator||", bool_, bool_, bool_);

const ast::BuiltinFunction function_swap_ii("swap", int_, int_, void_);
const ast::BuiltinFunction function_swap_ff("swap", float_, float_, void_);
const ast::BuiltinFunction function_swap_bb("swap", bool_, bool_, void_);

const ast::BuiltinFunction function_geti("geti", int_);
const ast::BuiltinFunction function_getf("getf", float_);
const ast::BuiltinFunction function_put_i("put", int_, void_);
const ast::BuiltinFunction function_put_f("put", float_, void_);

}
}