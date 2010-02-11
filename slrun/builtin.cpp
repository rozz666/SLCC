#include "builtin.hpp"

namespace sl
{

namespace builtin
{

const char *operatorName(ast::Sign s)
{
    switch (s)
    {
        case ast::plus_: return "operator+";
        case ast::minus_: return "operator-";
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

const st::BuiltinFunction operator_minus_i(operatorName(ast::minus_), st::int_, st::int_);
const st::BuiltinFunction operator_minus_f(operatorName(ast::minus_), st::float_, st::float_);

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

}
}